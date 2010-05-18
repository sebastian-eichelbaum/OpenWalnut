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

#include "../../common/WColor.h"
#include "../../dataHandler/WDataSetScalar.h"
#include "../../dataHandler/WDataTexture3D.h"
#include "../../graphicsEngine/WGEGeodeUtils.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WShader.h"
#include "../../kernel/WKernel.h"
#include "WMProbTractDisplay.h"
#include "directvolumerendering.xpm"

WMProbTractDisplay::WMProbTractDisplay():
    WModule(),
    m_rootNode( new osg::Node() )
{
    // Initialize members
}

WMProbTractDisplay::~WMProbTractDisplay()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMProbTractDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMProbTractDisplay() );
}

const char** WMProbTractDisplay::getXPMIcon() const
{
    return directvolumerendering_xpm;
}

const std::string WMProbTractDisplay::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Probabilistic Tract Rendering";
}

const std::string WMProbTractDisplay::getDescription() const
{
    // Specify your module description here. Be detailed. This text is read by the user.
    return "This module shows a direct volume rendering of the specified scalar dataset.";
}

void WMProbTractDisplay::connectors()
{
    // PTD needs one input: the scalar dataset
    m_input = boost::shared_ptr< WModuleInputData < WDataSetScalar  > >(
        new WModuleInputData< WDataSetScalar >( shared_from_this(), "in", "The scalar dataset shown using PTD." )
    );

    // As properties, every connector needs to be added to the list of connectors.
    addConnector( m_input );

    // call WModules initialization
    WModule::connectors();
}

