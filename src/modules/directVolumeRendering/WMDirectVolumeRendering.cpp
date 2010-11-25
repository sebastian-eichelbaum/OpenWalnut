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

#include <string>
#include <utility>

#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/StateAttribute>
#include <osgDB/ReadFile>

#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGERequirement.h"
#include "../../kernel/WKernel.h"
#include "WMDirectVolumeRendering.xpm"
#include "WMDirectVolumeRendering.h"

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
    // DVR needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "The scalar dataset shown using isosurface." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMDirectVolumeRendering::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_stepCount     = m_properties->addProperty( "Step count",       "The number of steps to walk along the ray during raycasting. A low value "
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    // illumination model
    m_localIlluminationSelections = boost::shared_ptr< WItemSelection >( new WItemSelection() );
    m_localIlluminationSelections->addItem( "No Local Illumination", "Volume Renderer only uses the classified voxel color." );
    m_localIlluminationSelections->addItem( "Blinn-Phong", "Blinn-Phong lighting is used for shading each classified voxel." );
    m_localIlluminationAlgo = m_properties->addProperty( "Local Illumination Model", "The illumination algorithm to use.",
                                                         m_localIlluminationSelections->getSelectorFirst(), m_propCondition );

    WPropertyHelper::PC_SELECTONLYONE::addTo( m_localIlluminationAlgo );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_localIlluminationAlgo );

    // transfer functions
    m_tfLoaderGroup = m_properties->addPropertyGroup( "Transfer Function", "Transfer Function loading." );
    m_tfLoaderEnabled = m_tfLoaderGroup->addProperty( "Enable", "Enable TF loading from file.", false, m_propCondition );
    m_tfLoaderFile = m_tfLoaderGroup->addProperty( "File", "The 1D image containing the transfer function.",
                                                    WPathHelper::getAppPath(), m_propCondition );
    WPropertyHelper::PC_PATHEXISTS::addTo( m_tfLoaderFile );
    m_tfLoaderTrigger = m_tfLoaderGroup->addProperty( "Load", "Triggers loading.", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition );
    WModule::properties();
}

void WMDirectVolumeRendering::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

void WMDirectVolumeRendering::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "WMDirectVolumeRendering", m_localPath ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    osg::ref_ptr< WGEManagedGroupNode > rootNode = new WGEManagedGroupNode( m_active );
    bool rootInserted = false;

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // quit if requested
        if ( m_shutdownFlag() )
        {
            break;
        }

        // was there an update?
        bool dataUpdated = m_input->updated();
        boost::shared_ptr< WDataSetScalar > dataSet = m_input->getData();
        bool dataValid   = ( dataSet );
        bool propUpdated = m_localIlluminationAlgo->changed() || m_tfLoaderEnabled || m_tfLoaderFile->changed() || m_tfLoaderTrigger->changed();

        // As the data has changed, we need to recreate the texture.
        if ( ( propUpdated || dataUpdated ) && dataValid )
        {
            debugLog() << "Data changed. Uploading new data as texture.";

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // use the OSG Shapes, create unit cube
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode(
                wmath::WPosition( 0.0, 0.0, 0.0 ),
                wmath::WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ),
                WColor( 1.0, 1.0, 1.0, 1.0 )
            );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
                                                                              // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( wge::toOSGMatrix( grid->getTransformationMatrix() ) );

            m_shader->apply( cube );

            // bind the texture to the node
            osg::ref_ptr< osg::Texture3D > texture3D = dataSet->getTexture()->getTexture();
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );

            // enable transparency
            rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup defines (illumination)
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            size_t localIlluminationAlgo = m_localIlluminationAlgo->get( true ).getItemIndexOfSelected( 0 );
            m_shader->eraseAllDefines();
            switch ( localIlluminationAlgo )
            {
                case Phong:
                    m_shader->setDefine( "LOCALILLUMINATION_PHONG" );
                    break;
                default:
                    break;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // load transfer function
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // try to load the tf from file if existent
            if ( m_tfLoaderEnabled->get( true ) )
            {
                osg::ref_ptr< osg::Image > tfImg = osgDB::readImageFile( m_tfLoaderFile->get( true ).file_string() );
                if ( tfImg )
                {
                    // bind it as a texture
                    osg::ref_ptr< osg::Texture1D > tfTexture = new osg::Texture1D();
                    tfTexture->setImage( tfImg );

                    // apply it
                    rootState->setTextureAttributeAndModes( 1, tfTexture, osg::StateAttribute::ON );
                    rootState->addUniform( new osg::Uniform( "tex1", 1 ) );
                    m_shader->setDefine( "TRANSFERFUNCTION_SAMPLER", "tex1" );
                    m_shader->setDefine( "TRANSFERFUNCTION_ENABLED" );
                }
                else
                {
                    warnLog() << "Transfer function texture could not be loaded from " << m_tfLoaderFile->get( true ).file_string() << ".";
                }
            }

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // for the texture, also bind the appropriate uniforms
            rootState->addUniform( new osg::Uniform( "tex0", 0 ) );

            osg::ref_ptr< osg::Uniform > steps = new osg::Uniform( "u_steps", m_stepCount->get() );
            steps->setUpdateCallback( new SafeUniformCallback( this ) );

            rootState->addUniform( steps );

            WGEColormapping::apply( cube, false );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // build spatial search structure
            ////////////////////////////////////////////////////////////////////////////////////////////////////


            // update node
            debugLog() << "Adding new rendering.";
            rootNode->clear();
            rootNode->insert( cube );
            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if ( !rootInserted )
            {
                rootInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( rootNode );
            }

            // finally, reset the load trigger
            m_tfLoaderTrigger->set( WPVBaseTypes::PV_TRIGGER_READY );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( rootNode );
}

void WMDirectVolumeRendering::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
{
    // update some uniforms:
    if ( m_module->m_stepCount->changed() && ( uniform->getName() == "u_steps" ) )
    {
        uniform->set( m_module->m_stepCount->get( true ) );
    }
}

