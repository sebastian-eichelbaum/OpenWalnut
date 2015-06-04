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
#include <vector>

#include <osg/Geometry>

#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WDataSet.h"
#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WSubject.h"
#include "core/dataHandler/WValueSet.h"
#include "core/graphicsEngine/shaders/WGEShader.h"
#include "core/graphicsEngine/WGEUtils.h"
#include "core/graphicsEngine/WGEColormapping.h"
#include "core/graphicsEngine/shaders/WGEPropertyUniform.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WSelectionManager.h"

#include "WMArbitraryPlane.h"
#include "WMArbitraryPlane.xpm"

// This line is needed by the module loader to actually find your module. Do not remove. Do NOT add a ";" here.
W_LOADABLE_MODULE( WMArbitraryPlane )

WMArbitraryPlane::WMArbitraryPlane():
    WModule(),
    m_dirty( false )
{
    // initialize members
}

WMArbitraryPlane::~WMArbitraryPlane()
{
    // Cleanup!
}

boost::shared_ptr< WModule > WMArbitraryPlane::factory() const
{
    // See "src/modules/template/" for an extensively documented example.
    return boost::shared_ptr< WModule >( new WMArbitraryPlane() );
}

const char** WMArbitraryPlane::getXPMIcon() const
{
    return arbitraryPlane_xpm;
}

const std::string WMArbitraryPlane::getName() const
{
    // Specify your module name here. This name must be UNIQUE!
    return "Arbitrary Plane";
}

const std::string WMArbitraryPlane::getDescription() const
{
    return "Module draws an arbitrarily moveable plane through the data.";
}

void WMArbitraryPlane::connectors()
{
    // Put the code for your connectors here. See "src/modules/template/" for an extensively documented example.

    // call WModules initialization
    WModule::connectors();
}

