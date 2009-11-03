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
#include "../modules/data/WDataModule.hpp"
#include "../modules/navigationSlices/WNavigationSliceModule.h"
#include "../modules/fiberDisplay/WFiberDisplay.h"
#include "../modules/fiberCulling/WFiberCulling.h"
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

boost::shared_ptr< WDataHandler > WKernel::getDataHandler() const
{
    return m_dataHandler;
}

boost::shared_ptr< WGUI > WKernel::getGui()
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

int WKernel::run()
{
    WLogger::getLogger()->addLogMessage( "Starting Kernel", "Kernel", LL_DEBUG );

    // TODO(ebaum): add separate graphics thread here
    m_graphicsEngine->run();

    // run Gui
    // TODO(all): clean up this option handler mess
    m_gui->run();

    // run? data handler stuff?

    // **************************************************************************************************************************
    // This part will be exchanged by some kind of ModuleContainer managing module execution.
    // TODO(ebaum): replace by ModuleContainer
    // **************************************************************************************************************************

    // run module execution threads
    // TODO(ebaum): after having modules loaded they should be started here.
    // currently this is just the test module
    WLogger::getLogger()->addLogMessage( "*** Starting modules:", "Kernel", LL_DEBUG );
    for( std::list< boost::shared_ptr< WModule > >::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        WLogger::getLogger()->addLogMessage( "Starting module: " + ( *list_iter )->getName(), "Kernel", LL_DEBUG );
        ( *list_iter )->run();
    }

    // TODO(schurade): this must be moved somewhere else, and realize the wait loop in another fashion
    while ( !m_gui->isInitalized() )
    {
    }
    m_gui->getLoadButtonSignal()->connect( boost::bind( &WKernel::doLoadDataSets, this, _1 ) );

    for( std::list< boost::shared_ptr< WModule > >::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
                ++list_iter )
    {
        ( *list_iter )->connectToGui();
    }

    // wait
    // TODO(ebaum): this is not the optimal. It would be better to quit OSG, GE and so on in the right order.
    m_gui->wait( false );
    m_FinishRequested = true;

    // wait for modules to finish
    for( std::list< boost::shared_ptr< WModule > >::iterator list_iter = m_modules.begin(); list_iter != m_modules.end();
            ++list_iter )
    {
        ( *list_iter )->wait( true );
    }

    // finally GE
    m_graphicsEngine->wait( true );

    // how to get QT return code from its thread?
    return 0;
}

void WKernel::loadModules()
{
    // TODO(ebaum): add dynamic loading here
    WLogger::getLogger()->addLogMessage( "*** Loading Modules: ", "Kernel", LL_DEBUG );
    m_modules.clear();

    using boost::shared_ptr;
    shared_ptr< WModule > m = shared_ptr< WModule >( new WNavigationSliceModule() );
    // shared_ptr< WModule > m = shared_ptr< WModule >( new WFiberDisplay() );
    // shared_ptr< WModule > m = shared_ptr< WModule >( new WFiberCulling() );
    WLogger::getLogger()->addLogMessage( "Loading module: " + m->getName(), "Kernel", LL_DEBUG );

    m_modules.push_back( m );
}

void WKernel::init()
{
     // initialize
    findAppPath();

    // initialize graphics engine
    // this also includes initialization of WGEScene and OpenSceneGraph
    m_graphicsEngine = boost::shared_ptr< WGraphicsEngine >( new WGraphicsEngine( m_shaderPath ) );

    // initialize Datahandler
    m_dataHandler = boost::shared_ptr< WDataHandler >( new WDataHandler() );

    // m_gui->createMainWindow();

    m_dataHandler->getSignalAddDataset()->connect( boost::bind( &WKernel::slotFinishLoadData, this, _1 ) );
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
    m_dataHandler->loadDataSets( fileNames );
}

void WKernel::slotFinishLoadData( boost::shared_ptr< WDataSet > dataSet )
{
    boost::shared_ptr< WModule > module = boost::shared_ptr< WModule >( new WDataModule< int >( dataSet ) );

    module->getProperties()->addBool( "active", true );
    module->getProperties()->hideProperty( "active" );
    module->getProperties()->addBool( "interpolation", true );
    module->getProperties()->addInt( "threshold", 0 );
    module->getProperties()->addInt( "alpha", 100 );
    module->getProperties()->setMax( "alpha", 100 );
    module->getProperties()->setValue( "name", dataSet->getFileName() );

    m_gui->addDatasetToBrowser( module, 0 );
}

boost::shared_ptr< WDataHandler > WKernel::getDataHandler()
{
    return m_dataHandler;
}

std::string WKernel::getAppPath()
{
    return m_AppPath;
}

std::string WKernel::getShaderPath()
{
    return m_shaderPath;
}
