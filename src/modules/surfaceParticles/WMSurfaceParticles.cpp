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

#include <osg/ShapeDrawable>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Material>
#include <osg/StateAttribute>
#include <osg/MatrixTransform>
#include <osg/Projection>

#include "../../kernel/WKernel.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../common/WColor.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../graphicsEngine/WGEOffscreen.h"
#include "../../graphicsEngine/WGETextureHud.h"

#include "WMSurfaceParticles.h"
#include "surfaceParticles.xpm"

WMSurfaceParticles::WMSurfaceParticles():
    WModule(),
    m_rootNode( new WGEGroupNode() )
{
    // Initialize members
}

WMSurfaceParticles::~WMSurfaceParticles()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMSurfaceParticles::factory() const
{
    return boost::shared_ptr< WModule >( new WMSurfaceParticles() );
}

const char** WMSurfaceParticles::getXPMIcon() const
{
    return surfaceParticles_xpm;
}

const std::string WMSurfaceParticles::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Surface Particles";
}

const std::string WMSurfaceParticles::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module can show particles on a GPU based Isosurface.";
}

void WMSurfaceParticles::connectors()
{
    // needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The scalar dataset shown whose surface gets used for animation." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // and the fiber directions inside the volume
    m_directionInput = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "directions", "The voxelized fiber directions for each voxel in the input"
                                                                                  "\"in\"." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_directionInput );

    // and the fiber directions inside the volume
    m_tracesInput = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "traces", "The voxelized fiber traces for each voxel in the input \"in\"." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_tracesInput );

    // call WModules initialization
    WModule::connectors();
}

void WMSurfaceParticles::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoValue      = m_properties2->addProperty( "Isovalue",         "The Isovalue used whenever the Isosurface Mode is turned on.",
                                                                      50 );
    m_stepCount     = m_properties2->addProperty( "Step Count",       "The number of steps to walk along the ray during raycasting. A low value"
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_alpha         = m_properties2->addProperty( "Opacity %",        "The opacity in %. Transparency = 100 - Opacity.", 100 );

    m_gridResolution = m_properties2->addProperty( "Grid Resolution",
            "Determines the grid resolution for the particles in relation to the dataset grid.", 25.0
    );

    m_particleSize = m_properties2->addProperty( "Particle Size", "The size of a particle in relation to the voxel.", 1.0 );
    m_particleSize->setMin(   0 );
    m_particleSize->setMax( 100 );

    m_isoColor      = m_properties2->addProperty( "Iso Color",        "The color to blend the isosurface with.", WColor( 0.0, 0.0, 0.0, 1.0 ),
                      m_propCondition );

}

osg::ref_ptr< osg::Node > WMSurfaceParticles::renderSurface( std::pair< wmath::WPosition, wmath::WPosition > bbox )
{
    // use the OSG Shapes, create unit cube
    osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bbox.first, bbox.second, m_isoColor->get( true ) );
    cube->addUpdateCallback( new SafeUpdateCallback( this ) );
    cube->asTransform()->getChild( 0 )->setName( "DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
    m_shader->apply( cube );

    // bind the texture to the node
    osg::ref_ptr< osg::Texture3D > texture3D = m_dataSet->getTexture()->getTexture();
    osg::ref_ptr< osg::Texture3D > directionTexture3D = m_directionDataSet->getTexture()->getTexture();
    osg::ref_ptr< osg::Texture3D > tracesTexture3D = m_tracesDataSet->getTexture()->getTexture();
    osg::StateSet* rootState = cube->getOrCreateStateSet();
    rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );
    rootState->setTextureAttributeAndModes( 1, directionTexture3D, osg::StateAttribute::ON );
    rootState->setTextureAttributeAndModes( 2, tracesTexture3D, osg::StateAttribute::ON );

    //tracesTexture3D->setFilter( osg::Texture::MIN_FILTER, osg::Texture::NEAREST );
    //tracesTexture3D->setFilter( osg::Texture::MAG_FILTER, osg::Texture::NEAREST );

    // enable transparency
    rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup all those uniforms
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    // for the texture, also bind the appropriate uniforms
    rootState->addUniform( new osg::Uniform( "tex0", 0 ) );
    rootState->addUniform( new osg::Uniform( "tex1", 1 ) );
    rootState->addUniform( new osg::Uniform( "tex2", 2 ) );

    // we need to specify the texture scaling parameters to the shader
    rootState->addUniform( new osg::Uniform( "u_tex1Scale", m_directionDataSet->getTexture()->getMinMaxScale() ) );
    rootState->addUniform( new osg::Uniform( "u_tex1Min", m_directionDataSet->getTexture()->getMinValue() ) );
    rootState->addUniform( new osg::Uniform( "u_tex1Max", m_directionDataSet->getTexture()->getMaxValue() ) );

    // we need to specify the texture scaling parameters to the shader
    rootState->addUniform( new osg::Uniform( "u_tex2Scale", m_tracesDataSet->getTexture()->getMinMaxScale() ) );
    rootState->addUniform( new osg::Uniform( "u_tex2Min", m_tracesDataSet->getTexture()->getMinValue() ) );
    rootState->addUniform( new osg::Uniform( "u_tex2Max", m_tracesDataSet->getTexture()->getMaxValue() ) );

    osg::ref_ptr< osg::Uniform > isovalue = new osg::Uniform( "u_isovalue", static_cast< float >( m_isoValue->get() / 100.0 ) );
    isovalue->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > steps = new osg::Uniform( "u_steps", m_stepCount->get() );
    steps->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > alpha = new osg::Uniform( "u_alpha", static_cast< float >( m_alpha->get() / 100.0 ) );
    alpha->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > gridResolution = new osg::Uniform( "u_gridResolution", static_cast< float >( m_gridResolution->get() ) );
    gridResolution->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > particleSize = new osg::Uniform( "u_particleSize", static_cast< float >( m_particleSize->get() ) );
    particleSize->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > animationTime = new osg::Uniform( "u_animationTime", 0 );
    animationTime->setUpdateCallback( new ShaderAnimationCallback() );

    rootState->addUniform( isovalue );
    rootState->addUniform( steps );
    rootState->addUniform( alpha );
    rootState->addUniform( gridResolution );
    rootState->addUniform( particleSize );
    rootState->addUniform( animationTime );

    return cube;
}

