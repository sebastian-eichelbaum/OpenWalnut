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

#include "core/kernel/WKernel.h"
#include "core/dataHandler/WDataTexture3D.h"
#include "core/common/WColor.h"
#include "core/common/WBoundingBox.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGEGeodeUtils.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/callbacks/WGEShaderAnimationCallback.h"

#include "WMSurfaceParameterAnimator.h"
#include "WMSurfaceParameterAnimator.xpm"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMSurfaceParameterAnimator )

WMSurfaceParameterAnimator::WMSurfaceParameterAnimator():
    WModule(),
    m_rootNode( new WGEGroupNode() )
{
    // Initialize members
}

WMSurfaceParameterAnimator::~WMSurfaceParameterAnimator()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMSurfaceParameterAnimator::factory() const
{
    return boost::shared_ptr< WModule >( new WMSurfaceParameterAnimator() );
}

const char** WMSurfaceParameterAnimator::getXPMIcon() const
{
    return WMSurfaceParameterAnimator_xpm;
}

const std::string WMSurfaceParameterAnimator::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Surface Parameter Animator";
}

const std::string WMSurfaceParameterAnimator::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module can show parameters defined on surfaces using various types of animation.";
}

void WMSurfaceParameterAnimator::connectors()
{
    // needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "in", "The scalar dataset shown whose surface gets used for animation." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // and the fiber directions inside the volume
    m_tracesInput = boost::shared_ptr< WModuleInputData < WDataSetSingle  > >(
        new WModuleInputData< WDataSetSingle >( shared_from_this(), "traces", "The voxelized fiber traces for each voxel in the input \"in\"." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_tracesInput );

    // call WModules initialization
    WModule::connectors();
}

void WMSurfaceParameterAnimator::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoValue      = m_properties->addProperty( "Isovalue",         "The Isovalue used whenever the Isosurface Mode is turned on.",
                                                                      50 );
    m_stepCount     = m_properties->addProperty( "Step count",       "The number of steps to walk along the ray during raycasting. A low value"
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_alpha         = m_properties->addProperty( "Opacity %",        "The opacity in %. Transparency = 100 - Opacity.", 100 );

    m_isoColor      = m_properties->addProperty( "Iso color",        "The color to blend the isosurface with.", WColor( 0.0, 0.0, 0.0, 1.0 ),
                      m_propCondition );
    m_saturation    = m_properties->addProperty( "Saturation %",     "The saturation in %.", 100 );
    m_size1         = m_properties->addProperty( "Beam1 size",       "The relative size of the first beam. A value of 0 gets mapped to the "
                                                                      "smallest size, whilst 100 gets mapped to the largest. This is typically "
                                                                      "one third of the size of the voxelized surface.", 10 );
    m_size2         = m_properties->addProperty( "Beam2 size",       "The relative size of the second beam. A value of 0 gets mapped to the "
                                                                      "smallest size, whilst 100 gets mapped to the largest. This is typically "
                                                                      "one third of the size of the voxelized surface.", 50 );
    m_speed1         = m_properties->addProperty( "Beam1 speed",     "The relative speed of the beam. This speed relates to the clock used.", 25 );
    m_speed2         = m_properties->addProperty( "Beam2 speed",     "The relative speed of the beam. This speed relates to the clock used.", 25 );
    m_parameterScale = m_properties->addProperty( "Parameter scale", "Scaling the parameter space on the fly creates consistently sized and fast "
                                                                      "beams over multiple WMSurfaceParameterAnimator instances.", 1.0 );

    WModule::properties();
}

osg::ref_ptr< osg::Node > WMSurfaceParameterAnimator::renderSurface( const WBoundingBox& bbox )
{
    // use the OSG Shapes, create unit cube
    osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bbox, m_isoColor->get( true ) );
    cube->addUpdateCallback( new SafeUpdateCallback( this ) );
    cube->asTransform()->getChild( 0 )->setName( "DVR Proxy Cube" ); // Be aware that this name is used in the pick handler.
    m_shader->apply( cube );

    // bind the texture to the node
    osg::StateSet* rootState = cube->getOrCreateStateSet();

    osg::ref_ptr< WGETexture3D > texture3D = m_dataSet->getTexture();
    osg::ref_ptr< WGETexture3D > tracesTexture3D = m_tracesDataSet->getTexture();
    texture3D->bind( cube, 0 );
    tracesTexture3D->bind( cube, 1 );

    // enable transparency
    rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // setup all those uniforms
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    osg::ref_ptr< osg::Uniform > isovalue = new osg::Uniform( "u_isovalue", static_cast< float >( m_isoValue->get() / 100.0 ) );
    isovalue->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > steps = new osg::Uniform( "u_steps", m_stepCount->get() );
    steps->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > alpha = new osg::Uniform( "u_alpha", static_cast< float >( m_alpha->get() / 100.0 ) );
    alpha->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > animationTime = new osg::Uniform( "u_animationTime", 0 );
    animationTime->setUpdateCallback( new WGEShaderAnimationCallback() );

    osg::ref_ptr< osg::Uniform > size1 = new osg::Uniform( "u_size1", 0 );
    osg::ref_ptr< osg::Uniform > size2 = new osg::Uniform( "u_size2", 0 );
    size1->setUpdateCallback( new SafeUniformCallback( this ) );
    size2->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > speed1 = new osg::Uniform( "u_speed1", 0 );
    osg::ref_ptr< osg::Uniform > speed2 = new osg::Uniform( "u_speed2", 0 );
    speed1->setUpdateCallback( new SafeUniformCallback( this ) );
    speed2->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > paramScale = new osg::Uniform( "u_parameterScale", 1.0f );
    paramScale->setUpdateCallback( new SafeUniformCallback( this ) );

    osg::ref_ptr< osg::Uniform > saturation = new osg::Uniform( "u_saturation", 1.0f );
    saturation->setUpdateCallback( new SafeUniformCallback( this ) );

    rootState->addUniform( isovalue );
    rootState->addUniform( steps );
    rootState->addUniform( alpha );
    rootState->addUniform( animationTime );
    rootState->addUniform( size1 );
    rootState->addUniform( size2 );
    rootState->addUniform( speed1 );
    rootState->addUniform( speed2 );
    rootState->addUniform( paramScale );
    rootState->addUniform( saturation );

    return cube;
}

