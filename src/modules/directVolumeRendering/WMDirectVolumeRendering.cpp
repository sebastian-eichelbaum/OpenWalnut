//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include "core/common/WColor.h"
#include "core/common/WPropertyHelper.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/shaders/WGEShaderDefine.h"
#include "core/graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/WGEManagedGroupNode.h"
#include "core/graphicsEngine/WGERequirement.h"
#include "core/graphicsEngine/WGETextureUtils.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/kernel/WKernel.h"
#include "WMDirectVolumeRendering.h"
#include "WMDirectVolumeRendering.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMDirectVolumeRendering )

WMDirectVolumeRendering::WMDirectVolumeRendering():
    WModule()
{
    // Initialize members
}

WMDirectVolumeRendering::~WMDirectVolumeRendering()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMDirectVolumeRendering::factory() const
{
    return boost::shared_ptr< WModule >( new WMDirectVolumeRendering() );
}

const char** WMDirectVolumeRendering::getXPMIcon() const
{
    return WMDirectVolumeRendering_xpm;
}

const std::string WMDirectVolumeRendering::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Direct Volume Rendering";
}

const std::string WMDirectVolumeRendering::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "Direct volume rendering of regular volumetric data.";
}

void WMDirectVolumeRendering::connectors()
{
    // The transfer function for our DVR
    m_transferFunction = WModuleInputData< WDataSetSingle >::createAndAdd( shared_from_this(), "transfer function", "The 1D transfer function." );

    // DVR needs one input: the scalar dataset
    m_input = WModuleInputData< WDataSetScalar >::createAndAdd( shared_from_this(), "in", "The scalar dataset." );

    // Optional: the gradient field
    m_gradients = WModuleInputData< WDataSetVector >::createAndAdd( shared_from_this(),
            "gradients", "The gradient field of the dataset to display" );

    // call WModules initialization
    WModule::connectors();
}

void WMDirectVolumeRendering::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_samples     = m_properties->addProperty( "Sample count", "The number of samples to walk along the ray during raycasting. A low value "
            "may cause artifacts whilst a high value slows down rendering.", 256 );
    m_samples->setMin( 1 );
    m_samples->setMax( 5000 );

    // illumination model
    m_localIlluminationSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_localIlluminationSelections->addItem( "No Local Illumination", "Volume Renderer only uses the classified voxel color." );
    m_localIlluminationSelections->addItem( "Blinn-Phong", "Blinn-Phong lighting is used for shading each classified voxel." );
    m_localIlluminationAlgo = m_properties->addProperty( "Local illumination model", "The illumination algorithm to use.",
            m_localIlluminationSelections->getSelectorFirst(), m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_localIlluminationAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_localIlluminationAlgo );

    // additional artifact removal methods
    m_improvementGroup = m_properties->addPropertyGroup( "Improvements", "Methods for improving image quality. Most of these methods imply "
            "additional calculation/texture overhead and therefore slow down rendering." );

    m_stochasticJitterEnabled = m_improvementGroup->addProperty( "Stochastic jitter", "With stochastic jitter, wood-grain artifacts can be "
            "removed with the cost of possible noise artifacts.", true,
            m_propCondition );

    m_opacityCorrectionEnabled = m_improvementGroup->addProperty( "Opacity correction", "If enabled, opacities are assumed to be relative to the "
            "sample count. If disabled, changing the sample count "
            "varies brightness of the image.", true,
            m_propCondition );

    m_maximumIntensityProjectionEnabled = m_improvementGroup->addProperty( "MIP", "If enabled, MIP is used.", false,
            m_propCondition );

    m_depthProjectionEnabled = m_improvementGroup->addProperty( "Depth Projection", "If enabled, depth projection mode is used", false,
            m_propCondition );

    WModule::properties();
}

void WMDirectVolumeRendering::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

/**
 * Generates a white noise texture with given resolution.
 *
 * \param resX the resolution
 *
 * \return a image with resX*resX resolution.
 */
