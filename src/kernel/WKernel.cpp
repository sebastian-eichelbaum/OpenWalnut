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

// Use filesystem version 2 for compatibility with newer boost versions.
#ifndef BOOST_FILESYSTEM_VERSION
    #define BOOST_FILESYSTEM_VERSION 2
#endif
#include <boost/filesystem.hpp>
#include <boost/thread/xtime.hpp>

#include "../common/WPreferences.h"
#include "../common/WStringUtils.h"
#include "../common/WThreadedRunner.h"
#include "../common/WTimer.h"
#include "../common/WRealtimeTimer.h"
#include "../dataHandler/WDataHandler.h"
#include "../gui/WGUI.h"
#include "WKernel.h"
#include "WModule.h"
#include "WModuleContainer.h"
#include "WModuleFactory.h"
#include "WROIManager.h"
#include "WSelectionManager.h"

/**
 * Used for program wide access to the kernel.
 */
WKernel* WKernel::m_kernel = NULL;

WKernel::WKernel( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui ):
    WThreadedRunner(),
    m_timer( WTimer::SPtr( new WRealtimeTimer() ) )
{
    WLogger::getLogger()->addLogMessage( "Initializing Kernel", "Kernel", LL_INFO );

    // init the singleton
    m_kernel = this;

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

WKernel* WKernel::instance( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WGUI > gui )
{
    if( m_kernel == NULL )
    {
        new WKernel( ge, gui ); // m_kernel will be set in the constructor.
    }

    return m_kernel;
}

void WKernel::init()
{
    // initialize
    m_roiManager = boost::shared_ptr< WROIManager >( new WROIManager() );

    m_selectionManager = boost::shared_ptr< WSelectionManager >( new WSelectionManager() );

    // get module factory
    m_moduleFactory = WModuleFactory::getModuleFactory();

    // init data handler
    WDataHandler::getDataHandler();

    // initialize module container
    m_moduleContainer = boost::shared_ptr< WModuleContainer >( new WModuleContainer( "KernelRootContainer",
                "Root module container in Kernel." ) );
    // this avoids the root container to be marked as "crashed" if a contained module crashes.
    m_moduleContainer->setCrashIfModuleCrashes( false );

    // load all modules
    m_moduleFactory->load();
}

WKernel* WKernel::getRunningKernel()
{
    return m_kernel;
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

    WLogger::getLogger()->addLogMessage( "Stopping Data Handler", "Kernel", LL_INFO );
    WDataHandler::getDataHandler()->clear();
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
        std::string stdModules = "";
        WPreferences::getPreference( "modules.default", &stdModules );
        std::vector< std::string > defMods = string_utils::tokenize( stdModules, "," );
        for ( std::vector< std::string >::iterator iter = defMods.begin(); iter != defMods.end(); ++iter )
        {
            std::string moduleName = string_utils::trim( ( *iter ) );
            boost::shared_ptr< WModule> proto = m_moduleFactory->isPrototypeAvailable( moduleName );

            // try to find a prototype
            if ( proto.get() )
            {
                // the module exists
                m_moduleContainer->add( m_moduleFactory->create( proto ) , true );
            }
            else
            {
                WLogger::getLogger()->addLogMessage( "Specified default module \"" + moduleName + "\" does not exist. Ignoring.",
                                                     "Kernel",
                                                     LL_WARNING );
            }
        }
    }

    // actually there is nothing more to do here
    waitForStop();

    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_INFO );
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

boost::shared_ptr< WROIManager > WKernel::getRoiManager()
{
    return m_roiManager;
}

boost::shared_ptr< WSelectionManager>WKernel::getSelectionManager()
{
    return m_selectionManager;
}

WTimer::ConstSPtr WKernel::getTimer() const
{
    return m_timer;
}