void WMSurfaceParticles::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "GPUSurfaceParticles" ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_directionInput->getDataChangedCondition() );
    m_moduleState.add( m_tracesInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();

    // add this module's group node
    m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Now wait for data
    while ( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if ( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        boost::shared_ptr< WDataSetSingle > newDirectionDataSet = m_directionInput->getData();
        boost::shared_ptr< WDataSetSingle > newTracesDataSet = m_tracesInput->getData();

        bool dataChanged = ( m_dataSet != newDataSet ) || ( m_directionDataSet != newDirectionDataSet ) || ( m_tracesDataSet != newTracesDataSet );
        bool dataValid =   ( newDataSet && newDirectionDataSet && newTracesDataSet );

        // As the data has changed, we need to recreate the texture.
        if ( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
            m_directionDataSet = newDirectionDataSet;
            m_tracesDataSet = newTracesDataSet;

            debugLog() << "Data changed. Uploading new data as texture.";

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            if ( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // get the BBox
            std::pair< wmath::WPosition, wmath::WPosition > bb = grid->getBoundingBox();

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // Prepare FBO
            //////////////////////////////////////////////////////////////////////////////////////////////////

            osg::ref_ptr<osg::Camera> sceneCamera = WKernel::getRunningKernel()->getGraphicsEngine()->getViewer()->getCamera();

            // setup the FBO
            osg::ref_ptr< WGEOffscreen > offscreen1 = new WGEOffscreen( sceneCamera, 0 );
            offscreen1->setClearColor( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );

            // For debugging:
            osg::ref_ptr< WGETextureHud > hud = new WGETextureHud();
            // increase texture size a little bit
            hud->setMaxElementWidth( 512 );

            // **********************************************************************************************
            // Render Pass 1: Rendering the geometry and projecting the directions
            // **********************************************************************************************

            // The surface
            //osg::ref_ptr< osg::Texture2D > surfaceTex = offscreen1->attach( osg::Camera::COLOR_BUFFER0 );
            //osg::ref_ptr< osg::Texture2D > dirTex = offscreen1->attach( osg::Camera::COLOR_BUFFER1 );

            //hud->addTexture( new WGETextureHud::WGETextureHudEntry( surfaceTex ) );
            //hud->addTexture( new WGETextureHud::WGETextureHudEntry( dirTex ) );

            // attach the geometry to the first FBO
            osg::ref_ptr< osg::Node > cube = renderSurface( bb );
            //offscreen1->addChild( cube );

            // **********************************************************************************************
            // Update scene
            // **********************************************************************************************

            // update node
            debugLog() << "Adding new rendering.";
            m_rootNode->clear();
    //        m_rootNode->insert( offscreen1 );
    //        m_rootNode->insert( hud );
            m_rootNode->insert( cube );

        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMSurfaceParticles::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // update material info
    if ( m_module->m_isoColor->changed() || m_initialUpdate )
    {
        // Grab the color
        WColor c = m_module->m_isoColor->get( true );

        // Set the diffuse color and material:
        osg::ref_ptr< osg::Material > mat = new osg::Material();
        mat->setDiffuse( osg::Material::FRONT, osg::Vec4( c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha() ) );
        node->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );

        m_initialUpdate = false;
    }

    traverse( node, nv );
}

void WMSurfaceParticles::ShaderAnimationCallback::operator() ( osg::Uniform* uniform, osg::NodeVisitor* /*nv*/ )
{
    m_counter++;
    uniform->set( m_counter );
}

void WMSurfaceParticles::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
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
    if ( m_module->m_gridResolution->changed() && ( uniform->getName() == "u_gridResolution" ) )
    {
        uniform->set( static_cast< float >( m_module->m_gridResolution->get( true ) ) );
    }
    if ( m_module->m_particleSize->changed() && ( uniform->getName() == "u_particleSize" ) )
    {
        uniform->set( static_cast< float >( m_module->m_particleSize->get( true ) ) );
    }
}

void WMSurfaceParticles::activate()
{
    // Activate/Deactivate the rendering
    if ( m_rootNode )
    {
        if ( m_active->get() )
        {
            m_rootNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_rootNode->setNodeMask( 0x0 );
        }
    }

    // Always call WModule's activate!
    WModule::activate();
}