void WMArbitraryPlane::properties()
{
    // Initialize the properties
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_showComplete = m_properties->addProperty( "Show complete", "Slice should be drawn complete even if the texture value is zero.",
            false, m_propCondition );
    m_showManipulators = m_properties->addProperty( "Show manipulators", "Hide/Show manipulators.", true, m_propCondition );

    m_attach2Crosshair = m_properties->addProperty( "Attach to crosshair", "Attach to Crosshair", true, m_propCondition );

    m_buttonReset2Axial = m_properties->addProperty( "Axial", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_buttonReset2Coronal = m_properties->addProperty( "Coronal", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );
    m_buttonReset2Sagittal = m_properties->addProperty( "Sagittal", "Resets and aligns the plane", WPVBaseTypes::PV_TRIGGER_READY, m_propCondition  );

    WModule::properties();
}

void WMArbitraryPlane::moduleMain()
{
    initPlane();

    updatePlane();

    ready();

    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_propCondition );
    m_moduleState.add( m_active->getUpdateCondition() );

    while( !m_shutdownFlag() )
    {
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        if( m_showComplete->changed() )
        {
            m_showComplete->get( true );
            m_dirty = true;
        }

        if( m_active->changed() )
        {
            if( m_active->get( true ) && m_showManipulators->get() )
            {
                m_s0->unhide();
                m_s1->unhide();
                m_s2->unhide();
            }
            else
            {
                m_s0->hide();
                m_s1->hide();
                m_s2->hide();
            }
        }

        if( m_showManipulators->changed() )
        {
            if( m_showManipulators->get( true ) )
            {
                if( m_active->get() )
                {
                    m_s0->unhide();
                    m_s1->unhide();
                    m_s2->unhide();
                }
            }
            else
            {
                m_s0->hide();
                m_s1->hide();
                m_s2->hide();
            }
        }

        if( m_buttonReset2Axial->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( WPosition( center[0] - 100, center[1], center[2] ) );
            m_s2->setPosition( WPosition( center[0], center[1] - 100, center[2] ) );
            m_buttonReset2Axial->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }

        if( m_buttonReset2Coronal->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( WPosition( center[0] - 100, center[1], center[2] ) );
            m_s2->setPosition( WPosition( center[0], center[1], center[2] - 100 ) );
            m_buttonReset2Coronal->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }

        if( m_buttonReset2Sagittal->get( true ) == WPVBaseTypes::PV_TRIGGER_TRIGGERED )
        {
            WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
            m_s0->setPosition( center );
            m_s1->setPosition( WPosition( center[0], center[1], center[2] - 100 ) );
            m_s2->setPosition( WPosition( center[0], center[1] - 100, center[2] ) );
            m_buttonReset2Sagittal->set( WPVBaseTypes::PV_TRIGGER_READY, false );
            m_dirty = true;
        }
    }
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( &( *m_s0 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( &( *m_s1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->remove( &( *m_s2 ) );

    m_s0->removeROIChangeNotifier( m_changeRoiSignal );
    m_s1->removeROIChangeNotifier( m_changeRoiSignal );
    m_s2->removeROIChangeNotifier( m_changeRoiSignal );

    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->remove( m_rootNode );
}

void WMArbitraryPlane::initPlane()
{
    m_rootNode = new WGEManagedGroupNode( m_active );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->insert( m_rootNode );
    m_geode = new osg::Geode();
    m_geode->setName( "_arbitraryPlane" );

    // create shader
    m_shader = osg::ref_ptr< WGEShader > ( new WGEShader( "WMArbitraryPlane", m_localPath ) );
    m_shader->apply( m_rootNode );

    // Apply colormapping
    WGEColormapping::apply( m_geode, m_shader );

    // bind some uniforms with properties
    m_geode->getOrCreateStateSet()->addUniform( new WGEPropertyUniform< WPropBool >( "u_showComplete", m_showComplete ) );

    // we need transparency
    m_geode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
    m_geode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

    m_rootNode->insert( m_geode );
    m_rootNode->addUpdateCallback( new WGEFunctorCallback< osg::Node >( boost::bind( &WMArbitraryPlane::updateCallback, this ) ) );

    WPosition center = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    m_p0 = WPosition( center );
    m_p1 = WPosition( center[0] - 100, center[1], center[2] );
    m_p2 = WPosition( center[0], center[1] - 100, center[2] );

    m_s0 = osg::ref_ptr<WROISphere>( new WROISphere( m_p0, 2.5 ) );
    m_s1 = osg::ref_ptr<WROISphere>( new WROISphere( m_p1, 2.5 ) );
    m_s1->setLockY();
    m_s2 = osg::ref_ptr<WROISphere>( new WROISphere( m_p2, 2.5 ) );
    m_s2->setLockX();

    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s0 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s1 ) );
    WGraphicsEngine::getGraphicsEngine()->getScene()->addChild( &( *m_s2 ) );

    m_changeRoiSignal
        = boost::shared_ptr< boost::function< void() > >( new boost::function< void() >( boost::bind( &WMArbitraryPlane::setDirty, this ) ) );
    m_s0->addROIChangeNotifier( m_changeRoiSignal );
    m_s1->addROIChangeNotifier( m_changeRoiSignal );
    m_s2->addROIChangeNotifier( m_changeRoiSignal );
}

void WMArbitraryPlane::updatePlane()
{
    m_geode->removeDrawables( 0, 1 );

    if( m_attach2Crosshair->get() )
    {
        m_s0->setPosition( WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition() );
    }

    WPosition p0 = m_s0->getPosition();

    if( p0 != m_p0 )
    {
        WVector3d offset = p0 - m_p0;
        m_p0 = p0;
        m_s1->setPosition( m_s1->getPosition() + offset );
        m_s2->setPosition( m_s2->getPosition() + offset );
    }
    WPosition p1 = m_s1->getPosition();
    WPosition p2 = m_s2->getPosition();

    WPosition v0( p1[0]                  , p2[1]                 , p0[2] - ( p0[2] - p1[2] ) - ( p0[2] - p2[2] ) );
    WPosition v1( p1[0]                  , p0[1] + p0[1] - p2[1] , p0[2] - ( p0[2] - p1[2] ) + ( p0[2] - p2[2] ) );
    WPosition v2( p0[0] + p0[0] - p1[0]  , p0[1] + p0[1] - p2[1] , p0[2] + ( p0[2] - p1[2] ) + ( p0[2] - p2[2] ) );
    WPosition v3( p0[0] + p0[0] - p1[0]  , p2[1]                 , p0[2] + ( p0[2] - p1[2] ) - ( p0[2] - p2[2] ) );

    osg::ref_ptr<osg::Geometry> planeGeometry = osg::ref_ptr<osg::Geometry>( new osg::Geometry() );
    osg::Vec3Array* planeVertices = new osg::Vec3Array;

    planeVertices->push_back( v0 );
    planeVertices->push_back( v1 );
    planeVertices->push_back( v2 );
    planeVertices->push_back( v3 );

    planeGeometry->setVertexArray( planeVertices );
    osg::DrawElementsUInt* quad = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
    quad->push_back( 3 );
    quad->push_back( 2 );
    quad->push_back( 1 );
    quad->push_back( 0 );
    planeGeometry->addPrimitiveSet( quad );

    m_geode->addDrawable( planeGeometry );

    m_dirty = false;
}

void WMArbitraryPlane::updateCallback()
{
    WPosition ch = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    WPosition cho = getCenterPosition();
    if( ch[0] != cho[0] || ch[1] != cho[1] || ch[2] != cho[2] )
    {
        setDirty();
    }

    if( isDirty() )
    {
        updatePlane();
    }
}

void WMArbitraryPlane::setDirty()
{
    m_dirty = true;
}

bool WMArbitraryPlane::isDirty()
{
    return m_dirty;
}

WPosition WMArbitraryPlane::getCenterPosition()
{
    return m_s0->getPosition();
}
