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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

#include <QApplication>
#include <QSplashScreen>
#include <QFileDialog>
#include <QtCore/QDir>
#include <QtCore/QSettings>

#include "WMainWindow.h" // this has to be included before any other includes
#include "WApplication.h"
#include "core/common/WConditionOneShot.h"
#include "core/common/WIOTools.h"
#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WSubject.h"
#include "core/graphicsEngine/WGraphicsEngine.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModuleContainer.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WROIManager.h"
#include "controlPanel/WQtControlPanel.h"
#include "events/WModuleAssocEvent.h"
#include "events/WModuleConnectEvent.h"
#include "events/WModuleCrashEvent.h"
#include "events/WModuleDisconnectEvent.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleRemovedEvent.h"
#include "events/WRoiAssocEvent.h"
#include "events/WRoiRemoveEvent.h"
#include "events/WUpdateTextureSorterEvent.h"
#include "events/WLogEvent.h"
#include "WQtModuleConfig.h"

#include "WQtGui.h"

#ifdef Q_WS_X11
    #include <X11/Xlib.h>   // NOLINT - this needs to be done AFTER Qt has set some defines in their headers and after several other
                            //          headers which are included indirectly, although it is a system header.
#endif

WMainWindow* WQtGui::m_mainWindow = NULL;

QSettings* WQtGui::m_settings = NULL;

WQtGui::WQtGui( const boost::program_options::variables_map& options, int argc, char** argv )
    : WUI( argc, argv ),
    m_optionsMap( options ),
    m_loadDeferredOnce( true )
{
}

WQtGui::~WQtGui()
{
    delete m_settings;
}

void WQtGui::moduleError( boost::shared_ptr< WModule > module, const WException& exception )
{
    QCoreApplication::postEvent( m_mainWindow, new WModuleCrashEvent( module, exception.what() ) );
    QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleCrashEvent( module, exception.what() ) );
}

WMainWindow* WQtGui::getMainWindow()
{
    return m_mainWindow;
}

WIconManager* WQtGui::getIconManager()
{
    return getMainWindow()->getIconManager();
}

void WQtGui::deferredLoad()
{
    m_deferredLoadMutex.lock();
    if( m_loadDeferredOnce )
    {
        m_loadDeferredOnce = false;
        wlog::debug( "OpenWalnut" ) << "Deferred loading of data and project files.";

        bool useInputFileNameAsProject = false;
        // check if we want to load data due to command line and call the respective function
        if( m_optionsMap.count( "input" ) )
        {
            std::vector< std::string > dataFileNames = m_optionsMap["input"].as< std::vector< std::string > >();
            useInputFileNameAsProject = ( dataFileNames.size() == 1
                                          && ( boost::algorithm::ends_with( dataFileNames[0], ".owp" )
                                               || boost::algorithm::ends_with( dataFileNames[0], ".owproj" ) ) );
            if( !useInputFileNameAsProject )
            {
                m_kernel->loadDataSets( dataFileNames );
            }
        }

        // Load project file
        if( m_optionsMap.count( "project" ) || useInputFileNameAsProject )
        {
            std::string projectFileName;
            if( useInputFileNameAsProject )
            {
                projectFileName = m_optionsMap["input"].as< std::vector< std::string > >()[0];
            }
            else
            {
                projectFileName = m_optionsMap["project"].as< std::string >();
            }

            try
            {
                // This call is asynchronous. It parses the file and the starts a thread to actually do all the stuff
                m_mainWindow->asyncProjectLoad( projectFileName );
            }
            catch( const WException& e )
            {
                wlog::error( "GUI" ) << "Project file \"" << m_optionsMap["project"].as< std::string >() << "\" could not be loaded. Message: " <<
                    e.what();
            }
        }
    }
    m_deferredLoadMutex.unlock();
}

