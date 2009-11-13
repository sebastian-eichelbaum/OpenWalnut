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

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/thread/xtime.hpp>

#include "WModule.h"
#include "WModuleFactory.h"
#include "../common/WException.h"

#include "../graphicsEngine/WGraphicsEngine.h"

#include "WKernel.h"

/**
 * Used for program wide access to the kernel.
 */
WKernel* kernel = NULL;

WKernel::WKernel( int argc, char* argv[], boost::shared_ptr< WGUI > gui )
    : m_gui( gui )
{
    WLogger::getLogger()->addLogMessage( "Initializing Kernel", "Kernel", LL_DEBUG );

    kernel = this;

    // initialize members
    m_ArgC = argc;
    m_ArgV = argv;
    m_FinishRequested = false;

    // get module factory
    m_moduleFactory = WModuleFactory::getModuleFactory();
    m_moduleContainer = boost::shared_ptr< WModuleContainer >();

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

boost::shared_ptr< WGraphicsEngine > WKernel::getGraphicsEngine() const
{
    return m_graphicsEngine;
}

boost::shared_ptr< WModuleContainer > WKernel::getRootContainer() const
{
    return m_moduleContainer;
}

boost::shared_ptr< WGUI > WKernel::getGui() const
{
    return m_gui;
}

void WKernel::setGui( boost::shared_ptr< WGUI > gui )
{
    m_gui = gui;
}

int WKernel::getArgumentCount() const
{
    return m_ArgC;
}

char** WKernel::getArguments() const
{
    return m_ArgV;
}

void WKernel::stop()
{
    getRootContainer()->stop();
}

int WKernel::run()
{
    WLogger::getLogger()->addLogMessage( "Starting Kernel", "Kernel", LL_DEBUG );

    // run Gui
    m_gui->run();

    // run? data handler stuff?

    // TODO(schurade): this must be moved somewhere else, and realize the wait loop in another fashion
    while ( !m_gui->isInitalized() )
    {
    }
    m_gui->getLoadButtonSignal()->connect( boost::bind( &WKernel::doLoadDataSets, this, _1 ) );

    // default modules
    //m_moduleContainer->add( m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "Navigation Slice Module" ) ) );
    //m_moduleContainer->add( m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "Coordinate System Module" ) ) );

    m_gui->wait( false );
    m_FinishRequested = true;
    m_moduleContainer->stop();

    // how to get QT return code from its thread?
    return 0;
}

void WKernel::loadModules()
{
    // load all modules
    m_moduleFactory->load();
}

void WKernel::init()
{
     // initialize
    findAppPath();

    // initialize module container
    m_moduleContainer = boost::shared_ptr< WModuleContainer >( new WModuleContainer( "KernelRootContainer",
                "Root module container in Kernel." ) );

    // initialize graphics engine
    // this also includes initialization of WGEScene and OpenSceneGraph
    m_graphicsEngine = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine( m_shaderPath ) );

    // initialize Datahandler
    m_dataHandler = boost::shared_ptr< WDataHandler >( new WDataHandler() );
}

bool WKernel::findAppPath()
{
    // FIXME (schurade)
    // this should work on linux, have to implement it for windows and mac later
#ifdef __linux__
    int length;
    char appPath[255];

    length = readlink( "/proc/self/exe", appPath, sizeof( appPath ) );

    // Catch some errors
    if ( length < 0 )
    {
        WLogger::getLogger()->addLogMessage( "Error resolving symlink /proc/self/exe.", "Kernel", LL_ERROR );
        return false;
    }
    if ( length >= 255 )
    {
        WLogger::getLogger()->addLogMessage( "Path too long. Truncated.", "Kernel", LL_ERROR );
        return false;
    }

    // the string this readlink() function returns is appended with a '@'.
    appPath[length] = '\0';

    // strip off the executable name
    while ( appPath[length] != '/' )
    {
        appPath[length] = '\0';
        --length;
        assert( length >= 0 );
    }

    m_AppPath = appPath;

    std::string shaderPath( appPath );
    m_shaderPath = shaderPath + "shaders/";

    return true;
#elif defined( __APPLE__ )
    char path[1024];
    uint32_t size = sizeof( path );
    if( _NSGetExecutablePath( path, &size ) == 0 )
    {
        WLogger::getLogger()->addLogMessage( "Executable path is " + std::string( path ), "Kernel", LL_ERROR );

        int i = strlen( path );
        while( path[i] != '/' )
        {
            path[i] = '\0';
            i--;
            assert( i >= 0 );
        }
        WLogger::getLogger()->addLogMessage( "Application path is " + std::string( path ), "Kernel", LL_ERROR );
        m_AppPath = path;

        std::string shaderPath( path );
        m_shaderPath = shaderPath + "shaders/";

        return true;
    }
    else
    {
        WLogger::getLogger()->addLogMessage( "Buffer too small; need size " + boost::lexical_cast< std::string >( size ),
                                             "Kernel", LL_ERROR );
        assert( size <= sizeof( path ) );
    }
#else
#error findAppPath not implemented for this platform
#endif

    return false;
}

bool WKernel::isFinishRequested() const
{
    return m_FinishRequested;
}

void WKernel::doLoadDataSets( std::vector< std::string > fileNames )
{
    // add a new data module for each file to load
    for ( std::vector< std::string >::iterator iter = fileNames.begin(); iter != fileNames.end(); ++iter )
    {
        boost::shared_ptr< WModule > mod = m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "Data Module" ) );
        mod->getProperties()->setValue( "filename" , ( *iter ) );
        m_moduleContainer->add( mod );
    }
}

boost::shared_ptr< WDataHandler > WKernel::getDataHandler() const
{
    return m_dataHandler;
}

std::string WKernel::getAppPath() const
{
    return m_AppPath;
}

std::string WKernel::getShaderPath() const
{
    return m_shaderPath;
}
