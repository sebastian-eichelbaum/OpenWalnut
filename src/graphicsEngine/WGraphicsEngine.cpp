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

#include "exceptions/WGEInitFailed.h"
#include "../common/WLogger.h"
#include "WGraphicsEngine.h"
#include "WGEViewer.h"

WGraphicsEngine::WGraphicsEngine( std::string shaderPath ):
    WThreadedRunner(),
    m_shaderPath( shaderPath )
{
    WLogger::getLogger()->addLogMessage( "Initializing Graphics Engine", "GE", LL_DEBUG );

    // initialize members
    m_RootNode = new WGEScene();
}

WGraphicsEngine::~WGraphicsEngine()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Graphics Engine", "GE", LL_DEBUG );
}

osg::ref_ptr<WGEScene> WGraphicsEngine::getScene()
{
    return m_RootNode;
}

std::string WGraphicsEngine::getShaderPath()
{
    return m_shaderPath;
}

void WGraphicsEngine::threadMain()
{
    // TODO(ebaum): is this thread actually needed since each viewer runs in separate thread
    // while( !m_FinishRequested )
    // {
        // currently a dummy
        // sleep( 1 );
    // }
}

boost::shared_ptr<WGEViewer> WGraphicsEngine::createViewer( osg::ref_ptr<WindowData> wdata, int x, int y, int width, int height )
{
    boost::shared_ptr<WGEViewer> viewer = boost::shared_ptr<WGEViewer>( new WGEViewer(  wdata, x, y, width, height ) );
    viewer->setScene( this->getScene() );

    // start rendering
    viewer->run();

    // store it in viewer list
    // XXX is this list needed? If yes someone has to care about a deregisterViewer function
    // boost::mutex::scoped_lock lock(m_ViewerLock);

    // m_Viewer.push_back( viewer );

    return viewer;
}