int WQtGui::run()
{
#ifdef Q_WS_X11
    XInitThreads();
#endif

    m_splash = NULL;
    // init logger
    m_loggerConnection = WLogger::getLogger()->subscribeSignal( WLogger::AddLog, boost::bind( &WQtGui::slotAddLog, this, _1 ) );

    // make qapp instance before using the applicationDirPath() function
#ifdef Q_WS_MAC
    //TODO(mario): this should run on all platforms but crashes at least on Linux right now. Therefore, I only use it on OSX
    WApplication appl( m_argc, m_argv, true );
#else
    // TODO(mario): I want a WApplication here for session handling but that code crashes
    QApplication appl( m_argc, m_argv, true );
#endif
    
    // the call path of the application, this uses QApplication which needs to be instantiated.
    boost::filesystem::path walnutBin = boost::filesystem::path( QApplication::applicationDirPath().toStdString() );
    
    // setup path helper which provides several paths to others
#ifdef Q_WS_MAC
    // apple has a special file hierarchy in so-called bundles
    // this code determines whether we are started from a bundle context
    // and sets the paths according to Apple's guidelines inside the bundle
    if( QApplication::applicationDirPath().endsWith( "/MacOS" ) )
    {
        // we are in a bundle
        // TODO(mario): apply default OSX behavior of using $HOME/Library/OpenWalnut ?
        WPathHelper::getPathHelper()->setBasePathsOSXBundle( walnutBin, boost::filesystem::path( QDir::homePath().toStdString() ) / ".OpenWalnut" );
    }
    else
    {
        // assume standard behavior
        WPathHelper::getPathHelper()->setBasePaths( walnutBin, boost::filesystem::path( QDir::homePath().toStdString() ) / ".OpenWalnut" );
    }
#else
    // on all other platforms, get the home directory form Qt and the path from the application binary location
    WPathHelper::getPathHelper()->setBasePaths( walnutBin, boost::filesystem::path( QDir::homePath().toStdString() ) / ".OpenWalnut" );
#endif

    QPixmap splashPixmap( QString::fromStdString( ( WPathHelper::getPathHelper()->getSharePath() / "qt5gui" / "splash.png" ).string() ) );
    m_splash = new QSplashScreen( splashPixmap );
    m_splash->show();

    // with the correct paths, we can load the settings
    m_settings = new QSettings( QString::fromStdString( ( WPathHelper::getHomePath() / "config.qt5gui" ).string() ), QSettings::IniFormat );

    WQtModuleConfig::initPathHelper();

    // get the minimum log level from preferences
    LogLevel logLevel = static_cast< LogLevel >( WQtGui::getSettings().value( "qt5gui/logLevel", LL_DEBUG ).toInt() );
    WLogger::getLogger()->setDefaultLogLevel( logLevel );

    // print the first output
    wlog::debug( "OpenWalnut" ) << "OpenWalnut binary path: " << walnutBin;
    wlog::info( "GUI" ) << "Bringing up GUI";

    // startup graphics engine
    m_ge = WGraphicsEngine::getGraphicsEngine();

    // and startup kernel
    m_kernel = boost::shared_ptr< WKernel >( WKernel::instance( m_ge, shared_from_this() ) );
    m_kernel->run();
    m_kernel->subscribeSignal( WKernel::KERNEL_STARTUPCOMPLETE, boost::bind( &WQtGui::deferredLoad, this ) );

    t_ModuleErrorSignalHandlerType func = boost::bind( &WQtGui::moduleError, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ERROR, func );

    // bind the GUI's slot with the signals provided by the kernel
    WCondition::t_ConditionNotifierType newDatasetSignal = boost::bind( &WQtGui::slotUpdateTextureSorter, this );
    WDataHandler::getDefaultSubject()->getListChangeCondition()->subscribeSignal( newDatasetSignal );

    // Assoc Event
    t_ModuleGenericSignalHandlerType assocSignal = boost::bind( &WQtGui::slotAddDatasetOrModuleToTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_ASSOCIATED, assocSignal );

    // Ready Event
    t_ModuleGenericSignalHandlerType readySignal = boost::bind( &WQtGui::slotActivateDatasetOrModuleInTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_READY, readySignal );

    // Remove Event
    t_ModuleGenericSignalHandlerType removedSignal = boost::bind( &WQtGui::slotRemoveDatasetOrModuleInTree, this, _1 );
    m_kernel->getRootContainer()->addDefaultNotifier( WM_REMOVED, removedSignal );

    // Connect Event
    t_GenericSignalHandlerType connectionEstablishedSignal = boost::bind( &WQtGui::slotConnectionEstablished, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_ESTABLISHED, connectionEstablishedSignal );

    // Disconnect Event
    t_GenericSignalHandlerType connectionClosedSignal = boost::bind( &WQtGui::slotConnectionClosed, this, _1, _2 );
    m_kernel->getRootContainer()->addDefaultNotifier( CONNECTION_CLOSED, connectionClosedSignal );

    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > assocRoiSignal;
    assocRoiSignal =
        boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
            new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WQtGui::slotAddRoiToTree, this, _1 ) ) );
    m_kernel->getRoiManager()->addAddNotifier( assocRoiSignal );

    boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > > removeRoiSignal;
    removeRoiSignal =
        boost::shared_ptr< boost::function< void( osg::ref_ptr< WROI > ) > >(
            new boost::function< void( osg::ref_ptr< WROI > ) > ( boost::bind( &WQtGui::slotRemoveRoiFromTree, this, _1 ) ) );
    m_kernel->getRoiManager()->addRemoveNotifier( removeRoiSignal );

    // create the window
    m_mainWindow = new WMainWindow( m_splash );

    m_widgetFactory = WUIQtWidgetFactory::SPtr( new WUIQtWidgetFactory( m_mainWindow ) );

#ifdef Q_WS_MAC
    //TODO(mario): this should run on all platforms but crashes at least on Linux right now. Therefore, I only use it on OSX
    appl.setMyMainWidget( m_mainWindow );
#endif
    m_mainWindow->setupGUI();
    m_mainWindow->show();

    // connect loader signal with kernel
