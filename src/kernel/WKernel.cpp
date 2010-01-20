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

#ifdef __linux__
#include <unistd.h> // used for getcwd (to get current directory)
#endif

#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/thread/xtime.hpp>

#include "WModule.h"
#include "WModuleFactory.h"
#include "WBatchLoader.h"
#include "../common/WException.h"
#include "../common/WCondition.h"
#include "../common/WConditionOneShot.h"
#include "../common/WFlag.h"
#include "../common/WPreferences.h"

#include "../graphicsEngine/WGraphicsEngine.h"

#include "WKernel.h"

/**
 * Used for program wide access to the kernel.
 */
WKernel* kernel = NULL;

/**
 * Define shader path.
 */
std::string WKernel::m_shaderPath = std::string();

/**
 * Define app path.
 */
std::string WKernel::m_AppPath = std::string();

WKernel::WKernel( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui ):
    WThreadedRunner()
{
    WLogger::getLogger()->addLogMessage( "Initializing Kernel", "Kernel", LL_INFO );

    // init the singleton
    kernel = this;

    // initialize members
    m_gui = gui;
    m_graphicsEngine = ge;

    // init
    init();
}

WKernel::~WKernel()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_INFO );
}

void WKernel::init()
{
    // initialize
    findAppPath();

    // get module factory
    m_moduleFactory = WModuleFactory::getModuleFactory();

    // initialize module container
    m_moduleContainer = boost::shared_ptr< WModuleContainer >( new WModuleContainer( "KernelRootContainer",
                "Root module container in Kernel." ) );

    // initialize graphics engine, or, at least set some stuff
    m_graphicsEngine->setShaderPath( WKernel::m_shaderPath );

    // load all modules
    m_moduleFactory->load();
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

void WKernel::finalize()
{
    WLogger::getLogger()->addLogMessage( "Stopping Kernel", "Kernel", LL_INFO );

    // NOTE: stopping a container erases all modules inside.
    getRootContainer()->stop();
}

void WKernel::threadMain()
{
    WLogger::getLogger()->addLogMessage( "Starting Kernel", "Kernel", LL_INFO );

    // wait for GUI to be initialized properly
    m_gui->isInitialized().wait();

    // start GE
    m_graphicsEngine->run();

    // default modules
    {
        bool ignore;
        m_moduleContainer->add( m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "Navigation Slice Module" ) ) , true );
        m_moduleContainer->add( m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "Coordinate System Module" ) ) , true );
        if( !( WPreferences::getPreference( "modules.standard.ignoreHUD", &ignore ) && ignore ) )
        {
            m_moduleContainer->add( m_moduleFactory->create( m_moduleFactory->getPrototypeByName( "HUD" ) ) , true );
        }
    }

    // actually there is nothing more to do here
    waitForStop();

    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_INFO );
}

void WKernel::findAppPath()
{
    // only get the path if not already done
    if ( m_AppPath != "" )
    {
        return;
    }

    if ( m_AppPath != "" )
    {
        return;
    }

    // unified version with boost::filesystem
    namespace fs = boost::filesystem;
    fs::path currentDir( fs::initial_path<fs::path>() );

    m_AppPath = currentDir.file_string();
//    std::cout << "Set app path to " << m_AppPath << std::endl;

    m_shaderPath = fs::path( currentDir / "shaders" ).file_string();
//    std::cout << "Set shader path to " << m_shaderPath << std::endl;
}

const WBoolFlag& WKernel::isFinishRequested() const
{
    return m_shutdownFlag;
}

void WKernel::loadDataSets( std::vector< std::string > fileNames )
{
    getRootContainer()->loadDataSets( fileNames );
}

void WKernel::loadDataSetsSynchronously( std::vector< std::string > fileNames )
{
    getRootContainer()->loadDataSetsSynchronously( fileNames );
}

boost::shared_ptr< WModule > WKernel::applyModule( boost::shared_ptr< WModule > applyOn, boost::shared_ptr< WModule > prototype )
{
    return getRootContainer()->applyModule( applyOn, prototype );
}

std::string WKernel::getAppPath()
{
    findAppPath();
    return WKernel::m_AppPath;
}

std::string WKernel::getShaderPath()
{
    findAppPath();
    return WKernel::m_shaderPath;
}
