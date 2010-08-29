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

#include <stdlib.h>

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
#include "../common/WPathHelper.h"
#include "../common/WPreferences.h"
#include "../common/math/WPosition.h"
#include "WGEViewer.h"
#include "WGraphicsEngine.h"
#include "exceptions/WGEInitFailed.h"
#include "exceptions/WGESignalSubscriptionFailed.h"

// graphics engine instance as singleton
boost::shared_ptr< WGraphicsEngine > WGraphicsEngine::m_instance = boost::shared_ptr< WGraphicsEngine >();

WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_INFO );

    // NOTE: the osgViewer::StatsHandler uses a hard coded font filename. :-(. Fortunately OSG allows us to modify the search path using
    // environment variables:
#ifndef _WIN32
    setenv( "OSGFILEPATH", WPathHelper::getFontPath().file_string().c_str(), 1 );
#else
    std::string envStr = std::string( "OSGFILEPATH=" ) + WPathHelper::getFontPath().file_string();
    putenv( envStr.c_str() );
#endif

    // initialize members
    m_rootNode = new WGEScene();
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

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

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
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );
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
