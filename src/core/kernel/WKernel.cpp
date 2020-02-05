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

#include <stdlib.h>
#include <string>
#include <vector>

#include "../common/WLogger.h"
#include "../common/WThreadedRunner.h"
#include "../common/WTimer.h"
#include "../common/WRealtimeTimer.h"
#include "../dataHandler/WDataHandler.h"
#include "../ui/WUI.h"
#include "exceptions/WKernelException.h"
#include "WKernel.h"
#include "WModuleContainer.h"
#include "WModuleFactory.h"
#include "WROIManager.h"
#include "WSelectionManager.h"

#include "core/WVersion.h"   // NOTE: this file is auto-generated by CMAKE

/**
 * Used for program wide access to the kernel.
 */
WKernel* WKernel::m_kernel = NULL;

WKernel::WKernel( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WUI > ui ):
    WThreadedRunner(),
    m_timer( WTimer::SPtr( new WRealtimeTimer() ) ),
    m_unattendedMode( false )
{
    WLogger::getLogger()->addLogMessage( "Initializing Kernel", "Kernel", LL_INFO );
    wlog::debug( "Kernel" ) << "Version: " << W_VERSION;

    setThreadName( "Kernel" );

    // init the singleton
    m_kernel = this;

    // initialize members
    m_ui = ui;
    m_graphicsEngine = ge;

    // init
    init();
}

WKernel::~WKernel()
{
    // cleanup
    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_INFO );
}

WKernel* WKernel::instance( boost::shared_ptr< WGraphicsEngine > ge, boost::shared_ptr< WUI > ui )
{
    if( m_kernel == NULL )
    {
        new WKernel( ge, ui ); // m_kernel will be set in the constructor.
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

    m_scriptEngine = boost::shared_ptr< WScriptEngine >( new WScriptEngine( m_moduleContainer ) );
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

boost::shared_ptr< WUI > WKernel::getUI() const
{
    return m_ui;
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

    // wait for UI to be initialized properly
    if( m_ui )
    {
        m_ui->isInitialized().wait();
    }
    else
    {
        wlog::warn( "Kernel" ) << "Expected UI instance but none was initialized.";
    }

    // start GE
    if( m_graphicsEngine )
    {
        m_graphicsEngine->run();

        // wait for it to be ready
        m_graphicsEngine->waitForFinalize();
    }
    else
    {
        wlog::warn( "Kernel" ) << "Expected GE instance but none was initialized.";
    }

    // do extension loading
    wlog::info( "Kernel" ) << "Initializing extensions.";
    WModuleFactory::getModuleLoader()->initializeExtensions();

    // done. Notify anyone waiting
    wlog::info( "Kernel" ) << "Initialization completed.";
    m_startupCompleted.notify();

    // actually there is nothing more to do here
    waitForStop();

    WLogger::getLogger()->addLogMessage( "Shutting down Kernel", "Kernel", LL_INFO );
}

const WBoolFlag& WKernel::isFinishRequested() const
{
    return m_shutdownFlag;
}

WBatchLoader::SPtr WKernel::loadDataSets( std::vector< std::string > filenames, bool suppressColormaps )
{
    return getRootContainer()->loadDataSets( filenames, suppressColormaps );
}

WBatchLoader::SPtr WKernel::loadDataSetsSynchronously( std::vector< std::string > filenames, bool suppressColormaps )
{
    return getRootContainer()->loadDataSetsSynchronously( filenames, suppressColormaps );
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

boost::shared_ptr<WScriptEngine> WKernel::getScriptEngine()
{
    return m_scriptEngine;
}

WTimer::ConstSPtr WKernel::getTimer() const
{
    return m_timer;
}

boost::signals2::connection WKernel::subscribeSignal( WKernel::KERNEL_SIGNAL signal, WKernel::t_KernelGenericSignalHandlerType notifier )
{
    switch( signal )
    {
        case KERNEL_STARTUPCOMPLETE:
            return m_startupCompleted.subscribeSignal( notifier );
        default:
            std::ostringstream s;
            s << "Could not subscribe to unknown signal.";
            throw WKernelException( s.str() );
            break;
    }
}

void WKernel::quit( bool hard ) const
{
    if( hard )
    {
        exit( 0 );
    }
}

bool WKernel::isUnattendedMode() const
{
    return m_unattendedMode;
}

void WKernel::setUnattendedMode( bool unattended )
{
    m_unattendedMode = unattended;
}