osg::ref_ptr< osg::Image > genWhiteNoise( size_t resX )
{
    std::srand( time( 0 ) );

    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    randImage->allocateImage( resX, resX, 1, GL_LUMINANCE, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( unsigned int x = 0; x < resX; x++ )
    {
        for( unsigned int y = 0; y < resX; y++ )
        {
            // - stylechecker says "use rand_r" but I am not sure about portability.
            unsigned char r = ( unsigned char )( std::rand() % 255 );  // NOLINT
            randomLuminance[ ( y * resX ) + x ] = r;
        }
    }

    return randImage;
}

void WMDirectVolumeRendering::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMDirectVolumeRendering", m_localPath ) );
    // setup all the defines needed

    // local illumination model
    WGEShaderDefineOptions::SPtr illuminationAlgoDefines = WGEShaderDefineOptions::SPtr(
            new WGEShaderDefineOptions( "LOCALILLUMINATION_NONE", "LOCALILLUMINATION_PHONG" )
            );
    m_shader->addPreprocessor( illuminationAlgoDefines );

    // gradient texture settings
    WGEShaderDefine< std::string >::SPtr gradTexSamplerDefine = m_shader->setDefine( "GRADIENTTEXTURE_SAMPLER", std::string( "tex1" ) );
    WGEShaderDefineSwitch::SPtr gradTexEnableDefine = m_shader->setDefine( "GRADIENTTEXTURE_ENABLED" );

    // transfer function texture settings
    WGEShaderDefine< std::string >::SPtr tfTexSamplerDefine = m_shader->setDefine( "TRANSFERFUNCTION_SAMPLER", std::string( "tex2" ) );
    WGEShaderDefineSwitch::SPtr tfTexEnableDefine = m_shader->setDefine( "TRANSFERFUNCTION_ENABLED" );

    // jitter
    WGEShaderDefine< std::string >::SPtr jitterSamplerDefine = m_shader->setDefine( "JITTERTEXTURE_SAMPLER", std::string( "tex3" ) );
    WGEShaderDefine< int >::SPtr jitterSizeXDefine = m_shader->setDefine( "JITTERTEXTURE_SIZEX", 0 );
    WGEShaderDefineSwitch::SPtr jitterEnable = m_shader->setDefine( "JITTERTEXTURE_ENABLED" );

    // opacity correction enabled?
    WGEShaderDefineSwitch::SPtr opacityCorrectionEnableDefine = m_shader->setDefine( "OPACITYCORRECTION_ENABLED" );

    WGEShaderDefineSwitch::SPtr maximumIntensityProjectionEnabledDefine = m_shader->setDefine( "MIP_ENABLED" );
    WGEShaderDefineSwitch::SPtr depthProjectionEnabledDefine = m_shader->setDefine( "DEPTH_PROJECTION_ENABLED" );

    // the texture used for the transfer function
    osg::ref_ptr< osg::Texture1D > tfTexture = new osg::Texture1D();
    osg::ref_ptr< osg::Image > tfImage = new osg::Image();
    bool updateTF = false;  // if true, update of TF is enforced

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_transferFunction->getDataChangedCondition() );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_gradients->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    osg::ref_ptr< WGEManagedGroupNode > rootNode = new WGEManagedGroupNode( m_active );
    bool rootInserted = false;

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        m_moduleState.wait();

        // quit if requested
        if( m_shutdownFlag() )
        {
            break;
        }

        // was there an update?
        bool dataUpdated = m_input->updated() || m_gradients->updated();
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        bool dataValid   = ( dataSet != NULL );
        bool propUpdated = m_localIlluminationAlgo->changed() || m_stochasticJitterEnabled->changed() ||  m_opacityCorrectionEnabled->changed() ||
            m_maximumIntensityProjectionEnabled->changed() || m_depthProjectionEnabled->changed();


        // reset module in case of invalid data. This accounts only for the scalar field input
        if( !dataValid )
        {
            cube.release();
            debugLog() << "Resetting.";
            rootNode->clear();
            continue;
        }

        // As the data has changed, we need to recreate the texture.
        if( ( propUpdated || dataUpdated ) && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // there are several updates. Clear the root node and later on insert the new rendering.
            rootNode->clear();

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::dynamic_pointer_cast< WGridRegular3D >( dataSet->getGrid() );
            if( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // use the OSG Shapes, create unit cube
            WBoundingBox bb( WPosition( 0.0, 0.0, 0.0 ),
                    WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) );
            cube = wge::generateSolidBoundingBoxNode( bb, WColor( 1.0, 1.0, 1.0, 1.0 ) );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
            // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( static_cast< WMatrix4d >( grid->getTransform() ) );

            m_shader->apply( cube );

            // bind the texture to the node
            osg::ref_ptr< WDataTexture3D > texture3D = dataSet->getTexture();
            wge::bindTexture( cube, texture3D, 0, "u_volume" );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup illumination
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // enable transparency
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            rootState->setMode( GL_BLEND, osg::StateAttribute::ON );
            rootState->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

            // set proper illumination define
            illuminationAlgoDefines->activateOption( m_localIlluminationAlgo->get( true ).getItemIndexOfSelected( 0 ) );

            // if there is a gradient field available -> apply as texture too
            boost::shared_ptr< WDataSetVector > gradients = m_gradients->getData();
            if( gradients )
            {
                debugLog() << "Uploading specified gradient field.";

                // bind the texture to the node
                osg::ref_ptr< WDataTexture3D > gradTexture3D = gradients->getTexture();
                wge::bindTexture( cube, gradTexture3D, 1, "u_gradients" );
                gradTexEnableDefine->setActive( true );
            }
            else
            {
                gradTexEnableDefine->setActive( false ); // disable gradient texture
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // stochastic jittering texture
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // create some random noise
            jitterSamplerDefine->setActive( false );
            jitterEnable->setActive( false );
            if( m_stochasticJitterEnabled->get( true ) )
            {
                const size_t size = 64;
                osg::ref_ptr< WGETexture2D > randTexture = new WGETexture2D( genWhiteNoise( size ) );
                randTexture->setFilterMinMag( osg::Texture2D::NEAREST );
                randTexture->setWrapSTR( osg::Texture2D::REPEAT );
                wge::bindTexture( cube, randTexture, 2, "u_jitter" );
                jitterSamplerDefine->setActive( true );
                jitterEnable->setActive( true );
                jitterSizeXDefine->setValue( size );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // transfer function texture
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            osg::ref_ptr< osg::Texture1D > tfTexture = new osg::Texture1D();
            tfTexture->setDataVariance( osg::Object::DYNAMIC );
            // create some ramp as default
            {
                int resX = 32;
                tfImage->allocateImage( resX, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );
                unsigned char *data = tfImage->data();  // should be 4 megs
                for( int x = 0; x < resX; x++ )
                {
                    unsigned char r = ( unsigned char )( 0.1 * 255.0 * static_cast< float >( x ) / static_cast< float >( resX ) );
                    data[ 4 * x + 0 ] = 255;
                    data[ 4 * x + 1 ] = 255;
                    data[ 4 * x + 2 ] = 255;
                    data[ 4 * x + 3 ] = r;
                }
            }

            tfTexture->setImage( tfImage );
            wge::bindTexture( cube, tfTexture, 3, "u_transferFunction" );
            // permanently enable the TF texture. As we have no alternative way to set the TF, always use a TF texture
            tfTexEnableDefine->setActive( true );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // opacity correction
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_opacityCorrectionEnabled->get( true ) )
            {
                opacityCorrectionEnableDefine->setActive( true );
            }
            else
            {
                opacityCorrectionEnableDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // maximum intensity projection (MIP)
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_maximumIntensityProjectionEnabled->get( true ) )
            {
                maximumIntensityProjectionEnabledDefine->setActive( true );
            }
            else
            {
                maximumIntensityProjectionEnabledDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // depth projection
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            if( m_depthProjectionEnabled->get( true ) )
            {
                depthProjectionEnabledDefine->setActive( true );
            }
            else
            {
                depthProjectionEnabledDefine->setActive( false );
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            rootState->addUniform( new WGEPropertyUniform< WPropInt >( "u_samples", m_samples ) );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // build spatial search structure
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // update node
            debugLog() << "Adding new rendering.";
            rootNode->insert( cube );
            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if( !rootInserted )
            {
                rootInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
            }

            updateTF = true;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // load transfer function
        ////////////////////////////////////////////////////////////////////////////////////////////////////

        if( ( updateTF || propUpdated || m_transferFunction->updated() ) && dataValid && cube )
        {
            updateTF = false;
            boost::shared_ptr< WDataSetSingle > dataSet = m_transferFunction->getData();
            if( !dataSet )
            {
                debugLog() << "no data set?";
            }
            else
            {
                WAssert( dataSet, "data set" );
                boost::shared_ptr< WValueSetBase > valueSet = dataSet->getValueSet();
                WAssert( valueSet, "value set" );
                boost::shared_ptr< WValueSet< unsigned char > > cvalueSet( boost::dynamic_pointer_cast<WValueSet< unsigned char> >( valueSet ) );
                if( !cvalueSet )
                {
                    debugLog() << "invalid type";
                }
                else
                {
                    size_t tfsize = cvalueSet->rawSize();

                    // create image and copy the TF
                    tfImage->allocateImage( tfsize/4, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE );
                    tfImage->setInternalTextureFormat( GL_RGBA );
                    unsigned char* data = reinterpret_cast< unsigned char* >( tfImage->data() );
                    std::copy( cvalueSet->rawData(), &cvalueSet->rawData()[ tfsize ], data );

                    // force OpenGl to use the new texture
                    tfTexture->dirtyTextureObject();
                }
            }
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}

