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

#include <boost/shared_ptr.hpp>

#include <osg/Geode>
#include <osg/Geometry>

#include "../../common/WColor.h"
#include "../../common/WLogger.h"
#include "../../graphicsEngine/WGEUtils.h"
#include "../../graphicsEngine/WROIBox.h"
#include "../../kernel/WKernel.h"
#include "WMFiberDisplay.h"
#include "fiberdisplay.xpm"

WMFiberDisplay::WMFiberDisplay()
    : WModule(),
      m_noData( new WCondition, true ),
      m_osgNode( osg::ref_ptr< osg::Group >() )
{
    m_shader = osg::ref_ptr< WShader > ( new WShader( "fake-tubes" ) );
}

WMFiberDisplay::~WMFiberDisplay()
{
}

boost::shared_ptr< WModule > WMFiberDisplay::factory() const
{
    return boost::shared_ptr< WModule >( new WMFiberDisplay() );
}

const char** WMFiberDisplay::getXPMIcon() const
{
    return fiberdisplay_xpm;
}

void WMFiberDisplay::moduleMain()
{
    // additional fire-condition: "data changed" flag
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_fiberInput->getDataChangedCondition() );

    ready();

    while ( !m_shutdownFlag() ) // loop until the module container requests the module to quit
    {
        debugLog() << "Waiting for event";

        m_moduleState.wait(); // waits for firing of m_moduleState ( dataChanged, shutdown, etc. )

        /////////////////////////////////////////////////////////////////////////////////////////
        // what caused wait to return?
        /////////////////////////////////////////////////////////////////////////////////////////

        // data changed?
        if ( m_dataset != m_fiberInput->getData() )
        {
            // data has changed
            // -> recalculate
            debugLog() << "Data changed on " << m_fiberInput->getCanonicalName();

            m_dataset = m_fiberInput->getData();

            // ensure the data is valid (not NULL)
            if ( !m_fiberInput->getData().get() ) // ok, the output has been reset, so we can ignore the "data change"
            {
                m_noData.set( true );
                debugLog() << "Data reset on " << m_fiberInput->getCanonicalName() << ". Ignoring.";
                continue;
            }

            m_noData.set( false );

            infoLog() << "Fiber dataset for display with: " << m_dataset->size() << " fibers loaded.";

            if( m_dataset->size() != 0 ) // incase of an empty fiber dataset nothing is to display
            {
                WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

                WKernel::getRunningKernel()->getRoiManager()->addFiberDataset( m_dataset );
                create();
            }
            else
            {
                warnLog() << "Nothing to display for an empty fiber dataset";
            }
        }
    }
}

void WMFiberDisplay::update()
{
    boost::shared_lock<boost::shared_mutex> slock;
    slock = boost::shared_lock<boost::shared_mutex>( m_updateLock );
    if( m_noData.changed() )
    {
        if( m_osgNode && m_noData.get( true ) )
        {
            m_osgNode->setNodeMask( 0x0 );
        }
        else
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
    }

    if ( WKernel::getRunningKernel()->getRoiManager()->isDirty() )
    {
        //m_tubeDrawable->dirtyDisplayList();
    }

    slock.unlock();
}

void WMFiberDisplay::create()
{
    // create new node
    osg::ref_ptr< osg::Group > osgNodeNew = osg::ref_ptr< osg::Group >( new osg::Group );

    m_tubeDrawable = osg::ref_ptr< WTubeDrawable >( new WTubeDrawable );
    m_tubeDrawable->setDataset( m_dataset );
    m_tubeDrawable->setUseDisplayList( false );
    m_tubeDrawable->setDataVariance( osg::Object::DYNAMIC );

    osg::ref_ptr< osg::Geode > geode = osg::ref_ptr< osg::Geode >( new osg::Geode );
    geode->addDrawable( m_tubeDrawable );

    osgNodeNew->addChild( geode );

    osgNodeNew->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->addChild( osgNodeNew.get() );

    osgNodeNew->setUserData( this );
    osgNodeNew->addUpdateCallback( new fdNodeCallback );

    // remove previous nodes if there are any
    WKernel::getRunningKernel()->getGraphicsEngine()->getScene()->removeChild( m_osgNode.get() );

    m_osgNode = osgNodeNew;

    activate();
}

void WMFiberDisplay::connectors()
{
    using boost::shared_ptr;
    typedef WModuleInputData< const WDataSetFibers > FiberInputData;  // just an alias

    m_fiberInput = shared_ptr< FiberInputData >( new FiberInputData( shared_from_this(), "fiberInput", "A loaded fiber dataset." ) );

    addConnector( m_fiberInput );
    WModule::connectors();  // call WModules initialization
}

void WMFiberDisplay::activate()
{
    if( m_osgNode )
    {
        if( m_active->get() )
        {
            m_osgNode->setNodeMask( 0xFFFFFFFF );
        }
        else
        {
            m_osgNode->setNodeMask( 0x0 );
        }
    }

    WModule::activate();
}

void WMFiberDisplay::properties()
{
    m_customColoring = m_properties2->addProperty( "Custom Coloring", "Switches the coloring between custom and predefined.", false );
    m_coloring = m_properties2->addProperty( "Global/Local Coloring", "Switches the coloring between global and local.", true );

    m_useTubesProp = m_properties2->addProperty( "Use Tubes", "Draw fiber tracts as fake tubes.", false );
    m_tubeThickness = m_properties2->addProperty( "Tube Thickness", "Adjusts the thickness of the tubes.", 50.,
            boost::bind( &WMFiberDisplay::adjustTubes, this ) );
    m_tubeThickness->setMin( 0 );
    m_tubeThickness->setMax( 1000 );
}

void WMFiberDisplay::toggleTubes()
{
    if( m_useTubesProp->changed() )
    {
        if ( m_useTubesProp->get( true ) )
        {
            m_tubeDrawable->setUseTubes( true );
            m_tubeDrawable->dirtyDisplayList();
            m_shader->apply( m_osgNode );
            osg::ref_ptr<osg::Uniform>( new osg::Uniform( "globalColor", 1 ) );
            osg::StateSet* rootState = m_osgNode->getOrCreateStateSet();
            rootState->addUniform( osg::ref_ptr<osg::Uniform>( new osg::Uniform( "globalColor", 1 ) ) );

            m_uniformTubeThickness = osg::ref_ptr<osg::Uniform>( new osg::Uniform( "u_thickness", static_cast<float>( m_tubeThickness->get() ) ) );
            rootState->addUniform( m_uniformTubeThickness );
        }
        else
        {
            m_tubeDrawable->setUseTubes( false );
            m_tubeDrawable->dirtyDisplayList();
            m_shader->deactivate( m_osgNode );
        }
    }
}

void WMFiberDisplay::toggleColoring()
{
    if( m_coloring->changed() || m_customColoring->changed() )
    {
        m_tubeDrawable->setColoringMode( m_coloring->get( true ) );
        m_tubeDrawable->setCustomColoring( m_customColoring->get( true ) );
    }
}

void WMFiberDisplay::adjustTubes()
{
    if ( m_tubeThickness->changed() && m_useTubesProp->get( true ) )
    {
        m_uniformTubeThickness->set( static_cast<float>( m_tubeThickness->get() ) );
    }
}