void WMProbTractDisplay::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_isoSurface    = m_properties->addProperty( "Isosurface Mode",  "If enabled, the Volume Renderer will render an isosurface and ignores the "
                                                                      "transfer function.", true );
    m_isoValue0     = m_properties->addProperty( "Isovalue 0",       "The isovalue used whenever the isosurface Mode is turned on.", 50 );
    m_isoValue1     = m_properties->addProperty( "Isovalue 1",       "The isovalue used whenever the isosurface Mode is turned on.", 50 );
    m_isoValue2     = m_properties->addProperty( "Isovalue 2",       "The isovalue used whenever the isosurface Mode is turned on.", 50 );
    m_isoValue3     = m_properties->addProperty( "Isovalue 3",       "The isovalue used whenever the isosurface Mode is turned on.", 50 );

    m_isoColor0     = m_properties->addProperty( "Iso Color 0",   "The color to blend the 0'th isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );
    m_isoColor1     = m_properties->addProperty( "Iso Color 1",   "The color to blend the 1'th isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );
    m_isoColor2     = m_properties->addProperty( "Iso Color 2",   "The color to blend the 2'th isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );
    m_isoColor3     = m_properties->addProperty( "Iso Color 3",   "The color to blend the 3'th isosurface with.", WColor( 1.0, 1.0, 1.0, 1.0 ),
                      m_propCondition );

    m_stepCount     = m_properties->addProperty( "Step Count",       "The number of steps to walk along the ray during raycasting. A low value "
                                                                      "may cause artifacts whilst a high value slows down rendering.", 250 );
    m_stepCount->setMin( 1 );
    m_stepCount->setMax( 1000 );

    m_alpha0         = m_properties->addProperty( "Opacity % 0",  "The opacity in %. Transparency = 100 - Opacity for the 0'th isosurface", 100 );
    m_alpha1         = m_properties->addProperty( "Opacity % 1",  "The opacity in %. Transparency = 100 - Opacity for the 1'th isosurface", 100 );
    m_alpha2         = m_properties->addProperty( "Opacity % 2",  "The opacity in %. Transparency = 100 - Opacity for the 2'th isosurface", 100 );
    m_alpha3         = m_properties->addProperty( "Opacity % 3",  "The opacity in %. Transparency = 100 - Opacity for the 3'th isosurface", 100 );

    m_useSimpleDepthColoring = m_properties->addProperty( "Use Depth Cueing", "Enable it to have simple depth dependent coloring only.", false );
}

void WMProbTractDisplay::moduleMain()
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "PTDRaycast" ) );

    // let the main loop awake if the data changes or the properties changed.
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    // Signal ready state.
    ready();
    debugLog() << "Module is now ready.";

    // Normally, you will have a loop which runs as long as the module should not shutdown. In this loop you can react on changing data on input
    // connectors or on changed in your properties.
    debugLog() << "Entering main loop";
    while ( !m_shutdownFlag() )
    {
        // Now, the moduleState variable comes into play. The module can wait for the condition, which gets fired whenever the input receives data
        // or an property changes. The main loop now waits until something happens.
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        // has the data changed?
        boost::shared_ptr< WDataSetScalar > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        if ( dataChanged || !m_dataSet )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // invalid data
            if ( !m_dataSet )
            {
                debugLog() << "Invalid Data. Disabling.";
                continue;
            }
        }

        // m_isoColor changed
        if( m_isoColor0->changed() || m_isoColor1->changed() || m_isoColor2->changed() || m_isoColor3->changed() )
        {
            // a new color requires the proxy geometry to be rebuild as we store it as color in this geometry
            dataChanged = true;
        }

        // As the data has changed, we need to recreate the texture.
        if ( dataChanged )
        {
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

            // use the OSG Shapes, create unit cube
            osg::ref_ptr< osg::Node > cube = wge::generateSolidBoundingBoxNode( bb.first, bb.second, m_isoColor3->get( true ) );
            cube->asTransform()->getChild( 0 )->setName( "PTD Proxy Cube" ); // Be aware that this name is used in the pick handler.
            m_shader->apply( cube );

            // bind the texture to the node
            osg::ref_ptr< osg::Texture3D > texture3D = m_dataSet->getTexture()->getTexture();
            osg::StateSet* rootState = cube->getOrCreateStateSet();
            rootState->setTextureAttributeAndModes( 0, texture3D, osg::StateAttribute::ON );

            // enable transparency
            rootState->setMode( GL_BLEND, osg::StateAttribute::ON );

            ////////////////////////////////////////////////////////////////////////////////////////////////////
            // setup all those uniforms
            ////////////////////////////////////////////////////////////////////////////////////////////////////

            // for the texture, also bind the appropriate uniforms
            rootState->addUniform( new osg::Uniform( "tex0", 0 ) );

            osg::ref_ptr< osg::Uniform > isovalue0 = new osg::Uniform( "u_isovalue0", static_cast< float >( m_isoValue0->get() / 100.0 ) );
            isovalue0->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isovalue1 = new osg::Uniform( "u_isovalue1", static_cast< float >( m_isoValue1->get() / 100.0 ) );
            isovalue1->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isovalue2 = new osg::Uniform( "u_isovalue2", static_cast< float >( m_isoValue2->get() / 100.0 ) );
            isovalue2->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isovalue3 = new osg::Uniform( "u_isovalue3", static_cast< float >( m_isoValue3->get() / 100.0 ) );
            isovalue3->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isosurface = new osg::Uniform( "u_isosurface", m_isoSurface->get() );
            isosurface->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > steps = new osg::Uniform( "u_steps", m_stepCount->get() );
            steps->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > alpha0 = new osg::Uniform( "u_alpha0", static_cast< float >( m_alpha0->get() / 100.0 ) );
            alpha0->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > alpha1 = new osg::Uniform( "u_alpha1", static_cast< float >( m_alpha1->get() / 100.0 ) );
            alpha1->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > alpha2 = new osg::Uniform( "u_alpha2", static_cast< float >( m_alpha2->get() / 100.0 ) );
            alpha2->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > alpha3 = new osg::Uniform( "u_alpha3", static_cast< float >( m_alpha3->get() / 100.0 ) );
            alpha3->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > depthCueingOnly = new osg::Uniform( "u_depthCueingOnly", m_useSimpleDepthColoring->get() );
            depthCueingOnly->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isocolor0 = new osg::Uniform( "u_isocolor0", wge::osgColor( m_isoColor0->get() ) );
            isocolor0->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isocolor1 = new osg::Uniform( "u_isocolor1", wge::osgColor( m_isoColor1->get() ) );
            isocolor1->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isocolor2 = new osg::Uniform( "u_isocolor2", wge::osgColor( m_isoColor2->get() ) );
            isocolor2->setUpdateCallback( new SafeUniformCallback( this ) );

            osg::ref_ptr< osg::Uniform > isocolor3 = new osg::Uniform( "u_isocolor3", wge::osgColor( m_isoColor3->get() ) );
            isocolor3->setUpdateCallback( new SafeUniformCallback( this ) );

            rootState->addUniform( isovalue0 );
            rootState->addUniform( isovalue1 );
            rootState->addUniform( isovalue2 );
            rootState->addUniform( isovalue3 );
            rootState->addUniform( isosurface );
            rootState->addUniform( steps );
            rootState->addUniform( alpha0 );
            rootState->addUniform( alpha1 );
            rootState->addUniform( alpha2 );
            rootState->addUniform( alpha3 );
            rootState->addUniform( isocolor0 );
            rootState->addUniform( isocolor1 );
            rootState->addUniform( isocolor2 );
            rootState->addUniform( isocolor3 );
            rootState->addUniform( depthCueingOnly );

            // update node
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
            m_rootNode = cube;
            m_rootNode->setNodeMask( m_active->get() ? 0xFFFFFFFF : 0x0 );
            debugLog() << "Adding new rendering.";
            WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
        }
    }

    // At this point, the container managing this module signalled to shutdown. The main loop has ended and you should clean up. Always remove
    // allocated memory and remove all OSG nodes.
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMProbTractDisplay::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // currently, there is nothing to update
    traverse( node, nv );
}

