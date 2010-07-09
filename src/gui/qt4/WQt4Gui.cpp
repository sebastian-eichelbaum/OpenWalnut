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
#include <fstream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <QtGui/QApplication>
#include <QtGui/QFileDialog>

#include "WMainWindow.h" // this has to be included before any other includes
#include "../../common/WConditionOneShot.h"
#include "../../common/WIOTools.h"
#include "../../common/WPreferences.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../graphicsEngine/WGraphicsEngine.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WProjectFile.h"
#include "events/WModuleAssocEvent.h"
#include "events/WModuleCrashEvent.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleRemovedEvent.h"
#include "events/WModuleConnectEvent.h"
#include "events/WModuleDisconnectEvent.h"
#include "events/WOpenCustomDockWidgetEvent.h"
#include "events/WRoiAssocEvent.h"
#include "events/WRoiRemoveEvent.h"
#include "events/WUpdateTextureSorterEvent.h"

#include "WQt4Gui.h"

WMainWindow* WQt4Gui::m_mainWindow = NULL;

WQt4Gui::WQt4Gui( int argc, char** argv )
    : WGUI( argc, argv )
{
}

WQt4Gui::~WQt4Gui()
{
}

bool WQt4Gui::parseOptions()
{
    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here
    po::options_description desc( "Allowed options" );

    desc.add_options()
        ( "help,h", "Prints this help message" )
        ( "project,p", po::value< std::string >(), "Project file to be loaded on startup." )
        ( "input,i", po::value< std::vector< std::string > >(), "Input data files that should be loaded automatically" )
        ( "timed-output,t", "Flag indicating if all log strings should have a time string preceding" );

    po::positional_options_description p;
    p.add( "input", -1 );

    try
    {
        po::store( po::command_line_parser( m_argc, m_argv ).options( desc ).positional( p ).run(), m_optionsMap );
    }
    catch( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    po::notify( m_optionsMap );

    // print usage information if command line asks for help.
    if( m_optionsMap.count( "help" ) )
    {
        std::cout << desc << std::endl;
    }
    else if( m_optionsMap.count( "timed-output" ) )
    {
        WLogger::getLogger()->setDefaultFormat( "[%t] *%l [%s] %m \n" );
        wlog::info( "GUI" ) << "Timed output enabled";
    }
    return true;
}

void WQt4Gui::moduleError( boost::shared_ptr< WModule > module, const WException& exception )
{
    QCoreApplication::postEvent( m_mainWindow, new WModuleCrashEvent( module, exception.what() ) );
}

WMainWindow* WQt4Gui::getMainWindow()
{
    return m_mainWindow;
}

int WQt4Gui::run()
{
    bool parsingSuccessful = parseOptions();

    if( !parsingSuccessful )
    {
        return 1;
    }

    // exit as fast as possible if command line asked for help. The ,essage has been printed by parseOptions().
    if( m_optionsMap.count( "help" ) )
    {
        return 1;
    }

    WLogger::getLogger()->run();
    wlog::info( "GUI" ) << "Bringing up GUI";

    QApplication appl( m_argc, m_argv, true );

    // startup graphics engine
    m_ge = WGraphicsEngine::getGraphicsEngine();

    // and startup kernel
    m_kernel = boost::shared_ptr< WKernel >( new WKernel( m_ge, shared_from_this() ) );
    m_kernel->run();
    t_ModuleErrorSignalHandlerType func = boost::bind( &WQt4Gui::moduleError, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ERROR, func );

    // create the window
    m_mainWindow = new WMainWindow();
    m_mainWindow->setupGUI();
    m_mainWindow->show();

    // connect out loader signal with kernel
#ifdef _WIN32
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSetsSynchronously, m_kernel, _1 ) );
#else
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSets, m_kernel, _1 ) );
#endif

    WCondition::t_ConditionNotifierType newDatasetSignal = boost::bind( &WQt4Gui::slotUpdateTextureSorter, this );
    WDataHandler::getDefaultSubject()->getListChangeCondition()->subscribeSignal( newDatasetSignal );

    // bind the GUI's slot with the ready signal

    // Assoc Event
    t_ModuleGenericSignalHandlerType assocSignal = boost::bind( &WQt4Gui::slotAddDatasetOrModuleToBrowser, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ASSOCIATED, assocSignal );

    // Ready Event
    t_ModuleGenericSignalHandlerType readySignal = boost::bind( &WQt4Gui::slotActivateDatasetOrModuleInBrowser, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_READY, readySignal );

    // Remove Event
    t_ModuleGenericSignalHandlerType removedSignal = boost::bind( &WQt4Gui::slotRemoveDatasetOrModuleInBrowser, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_REMOVED, removedSignal );

    // Connect Event
    t_GenericSignalHandlerType connectionEstablishedSignal = boost::bind( &WQt4Gui::slotConnectionEstablished, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_ESTABLISHED, connectionEstablishedSignal );

    // Disconnect Event
    t_GenericSignalHandlerType connectionClosedSignal = boost::bind( &WQt4Gui::slotConnectionClosed, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_CLOSED, connectionClosedSignal );

    boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > assocRoiSignal =
            boost::bind( &WQt4Gui::slotAddRoiToBrowser, this, _1 );
    m_kernel->getRoiManager()->addAddNotifier( assocRoiSignal );
    boost::function< void( boost::shared_ptr< WRMROIRepresentation > ) > removeRoiSignal =
            boost::bind( &WQt4Gui::slotRemoveRoiFromBrowser, this, _1 );
    m_kernel->getRoiManager()->addRemoveNotifier( removeRoiSignal );

    // now we are initialized
    m_isInitialized( true );

    // check if we want to load data due to command line and call the respective function
    if( m_optionsMap.count("input") )
    {
        //
        // WE KNOW THAT THIS IS KIND OF A HACK. Iis is only provided to prevent naive users from having trouble.
        //
        bool allowOnlyOneFiberDataSet = false;
        bool doubleFibersFound = false; // have we detected the multiple loading of fibers?
        if( WPreferences::getPreference( "general.allowOnlyOneFiberDataSet", &allowOnlyOneFiberDataSet ) && allowOnlyOneFiberDataSet )
        {
            bool fibFound = false;
            std::vector< std::string > tmpFiles = m_optionsMap["input"].as< std::vector< std::string > >();
            for( std::vector< std::string >::iterator it = tmpFiles.begin(); it != tmpFiles.end(); ++it )
            {
                using wiotools::getSuffix;
                std::string suffix = getSuffix( *it );
                bool isFib = ( suffix == ".fib" );
                if( fibFound && isFib )
                {
                    QCoreApplication::postEvent( m_mainWindow, new WModuleCrashEvent(
                                                     WModuleFactory::getModuleFactory()->getPrototypeByName( "Data Module" ),
                                                     std::string( "Tried to load two fiber data sets. This is not allowed by your preferences." ) ) );
                    doubleFibersFound = true;
                }
                fibFound |= isFib;
            }
            if( fibFound && !doubleFibersFound )
            {
                // Found exactly one fiber data set. So signal this to main window.
                // If more than one are found we do not load them anyways. Thus we can allow to load a new one.
                m_mainWindow->setFibersLoaded();
            }
        }

        if( !doubleFibersFound )
        {
            m_kernel->loadDataSets( m_optionsMap["input"].as< std::vector< std::string > >() );
        }
    }

    // Load project file
    if( m_optionsMap.count("project") )
    {
        try
        {
            boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                    new WProjectFile( m_optionsMap["project"].as< std::string >() )
            );

            // This call is asynchronous. It parses the file and the starts a thread to actually do all the stuff
            proj->load();
        }
        catch( const WException& e )
        {
            wlog::error( "GUI" ) << "Project file \"" << m_optionsMap["project"].as< std::string >() << "\" could not be loaded. Message: " <<
                e.what();
        }
    }

    // run
    int qtRetCode = appl.exec();

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );
    WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    return qtRetCode;
}

