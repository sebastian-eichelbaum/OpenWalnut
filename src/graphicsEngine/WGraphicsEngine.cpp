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

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/ref_ptr>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>
#include <osgViewer/Viewer>

#include "../common/WColor.h"
#include "../common/WLogger.h"
#include "../common/WPreferences.h"
#include "../common/math/WPosition.h"
#include "WGEViewer.h"
#include "WGraphicsEngine.h"
#include "exceptions/WGEInitFailed.h"
#include "exceptions/WGESignalSubscriptionFailed.h"
#include "WGEResourceManager.h"

// graphics engine instance as singleton
boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::m_instance = boost::shared_ptr< WGraphicsEngine >();

WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_INFO );

    // initialize members
    m_rootNode = new WGEScene();

    m_shaderPath = "";
    m_fontPath = "";

    // initialize OSG render window
    m_viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );

    // ThreadingModel: enum with the following possibilities
    //
    //  SingleThreaded
    //  CullDrawThreadPerContext
    //  ThreadPerContext
    //  DrawThreadPerContext
    //  CullThreadPerCameraDrawThreadPerContext
    //  ThreadPerCamera
    //  AutomaticSelection
    bool multiThreadedViewers = true;
    if( WPreferences::getPreference( "ge.multiThreadedViewers", &multiThreadedViewers ) && !multiThreadedViewers )
    {
        m_viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );
    }
    else
    {
        m_viewer->setThreadingModel( osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext );
    }

    // init resource manager ( it is a singleton and gets created during first "getResourceManager" request.
    WGEResourceManager::getResourceManager();
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_INFO );
}


boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::getGraphicsEngine()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine() );
    }

    return m_instance;
}

osg::ref_ptr<WGEScene> WGraphicsEngine::getScene()
{
    return m_rootNode;
}

std::string WGraphicsEngine::getShaderPath() const
{
    return m_shaderPath;
}

void WGraphicsEngine::setShaderPath( std::string path )
{
    m_shaderPath = path;
}

std::string WGraphicsEngine::getFontPath() const
{
    return m_fontPath;
}

void WGraphicsEngine::setFontPath( std::string path )
{
    m_fontPath = path;

    // we need to propagate the change to the resource manager
    WGEResourceManager::getResourceManager()->setFontPath( path );
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, osg::ref_ptr<osg::Referenced> wdata, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    // init the composite viewer if not already done
    if ( m_viewer == osg::ref_ptr< osgViewer::CompositeViewer >() )
    {
    }

    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, wdata, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

    // finally add view
    m_viewer->addView( viewer->getView() );

    // store it in viewer list
    boost::mutex::scoped_lock lock( m_viewersLock );
    bool insertSucceeded = m_viewers.insert( make_pair( name, viewer ) ).second;
    assert( insertSucceeded == true );

    return viewer;
}

void WGraphicsEngine::closeViewer( const std::string name )
{
    boost::mutex::scoped_lock lock( m_viewersLock );
    if( m_viewers.count( name ) > 0 )
    {
        m_viewers[name]->close();

        m_viewers.erase( name );
    }
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewerByName( std::string name )
{
    boost::mutex::scoped_lock lock( m_viewersLock );
    boost::shared_ptr< WGEViewer > out = m_viewers.count( name ) > 0 ?
        m_viewers[name] :
        boost::shared_ptr< WGEViewer >();
    return out;
}

boost::shared_ptr< WGEViewer > WGraphicsEngine::getViewer()
{
    boost::mutex::scoped_lock lock( m_viewersLock );
    return m_viewers[ "main" ];
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_INFO );

    m_viewer->startThreading();
    m_viewer->run();
    m_viewer->stopThreading();
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );

    m_viewer->setDone( true );
}

void WGraphicsEngine::requestShaderReload()
{
    m_reloadShadersSignal();
}

boost::signals2::connection WGraphicsEngine::subscribeSignal( GE_SIGNAL signal, t_GEGenericSignalHandlerType notifier )
{
    switch ( signal )
    {
        case GE_RELOADSHADERS:
            return m_reloadShadersSignal.connect( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WGESignalSubscriptionFailed( s.str() );
            break;
    }
}
