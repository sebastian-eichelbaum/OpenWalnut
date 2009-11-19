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

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>

#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/View>

#include "exceptions/WGEInitFailed.h"
#include "../common/WLogger.h"
#include "WGraphicsEngine.h"
#include "WGEViewer.h"


WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_DEBUG );

    // initialize members
    m_rootNode = new WGEScene();

    m_shaderPath = "";

    // initialize OSG render window
    m_Viewer = osg::ref_ptr<osgViewer::CompositeViewer>( new osgViewer::CompositeViewer() );

    // ThreadingModel: enum with the following possibilities
    //
    //  SingleThreadet
    //  CullDrawThreadPerContext
    //  ThreadPerContext
    //  DrawThreadPerContext
    //  CullThreadPerCameraDrawThreadPerContext
    //  ThreadPerCamera
    //  AutomaticSelection
    m_Viewer->setThreadingModel( osgViewer::Viewer::CullThreadPerCameraDrawThreadPerContext );
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_DEBUG );
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

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer(
    osg::ref_ptr<WindowData> wdata, int x, int y, int width, int height, WGECamera::ProjectionMode projectionMode )
{
    // init the composite viewer if not already done
    if ( m_Viewer == osg::ref_ptr< osgViewer::CompositeViewer >() )
    {
    }

    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>( new WGEViewer(  wdata, x, y, width, height, projectionMode ) );
    viewer->setScene( getScene() );

    // finally add view
    m_Viewer->addView( viewer->getViewer().get() );

    // store it in viewer list
    // XXX is this list needed? If yes, someone has to care about a deregisterViewer function
    // boost::mutex::scoped_lock lock(m_ViewerLock);
    //m_Viewers.push_back( viewer );

    return viewer;
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_DEBUG );

    m_Viewer->startThreading();
    m_Viewer->run();
    m_Viewer->stopThreading();
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_DEBUG );

    m_Viewer->setDone( true );
}