void WQt4Gui::slotUpdateTextureSorter()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WUpdateTextureSorterEvent() );
}

void WQt4Gui::slotAddDatasetOrModuleToBrowser( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleAssocEvent( module ) );
}

void WQt4Gui::slotAddRoiToBrowser( boost::shared_ptr< WRMROIRepresentation > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WRoiAssocEvent( roi ) );
}

void WQt4Gui::slotRemoveRoiFromBrowser( boost::shared_ptr< WRMROIRepresentation > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WRoiRemoveEvent( roi ) );
}

void WQt4Gui::slotActivateDatasetOrModuleInBrowser( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleReadyEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow, new WModuleReadyEvent( module ) );
}

void WQt4Gui::slotRemoveDatasetOrModuleInBrowser( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleRemovedEvent( module ) );
}

void WQt4Gui::slotConnectionEstablished( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if ( in->isInputConnector() )
    {
        QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleConnectEvent( in, out ) );
    }
    else
    {
        QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleConnectEvent( out, in ) );
    }
}

void WQt4Gui::slotConnectionClosed( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if ( in->isInputConnector() )
    {
        QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleDisconnectEvent( in, out ) );
    }
    else
    {
        QCoreApplication::postEvent( m_mainWindow->getDatasetBrowser(), new WModuleDisconnectEvent( out, in ) );
    }
}

boost::shared_ptr< WModule > WQt4Gui::getSelectedModule()
{
    return m_mainWindow->getDatasetBrowser()->getSelectedModule();
}

boost::signals2::signal1< void, std::vector< std::string > >* WQt4Gui::getLoadButtonSignal()
{
    return m_mainWindow->getLoaderSignal();
}

boost::shared_ptr< WCustomWidget > WQt4Gui::openCustomWidget( std::string title, WGECamera::ProjectionMode projectionMode,
    boost::shared_ptr< WCondition > shutdownCondition )
{
    WConditionSet conditionSet;
    conditionSet.setResetable( true, false );
    conditionSet.add( shutdownCondition );

    boost::shared_ptr< WFlag< boost::shared_ptr< WCustomWidget > > > widgetFlag(
        new WFlag< boost::shared_ptr< WCustomWidget > >( new WConditionOneShot, boost::shared_ptr< WCustomWidget >() ) );
    conditionSet.add( widgetFlag->getCondition() );

    QCoreApplication::postEvent( m_mainWindow, new WOpenCustomDockWidgetEvent( title, projectionMode, widgetFlag ) );

    conditionSet.wait();
    return widgetFlag->get();
}

void WQt4Gui::closeCustomWidget( std::string title )
{
    m_mainWindow->closeCustomDockWidget( title );
}