void WMProbTractDisplay::SafeUniformCallback::operator()( osg::Uniform* uniform, osg::NodeVisitor* /* nv */ )
{
    // update some uniforms:
    if ( m_module->m_isoValue0->changed() && ( uniform->getName() == "u_isovalue0" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue0->get( true ) / 100.0 ) );
    }
    if ( m_module->m_isoValue1->changed() && ( uniform->getName() == "u_isovalue1" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue1->get( true ) / 100.0 ) );
    }
    if ( m_module->m_isoValue2->changed() && ( uniform->getName() == "u_isovalue2" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue2->get( true ) / 100.0 ) );
    }
    if ( m_module->m_isoValue3->changed() && ( uniform->getName() == "u_isovalue3" ) )
    {
        uniform->set( static_cast< float >( m_module->m_isoValue3->get( true ) / 100.0 ) );
    }
    if ( m_module->m_isoSurface->changed() && ( uniform->getName() == "u_isosurface" ) )
    {
        uniform->set( m_module->m_isoSurface->get( true ) );
    }
    if ( m_module->m_stepCount->changed() && ( uniform->getName() == "u_steps" ) )
    {
        uniform->set( m_module->m_stepCount->get( true ) );
    }
    if ( m_module->m_alpha0->changed() && ( uniform->getName() == "u_alpha0" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha0->get( true ) / 100.0 ) );
    }
    if ( m_module->m_alpha1->changed() && ( uniform->getName() == "u_alpha1" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha1->get( true ) / 100.0 ) );
    }
    if ( m_module->m_alpha2->changed() && ( uniform->getName() == "u_alpha2" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha2->get( true ) / 100.0 ) );
    }
    if ( m_module->m_alpha3->changed() && ( uniform->getName() == "u_alpha3" ) )
    {
        uniform->set( static_cast< float >( m_module->m_alpha3->get( true ) / 100.0 ) );
    }
    if( m_module->m_isoColor0->changed() && ( uniform->getName() == "u_isocolor0" ) )
    {
        uniform->set( wge::osgColor( m_module->m_isoColor0->get( true ) ) );
    }
    if( m_module->m_isoColor1->changed() && ( uniform->getName() == "u_isocolor1" ) )
    {
        uniform->set( wge::osgColor( m_module->m_isoColor1->get( true ) ) );
    }
    if( m_module->m_isoColor2->changed() && ( uniform->getName() == "u_isocolor2" ) )
    {
        uniform->set( wge::osgColor( m_module->m_isoColor2->get( true ) ) );
    }
    if( m_module->m_isoColor3->changed() && ( uniform->getName() == "u_isocolor3" ) )
    {
        uniform->set( wge::osgColor( m_module->m_isoColor3->get( true ) ) );
    }
    if ( m_module->m_useSimpleDepthColoring->changed() && ( uniform->getName() == "u_depthCueingOnly" ) )
    {
        uniform->set( m_module->m_useSimpleDepthColoring->get( true ) );
    }
}

void WMProbTractDisplay::activate()
{
    // Activate/Deactivate the PTD
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

