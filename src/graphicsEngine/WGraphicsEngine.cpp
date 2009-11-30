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
#include "../common/WColor.h"
#include "WGraphicsEngine.h"
#include "WGEViewer.h"


WGraphicsEngine::WGraphicsEngine():
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_INFO );

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
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_INFO );
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

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( std::string name, osg::ref_ptr<WindowData> wdata, int x, int y,
                                                            int width, int height, WGECamera::ProjectionMode projectionMode,
                                                            WColor bgColor )
{
    // init the composite viewer if not already done
    if ( m_Viewer == osg::ref_ptr< osgViewer::CompositeViewer >() )
    {
    }

    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>(
        new WGEViewer( name, wdata, x, y, width, height, projectionMode ) );
    viewer->setBgColor( bgColor );
    viewer->setScene( getScene() );

    // finally add view
    m_Viewer->addView( viewer->getViewer().get() );

    // store it in viewer list
    // TODO(cornimueller): someone has to care about a deregisterViewer function
    boost::mutex::scoped_lock lock( m_ViewersLock );
    assert( m_Viewers.insert( make_pair( name, viewer ) ).second == true );
    m_ViewersLock.unlock();

    return viewer;
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::getViewerByName( std::string name )
{
    return m_Viewers[name];
}

void WGraphicsEngine::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Graphics Engine", "GE", LL_INFO );

    m_Viewer->startThreading();
    m_Viewer->run();
    m_Viewer->stopThreading();
}

void WGraphicsEngine::notifyStop()
{
    WLogger::getLogger()->addLogMessage( "Stopping Graphics Engine", "GE", LL_INFO );

    m_Viewer->setDone( true );
}

