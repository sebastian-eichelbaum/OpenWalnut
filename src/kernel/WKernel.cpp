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

WKernel::WKernel( int argc, char* argv[] )
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
    std::cout << "Shutting down Kernel" << std::endl;
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


int WKernel::getArgumentCount() const
{
    return m_ArgC;
}

char** WKernel::getArguments() const
{
    // TODO( ebaum ): dasfdsf
    return m_ArgV;
}

int WKernel::run()
{
    std::cout << "Starting Kernel" << std::endl;

    // TODO(ebaum): add separate graphics thread here
    m_GraphicsEngine->run();

    // run Gui
    // TODO(all): clean up this option handler mess
    m_Gui->run();

    // run? data handler stuff?

    // **************************************************************************************************************************
    // This part will be exchanged by some kind of ModuleContainer managing module execution.
    // TODO(ebaum): replace by ModuleContainer
    // **************************************************************************************************************************
    
    // run module execution threads
    // TODO(ebaum): after having modules loaded they should be started here.
    // currently this is just the test module
    std::cout << "Starting modules:" << std::endl;
    for( std::list<boost::shared_ptr<WModule> >::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        std::cout << "Starting Module: " << ( *list_iter )->getName() << std::endl;
        ( *list_iter )->run();
    }

    // wait
    // TODO(ebaum): this is not the optimal. It would be better to quit OSG, GE and so on in the right order.
    m_Gui->wait( false );
    m_FinishRequested = true;

    // wait for modules to finish
    for( std::list<boost::shared_ptr<WModule> >::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
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
    std::cout << "Loading modules:" << std::endl;
    m_modules.clear();

    boost::shared_ptr<WModule> m = boost::shared_ptr<WModule>( new WNavigationSliceModule() );
    std::cout << "Loading Module: " << m->getName() << std::endl;

    m_modules.push_back( m );
}

void WKernel::init()
{
    // initialize

    // initialize graphics engine
    // this also includes initialization of WGEScene and OpenSceneGraph
    m_GraphicsEngine = boost::shared_ptr<WGraphicsEngine>( new WGraphicsEngine );

    // initialize GUI
    // TODO(all): clean up this option handler mess
    m_Gui = boost::shared_ptr<WMainApplication>( new WMainApplication() );

    // initialize Datahandler
    m_DataHandler = boost::shared_ptr<WDataHandler>( new WDataHandler() );

    findAppPath();
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
    m_ShaderPath = shaderPath + "shaders/";

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
    return m_ShaderPath;
}