void WMSurfaceParameterAnimator::moduleMain()
{
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMSurfaceParameterAnimator-Beams", m_localPath ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_tracesInput->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();

    // add this module's group node
    m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );

    // Now wait for data
    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        // woke up since the module is requested to finish
        if( m_shutdownFlag() )
        {
            break;
        }

        // has the data changed?
        boost::shared_ptr< WDataSetSingle > newDataSet = m_input->getData();
        boost::shared_ptr< WDataSetSingle > newTracesDataSet = m_tracesInput->getData();

        bool dataChanged = ( m_dataSet != newDataSet ) || ( m_tracesDataSet != newTracesDataSet );
        bool dataValid =   ( newDataSet && newTracesDataSet );

        // valid data available?
        if( !dataValid )
        {
            debugLog() << "Resetting.";

            // reset internal refs to data
            m_dataSet.reset();
            m_tracesDataSet.reset();

            // remove renderings
            m_rootNode->clear();
        }

        // As the data has changed, we need to recreate the texture.
        if( dataChanged && dataValid )
        {
            m_dataSet = newDataSet;
            m_tracesDataSet = newTracesDataSet;

            debugLog() << "Data changed. Uploading new data as texture.";

            // First, grab the grid
            boost::shared_ptr< WGridRegular3D > grid = boost::shared_dynamic_cast< WGridRegular3D >( m_dataSet->getGrid() );
            if( !grid )
            {
                errorLog() << "The dataset does not provide a regular grid. Ignoring dataset.";
                continue;
            }

            // attach the geometry to the first FBO
            osg::ref_ptr< osg::Node > cube = renderSurface( grid->getBoundingBox() );

            // **********************************************************************************************
            // Update scene
            // **********************************************************************************************

            // update node
            debugLog() << "Adding new rendering.";
            m_rootNode->clear();
            m_rootNode->insert( cube );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMSurfaceParameterAnimator::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // update material info
    if( m_module->m_isoColor->changed() || m_initialUpdate )
    {
        // Set the diffuse color and material:
        osg::ref_ptr< osg::Material > mat = new osg::Material();
        mat->setDiffuse( osg::Material::FRONT, m_module->m_isoColor->get( true ) );
        node->getOrCreateStateSet()->setAttribute( mat, osg::StateAttribute::ON );

        m_initialUpdate = false;
    }

    traverse( node, nv );
}

void WMSurfaceParameterAnimator::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
{
    // update some uniforms:
    if( m_module->m_isoValue->changed()  && ( uniform->getName() == "u_isovalue" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue->get( true ) ) / 100.0f );
    }
    if( m_module->m_stepCount->changed() && ( uniform->getName() == "u_steps" ) )
    {
        uniform->set( m_module->m_stepCount->get( true ) );
    }
    if( m_module->m_alpha->changed() && ( uniform->getName() == "u_alpha" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha->get( true ) / 100.0 ) );
    }
    if( m_module->m_alpha->changed() && ( uniform->getName() == "u_size1" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha->get( true ) / 100.0 ) );
    }
    if( m_module->m_size1->changed() && ( uniform->getName() == "u_size1" ) )
    {
        uniform->set( m_module->m_size1->get( true ) );
    }
    if( m_module->m_size2->changed() && ( uniform->getName() == "u_size2" ) )
    {
        uniform->set( m_module->m_size2->get( true ) );
    }
    if( m_module->m_speed1->changed() && ( uniform->getName() == "u_speed1" ) )
    {
        uniform->set( m_module->m_speed1->get( true ) );
    }
    if( m_module->m_speed2->changed() && ( uniform->getName() == "u_speed2" ) )
    {
        uniform->set( m_module->m_speed2->get( true ) );
    }
    if( m_module->m_parameterScale->changed() && ( uniform->getName() == "u_parameterScale" ) )
    {
        uniform->set( static_cast< float >( m_module->m_parameterScale->get( true ) ) );
    }
    if( m_module->m_saturation->changed() && ( uniform->getName() == "u_saturation" ) )
    {
        uniform->set( static_cast< float >( m_module->m_saturation->get( true ) ) / 100.0f );
    }
}

void WMSurfaceParameterAnimator::activate()
{
    // Activate/Deactivate the rendering
    if( m_rootNode )
    {
        if( m_active->get() )
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

