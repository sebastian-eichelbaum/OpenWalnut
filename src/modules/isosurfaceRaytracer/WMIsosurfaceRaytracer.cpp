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

#include "../../common/WColor.h"
#include "../../common/WPropertyHelper.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEColormapping.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEManagedGroupNode.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/shaders/WGEShader.h"
#include "../../graphicsEngine/shaders/WGEShaderDefineOptions.h"
#include "../../graphicsEngine/shaders/WGEShaderPropertyDefineOptions.h"
#include "../../graphicsEngine/postprocessing/WGEPostprocessingNode.h"
#include "../../graphicsEngine/WGERequirement.h"
#include "../../graphicsEngine/callbacks/WGENodeMaskCallback.h"
#include "../../kernel/WKernel.h"
#include "WMIsosurfaceRaytracer.xpm"
#include "WMIsosurfaceRaytracer.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMIsosurfaceRaytracer )

WMIsosurfaceRaytracer::WMIsosurfaceRaytracer():
    WModule()
{
    // Initialize members
}

WMIsosurfaceRaytracer::~WMIsosurfaceRaytracer()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMIsosurfaceRaytracer::factory() const
{
    return boost::shared_ptr< WModule >( new WMIsosurfaceRaytracer() );
}

const char** WMIsosurfaceRaytracer::getXPMIcon() const
{
    return isosurfaceraytracer_xpm;
}

const std::string WMIsosurfaceRaytracer::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Isosurface Raytracer";
}

const std::string WMIsosurfaceRaytracer::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module shows a fast raytraced isosurface of the specified scalar dataset.";
}

void WMIsosurfaceRaytracer::connectors()
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

void WMIsosurfaceRaytracer::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoValue      = m_properties->addProperty( "Isovalue",         "The isovalue used whenever the isosurface Mode is turned on.",
                                                                      50 );
    m_isoColor      = m_properties->addProperty( "Iso color",        "The color to blend the isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );

    m_stepCount     = m_properties->addProperty( "Step count",       "The number of steps to walk along the ray during raycasting. A low value "
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_alpha         = m_properties->addProperty( "Opacity %",        "The opacity in %. Transparency = 100 - Opacity.", 100 );

    m_cortexMode    = m_properties->addProperty( "Emphasize Cortex", "Emphasize the Cortex while inner parts ar not that well lighten.", false );

    WModule::properties();
}

void WMIsosurfaceRaytracer::requirements()
{
    m_requirements.push_back( new WGERequirement() );
}

void WMIsosurfaceRaytracer::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMIsosurfaceRaytracer", m_localPath ) );
    WGEShaderPreprocessor::SPtr cortexMode( new WGEShaderPropertyDefineOptions< WPropBool >( m_cortexMode, "CORTEXMODE_DISENABLED", "CORTEXMODE_ENABLED" ) );
    m_shader->addPreprocessor( cortexMode );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    // create the root node containing the transformation and geometry
    osg::ref_ptr< WGEManagedGroupNode > rootNode = new WGEManagedGroupNode( m_active );

    // create the post-processing node which actually does the nice stuff to the rendered image
    osg::ref_ptr< WGEPostprocessingNode > postNode = new WGEPostprocessingNode(
        WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera()
    );

    // provide the properties of the post-processor to the user
    m_properties->addProperty( postNode->getProperties() );

    // insert it
    postNode->insert( rootNode, m_shader );
    bool postNodeInserted = false;

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

        // valid data available?
        if ( !dataValid )
        {
            // remove renderings if no data is available anymore
            rootNode->clear();
        }

        // m_isoColor or shading changed
        if ( m_isoColor->changed() )
        {
            // a new color requires the proxy geometry to be rebuild as we store it as color in this geometry
            dataUpdated = true;
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataUpdated && dataValid )
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
            WBoundingBox bb( wmath::WPosition( 0.0, 0.0, 0.0 ),
                wmath::WPosition( grid->getNbCoordsX() - 1, grid->getNbCoordsY() - 1, grid->getNbCoordsZ() - 1 ) );
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bb, m_isoColor->get( true ) );
            cube->asTransform()->getChild( 0 )->setName( "_DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
                                                                              // because of the underscore in front it won't be picked
            // we also set the grid's transformation here
            rootNode->setMatrix( wge::toOSGMatrix( grid->getTransformationMatrix() ) );

            m_shader->apply( cube );

            // bind the texture to the node
            osg::ref_ptr< osg::Texture3D > texture3D = dataSet->getTexture()->getTexture();
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // for the texture, also bind the appropriate uniforms
            rootState->addUniform( new osg::Uniform( "tex0", 0 ) );

            osg::ref_ptr< osg::Uniform > isovalue = new osg::Uniform( "u_isovalue", static_cast< float >( m_isoValue->get() / 100.0 ) );
            isovalue->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > steps = new osg::Uniform( "u_steps", m_stepCount->get() );
            steps->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > alpha = new osg::Uniform( "u_alpha", static_cast< float >( m_alpha->get() / 100.0 ) );
            alpha->setUpdateCallback( new SafeUniformCallback( this ) );

            rootState->addUniform( isovalue );
            rootState->addUniform( steps );
            rootState->addUniform( alpha );

            // Stochastic jitter?
            const size_t size = 64;
            osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size );
            wge::bindTexture( cube, randTex, 1 );

            //WGEColormapping::apply( cube, false );

            // update node
            debugLog() << "Adding new rendering.";
            rootNode->clear();
            rootNode->insert( cube );
            // insert root node if needed. This way, we ensure that the root node gets added only if the proxy cube has been added AND the bbox
            // can be calculated properly by the OSG to ensure the proxy cube is centered in the scene if no other item has been added earlier.
            if ( !postNodeInserted )
            {
                postNodeInserted = true;
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( postNode );
            }
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( postNode );
}

void WMIsosurfaceRaytracer::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // currently, there is nothing to update
    traverse( node, nv );
}

void WMIsosurfaceRaytracer::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
{
    // update some uniforms:
    if ( m_module->m_isoValue->changed() && ( uniform->getName() == "u_isovalue" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue->get( true ) ) / 100.0f );
    }
    if ( m_module->m_stepCount->changed() && ( uniform->getName() == "u_steps" ) )
    {
        uniform->set( m_module->m_stepCount->get( true ) );
    }
    if ( m_module->m_alpha->changed() && ( uniform->getName() == "u_alpha" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha->get( true ) / 100.0 ) );
    }
}