#ifdef _WIN32
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSetsSynchronously, m_kernel, _1, false ) );
#else
    getLoadButtonSignal()->connect( boost::bind( &WKernel::loadDataSets, m_kernel, _1, false ) );
#endif

    // now we are initialized
    m_isInitialized( true );

     // run
    int qtRetCode = appl.exec();

    delete m_mainWindow;
    m_mainWindow = NULL; // the log slot needs this to be null now

    // signal everybody to shut down properly.
    WKernel::getRunningKernel()->wait( true );
    WKernel::getRunningKernel()->getGraphicsEngine()->wait( true );

    m_loggerConnection.disconnect();

    return qtRetCode;
}

void WQtGui::slotUpdateTextureSorter()
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WUpdateTextureSorterEvent() );
}

void WQtGui::slotAddLog( const WLogEntry& entry )
{
    // emit event but the main window might not be available. Check this.
    // NOTE: we disable debug log messages completely, since their extensive use in some modules causes SEVERE slowdown of the GUI as millions of
    //       events need to be handled each GUI event loop. See issue #283 for details.
    if( m_mainWindow && ( entry.getLogLevel() != LL_DEBUG ) )
    {
        QCoreApplication::postEvent( m_mainWindow, new WLogEvent( entry ) );
    }
}

void WQtGui::slotAddDatasetOrModuleToTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    if( m_mainWindow->getNetworkEditor() )
    {
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleAssocEvent( module ) );
    }
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleAssocEvent( module ) );
}

void WQtGui::slotAddRoiToTree( osg::ref_ptr< WROI > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WRoiAssocEvent( roi ) );
}

void WQtGui::slotRemoveRoiFromTree( osg::ref_ptr< WROI > roi )
{
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WRoiRemoveEvent( roi ) );
}

void WQtGui::slotActivateDatasetOrModuleInTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    if( m_mainWindow->getNetworkEditor() )
    {
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleReadyEvent( module ) );
    }
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleReadyEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow, new WModuleReadyEvent( module ) );
}

void WQtGui::slotRemoveDatasetOrModuleInTree( boost::shared_ptr< WModule > module )
{
    // create a new event for this and insert it into event queue
    QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleRemovedEvent( module ) );
    QCoreApplication::postEvent( m_mainWindow, new WModuleRemovedEvent( module ) );
    if( m_mainWindow->getNetworkEditor() )
    {
        QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleRemovedEvent( module ) );
    }
}

void WQtGui::slotConnectionEstablished( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if( in->isInputConnector() )
    {
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleConnectEvent( in, out ) );
        if( m_mainWindow->getNetworkEditor() )
        {
            QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleConnectEvent( in, out ) );
        }
    }
    else
    {
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleConnectEvent( out, in ) );
        if( m_mainWindow->getNetworkEditor() )
        {
            QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleConnectEvent( out, in ) );
        }
    }
}

void WQtGui::slotConnectionClosed( boost::shared_ptr<WModuleConnector> in, boost::shared_ptr<WModuleConnector> out )
{
    // create a new event for this and insert it into event queue
    if( in->isInputConnector() )
    {
        if( m_mainWindow->getNetworkEditor() )
        {
            QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleDisconnectEvent( in, out ) );
        }
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleDisconnectEvent( in, out ) );
    }
    else
    {
        if( m_mainWindow->getNetworkEditor() )
        {
            QCoreApplication::postEvent( m_mainWindow->getNetworkEditor(), new WModuleDisconnectEvent( out, in ) );
        }
        QCoreApplication::postEvent( m_mainWindow->getControlPanel(), new WModuleDisconnectEvent( out, in ) );
    }
}

boost::shared_ptr< WModule > WQtGui::getSelectedModule()
{
    return m_mainWindow->getControlPanel()->getSelectedModule();
}

boost::signals2::signal1< void, std::vector< std::string > >* WQtGui::getLoadButtonSignal()
{
    return m_mainWindow->getLoaderSignal();
}

QSettings& WQtGui::getSettings()
{
    return *m_settings;
}

const boost::program_options::variables_map& WQtGui::getOptionMap() const
{
    return m_optionsMap;
}

WUIWidgetFactory::SPtr WQtGui::getWidgetFactory() const
{
    return m_widgetFactory;
}

void WQtGui::execInGUIThread( boost::function< void( void ) > functor, WCondition::SPtr notify )
{
    if( !notify )
    {
        // the user did not specify a condition. We create our own
        notify = WCondition::SPtr( new WConditionOneShot() );
    }
    WDeferredCallEvent* ev = new WDeferredCallEvent( functor, notify );
    QCoreApplication::postEvent( getMainWindow(), ev );
    notify->wait();
}

void WQtGui::execInGUIThreadAsync( boost::function< void( void ) > functor, WCondition::SPtr notify )
{
    WDeferredCallEvent* ev = new WDeferredCallEvent( functor, notify );
    QCoreApplication::postEvent( getMainWindow(), ev );
}

