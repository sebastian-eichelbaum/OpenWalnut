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

#include <boost/thread/xtime.hpp>

#include "WKernel.h"
#include "WModule.h"
#include "../modules/navigationSlices/WNavigationSliceModule.h"
#include "../common/WException.h"

#include "../graphicsEngine/WGraphicsEngine.h"

/**
 * Used for program wide access to the kernel.
 */
WKernel* kernel = NULL;

WKernel::WKernel( int argc, char* argv[], boost::shared_ptr< WGUI > gui )
    : m_Gui( gui )
{
    std::cout << "Initializing Kernel" << std::endl;

    kernel = this;

    // initialize members
    m_ArgC = argc;
    m_ArgV = argv;
    m_FinishRequested = false;

    // init GE, DataHandler, ...
    init();

    // load modules
    loadModules();
}

WKernel::~WKernel()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_DEBUG );

    // finish running thread
    WLogger::getLogger()->wait( true );
    // write remaining log messages
    WLogger::getLogger()->processQueue();
}

WKernel::WKernel( const WKernel& other )
{
    *this = other;
}

WKernel* WKernel::getRunningKernel()
{
    return kernel;
}

boost::shared_ptr<WGraphicsEngine> WKernel::getGraphicsEngine() const
{
    return m_GraphicsEngine;
}

boost::shared_ptr<WDataHandler> WKernel::getDataHandler() const
{
    return m_DataHandler;
}

boost::shared_ptr< WGUI > WKernel::getGui()
{
    return m_Gui;
}

void WKernel::setGui( boost::shared_ptr< WGUI > gui )
{
    m_Gui = gui;
}

int WKernel::getArgumentCount() const
{
    return m_ArgC;
}

char** WKernel::getArguments() const
{
    return m_ArgV;
}

int WKernel::run()
{
    WLogger::getLogger()->addLogMessage( "Starting Kernel", "Kernel", LL_DEBUG );

    // TODO(ebaum): add separate graphics thread here
    m_GraphicsEngine->run();

    // run Gui
    // TODO(all): clean up this option handler mess
    m_Gui->run();

    // run? data handler stuff?

    // run module execution threads
    // TODO(ebaum): after having modules loaded they should be started here.
    // currently this is just the test module
    WLogger::getLogger()->addLogMessage( "*** Starting modules:", "Kernel", LL_DEBUG );
    for( std::list<WModule*>::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        WLogger::getLogger()->addLogMessage( "Starting module: " + ( *list_iter )->getName(), "Kernel", LL_DEBUG );
        ( *list_iter )->run();
    }

    // wait
    m_Gui->wait( false );
    m_FinishRequested = true;

    // wait for modules to finish
    for( std::list<WModule*>::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        ( *list_iter )->wait( true );
    }

    // finally GE
    m_GraphicsEngine->wait( true );

    // how to get QT return code from its thread?
    return 0;
}

void WKernel::loadModules()
{
    // TODO(ebaum): add dynamic loading here
    WLogger::getLogger()->addLogMessage( "*** Loading Modules: ", "Kernel", LL_DEBUG );
    m_modules.clear();

    WModule* m = new WNavigationSliceModule();
    WLogger::getLogger()->addLogMessage( "Loading module: " + m->getName(), "Kernel", LL_DEBUG );


    m_modules.push_back( m );
}

void WKernel::init()
{
    // initialize
    findAppPath();

    // initialize graphics engine
    // this also includes initialization of WGEScene and OpenSceneGraph
    m_GraphicsEngine = boost::shared_ptr<WGraphicsEngine>( new WGraphicsEngine( m_shaderPath ) );

    // TODO(all): clean up this option handler mess

    // initialize Datahandler
    m_DataHandler = boost::shared_ptr<WDataHandler>( new WDataHandler() );
}

bool WKernel::findAppPath()
{
    // FIXME (schurade)
    // this should work on linux, have to implement it for windows and mac later

    int length;
    char appPath[255];

    length = readlink( "/proc/self/exe", appPath, sizeof( appPath ) );

    // Catch some errors
    if ( length < 0 )
    {
        fprintf( stderr, "Error resolving symlink /proc/self/exe.\n" );
        return false;
    }
    if ( length >= 255 )
    {
        fprintf( stderr, "Path too long. Truncated.\n" );
        return false;
    }

    // the string this readlink() function returns is appended with a '@'.
    appPath[length] = '\0';

    // strip off the executable name
    while ( appPath[length] != '/' )
    {
        appPath[length] = '\0';
        --length;
    }

    m_AppPath = appPath;

    std::string shaderPath( appPath );
    m_shaderPath = shaderPath + "shaders/";

    return true;
}

bool WKernel::isFinishRequested() const
{
    return m_FinishRequested;
}

void WKernel::doLoadDataSets( std::vector< std::string > fileNames )
{
    m_DataHandler->loadDataSets( fileNames );
}

boost::shared_ptr<WDataHandler> WKernel::getDataHandler()
{
    return m_DataHandler;
}

std::string WKernel::getAppPath()
{
    return m_AppPath;
}

std::string WKernel::getShaderPath()
{
    return m_shaderPath;
}
