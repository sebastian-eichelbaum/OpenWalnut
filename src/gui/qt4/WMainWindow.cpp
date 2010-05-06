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
#include <string>
#include <vector>
#include <map>

#include <boost/thread.hpp>

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QSlider>
#include <QtGui/QShortcut>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMenuBar>
#include <QtGui/QMenu>

#include "WMainWindow.h"
#include "WOpenCustomDockWidgetEvent.h"
#include "WQtGLWidget.h"
#include "WQtNavGLWidget.h"
#include "WQtCustomDockWidget.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleCrashEvent.h"
#include "events/WEventTypes.h"
#include "datasetbrowser/WPropertyBoolWidget.h"
#include "../../common/WColor.h"
#include "../../common/WPreferences.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WProjectFile.h"
#include "../../modules/data/WMData.h"
#include "../../modules/navSlices/WMNavSlices.h"

#include "../../dataHandler/WEEG2.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetFibers.h"

#include "../../graphicsEngine/WROIBox.h"

#include "../icons/WIcons.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_iconManager(),
    m_fibLoaded( false )
{
}

void WMainWindow::setupGUI()
{
    m_iconManager.addIcon( std::string( "load" ), fileopen_xpm );
    m_iconManager.addIcon( std::string( "loadProject" ), projOpen_xpm );
    m_iconManager.addIcon( std::string( "saveProject" ), projSave_xpm );
    m_iconManager.addIcon( std::string( "logo" ), logoIcon_xpm );
    m_iconManager.addIcon( std::string( "help" ), question_xpm );
    m_iconManager.addIcon( std::string( "quit" ), quit_xpm );
    m_iconManager.addIcon( std::string( "moduleBusy" ), moduleBusy_xpm );
    m_iconManager.addIcon( std::string( "moduleCrashed" ), moduleCrashed_xpm );
    m_iconManager.addIcon( std::string( "remove" ), remove_xpm );

    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    resize( 946, 632 );
    setWindowIcon( m_iconManager.getIcon( "logo" ) );
    setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut (development version)", 0, QApplication::UnicodeUTF8 ) );

    m_menuBar = new QMenuBar( this );
    QMenu* fileMenu = m_menuBar->addMenu( "File" );
    fileMenu->addAction( m_iconManager.getIcon( "load" ), "Load", this, SLOT( openLoadDialog() ), QKeySequence( "Ctrl+L" ) );
    fileMenu->addAction( "Config", this, SLOT( openConfigDialog() ), QKeySequence( "Ctrl+C" ) );
    fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ), QKeySequence( "Ctrl+Q" ) );

    QMenu* helpMenu = m_menuBar->addMenu( "Help" );
    helpMenu->addAction( m_iconManager.getIcon( "help" ), "About OpenWalnut", this, SLOT( openAboutDialog() ), QKeySequence( "F1" ) );
    setMenuBar( m_menuBar );

    m_centralwidget = new QWidget( this );
    m_centralwidget->setObjectName( QString::fromUtf8( "centralwidget" ) );
    setCentralWidget( m_centralwidget );

    m_mainGLWidget = boost::shared_ptr< WQtGLWidget >( new WQtGLWidget( "main", this, WGECamera::ORTHOGRAPHIC ) );
    m_mainGLWidget->initialize();
    setCentralWidget( m_mainGLWidget.get() );

    // initially 3 navigation views
    {
        bool hideWidget;
        if( !( WPreferences::getPreference( "qt4gui.hideAxial", &hideWidget ) && hideWidget) )
        {
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "axial", this, "Axial Slice" ) );
            m_navAxial->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideCoronal", &hideWidget ) && hideWidget) )
        {
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "coronal", this, "Coronal Slice" ) );
            m_navCoronal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideSagittal", &hideWidget ) && hideWidget) )
        {
            m_navSagittal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "sagittal", this, "Sagittal Slice" ) );
            m_navSagittal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );
        }
    }

    // we do not need the dummy widget if there are no other widgets.
    if( m_navAxial || m_navCoronal || m_navSagittal )
    {
        m_dummyWidget = new QDockWidget( this );
        m_dummyWidget->setFeatures( QDockWidget::NoDockWidgetFeatures );
        m_dummyWidget->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Ignored );
        addDockWidget( Qt::LeftDockWidgetArea, m_dummyWidget );
    }

    // Default background color from config file
    {
        WColor bgColor;
        double r;
        double g;
        double b;
        if( WPreferences::getPreference( "ge.bgColor.r", &r )
            && WPreferences::getPreference( "ge.bgColor.g", &g )
            && WPreferences::getPreference( "ge.bgColor.b", &b ) )
        {
            bgColor.setRGB( r, g, b );
            m_mainGLWidget->setBgColor( bgColor );

            if ( m_navAxial )
            {
                m_navAxial->getGLWidget()->setBgColor( bgColor );
            }
            if ( m_navCoronal )
            {
                m_navCoronal->getGLWidget()->setBgColor( bgColor );
            }
            if ( m_navSagittal )
            {
                m_navSagittal->getGLWidget()->setBgColor( bgColor );
            }
        }
    }

    setupPermanentToolBar();

    setupCompatiblesToolBar();

    m_datasetBrowser = new WQtDatasetBrowser( this );
    m_datasetBrowser->setFeatures( QDockWidget::AllDockWidgetFeatures );
    addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );
    m_datasetBrowser->addSubject( "Default Subject" );
}

void WMainWindow::setupPermanentToolBar()
{
    m_permanentToolBar = new WQtToolBar( "Permanent Toolbar", this );

    m_iconManager.addIcon( std::string( "ROI" ), box_xpm );
    m_iconManager.addIcon( std::string( "axial" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal" ), sag_xpm );


    WQtPushButton* loadButton = new WQtPushButton( m_iconManager.getIcon( "load" ), "load", m_permanentToolBar );
    WQtPushButton* roiButton = new WQtPushButton( m_iconManager.getIcon( "ROI" ), "ROI", m_permanentToolBar );
    WQtPushButton* projectLoadButton = new WQtPushButton( m_iconManager.getIcon( "loadProject" ), "loadProject", m_permanentToolBar );
    WQtPushButton* projectSaveButton = new WQtPushButton( m_iconManager.getIcon( "saveProject" ), "saveProject", m_permanentToolBar );

    // setup save button
    QMenu* saveMenu = new QMenu( "Save Project", projectSaveButton );
    saveMenu->addAction( "Save Project", this, SLOT( projectSaveAll() ) );
    saveMenu->addAction( "Save Modules", this, SLOT( projectSaveModuleOnly() ) );
    saveMenu->addAction( "Save Camera", this, SLOT( projectSaveCameraOnly() ) );
    saveMenu->addAction( "Save ROIs", this, SLOT( projectSaveROIOnly() ) );
    projectSaveButton->setPopupMode( QToolButton::MenuButtonPopup );
    projectSaveButton->setMenu( saveMenu );

    connect( loadButton, SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );
    connect( roiButton, SIGNAL( pressed() ), this, SLOT( newRoi() ) );
    connect( projectLoadButton, SIGNAL( pressed() ), this, SLOT( projectLoad() ) );
    connect( projectSaveButton, SIGNAL( pressed() ), this, SLOT( projectSaveAll() ) );

    loadButton->setToolTip( "Load Data" );
    roiButton->setToolTip( "Create New ROI" );
    projectLoadButton->setToolTip( "Load a project from file" );
    projectSaveButton->setToolTip( "Save current project to file" );

    m_permanentToolBar->addWidget( loadButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addWidget( projectLoadButton );
    m_permanentToolBar->addWidget( projectSaveButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addWidget( roiButton );

    m_permanentToolBar->addSeparator();

    addToolBar( Qt::TopToolBarArea, m_permanentToolBar );
}

void WMainWindow::autoAdd( boost::shared_ptr< WModule > module, std::string proto )
{
    // get the prototype.
    if ( !WKernel::getRunningKernel()->getRootContainer()->applyModule( module, proto, true ) )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active but module " + proto + " could not be added.",
                                             "GUI", LL_ERROR );
    }
}

void WMainWindow::moduleSpecificSetup( boost::shared_ptr< WModule > module )
{
    // Add all special handlings here. This method is called whenever a module is marked "ready". You can set up the gui for special modules,
    // load certain modules for datasets and so on.

    // The Data Modules also play an special role. To have modules being activated when certain data got loaded, we need to hook it up here.
    bool useAutoDisplay = true;
    WPreferences::getPreference( "qt4gui.useAutoDisplay", &useAutoDisplay );
    if ( useAutoDisplay && module->getType() == MODULE_DATA )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active and Data module added. The proper module will be added.",
                                             "GUI", LL_DEBUG );

        // data modules contain an member denoting the real data type. Currently we only have one data module and a not very modulated data
        // structures.
        boost::shared_ptr< WMData > dataModule = boost::shared_static_cast< WMData >( module );

        // grab data and identify type
        if ( dataModule->getDataSet()->isA< WDataSetSingle >() )
        {
            // it is a dataset single
            // load a nav slice module if a WDataSetSingle is available!?

            // if it not already is running: add it
            if ( !WMNavSlices::isRunning() )
            {
                autoAdd( module, "Navigation Slices" );
            }
        }
        else if ( dataModule->getDataSet()->isA< WDataSetFibers >() )
        {
            // it is a fiber dataset -> add the FiberDisplay module

            // if it not already is running: add it
            if ( !WMFiberDisplay::isRunning() )
            {
                autoAdd( module, "Fiber Display" );
            }
        }
        else if ( dataModule->getDataSet()->isA< WEEG2 >() )
        {
            // it is a eeg dataset -> add the eegView module
            autoAdd( module, "EEG View" );
        }
    }

    // nav slices use separate buttons for slice on/off switching
    if ( module->getName() == "Navigation Slices" )
    {
        boost::shared_ptr< WPropertyBase > prop = module->getProperties()->findProperty( "showAxial" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showAxial\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WPropertyBoolWidget* button = new WPropertyBoolWidget( prop->toPropBool(), NULL, m_permanentToolBar, true );
            button->setToolTip( "Toggle Axial Slice" );
            button->getButton()->setMaximumSize( 24, 24 );
            button->getButton()->setIcon( m_iconManager.getIcon( "axial" ) );
            m_permanentToolBar->addWidget( button );
        }

        prop = module->getProperties()->findProperty( "showCoronal" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showCoronal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WPropertyBoolWidget* button = new WPropertyBoolWidget( prop->toPropBool(), NULL, m_permanentToolBar, true );
            button->setToolTip( "Toggle Coronal Slice" );
            button->getButton()->setMaximumSize( 24, 24 );
            button->getButton()->setIcon( m_iconManager.getIcon( "coronal" ) );
            m_permanentToolBar->addWidget( button );
        }

        prop = module->getProperties()->findProperty( "showSagittal" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"showSagittal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WPropertyBoolWidget* button = new WPropertyBoolWidget( prop->toPropBool(), NULL, m_permanentToolBar, true );
            button->setToolTip( "Toggle Sagittal Slice" );
            button->getButton()->setMaximumSize( 24, 24 );
            button->getButton()->setIcon( m_iconManager.getIcon( "sagittal" ) );
            m_permanentToolBar->addWidget( button );
        }

        // now setup the nav widget sliders
        prop = module->getProperties()->findProperty( "Axial Slice" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Axial Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navAxial )
            {
                m_navAxial->setSliderProperty( prop->toPropInt() );
            }
        }

        prop = module->getProperties()->findProperty( "Coronal Slice" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Coronal Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navCoronal )
            {
                m_navCoronal->setSliderProperty( prop->toPropInt() );
            }
        }

        prop = module->getProperties()->findProperty( "Sagittal Slice" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Sagittal Slice\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navSagittal )
            {
               m_navSagittal->setSliderProperty( prop->toPropInt() );
            }
        }
    }
}

void WMainWindow::setupCompatiblesToolBar()
{
    m_iconManager.addIcon( std::string( "o" ), o_xpm ); // duumy icon for modules

    m_compatiblesToolBar = new WQtToolBar( "Compatible Modules Toolbar", this );

    // optional toolbar break
    {
        bool useToolBarBreak = true;
        WPreferences::getPreference( "qt4gui.useToolBarBreak", &useToolBarBreak );
        if( useToolBarBreak )
        {
            // Blank toolbar for nicer layout in case of toolbar break
            // This can be done nicer very probably.
            WQtToolBar* blankToolBar = new WQtToolBar( "Blank Toolbar", this );
            addToolBar( Qt::TopToolBarArea, blankToolBar );
            addToolBarBreak( Qt::TopToolBarArea );
        }
    }

    addToolBar( Qt::TopToolBarArea, m_compatiblesToolBar );
}

WQtDatasetBrowser* WMainWindow::getDatasetBrowser()
{
    return m_datasetBrowser;
}

WQtToolBar* WMainWindow::getCompatiblesToolBar()
{
    return m_compatiblesToolBar;
}

void WMainWindow::projectSave( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer )
{
    QFileDialog fd;
    fd.setWindowTitle( "Save Project as" );
    fd.setFileMode( QFileDialog::AnyFile );
    fd.setAcceptMode( QFileDialog::AcceptSave );

    QStringList filters;
    filters << "Project File (*.owproj)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if ( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for ( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                new WProjectFile( ( *constIterator ).toStdString() )
        );

        try
        {
            // This call is synchronous.
            if ( writer.empty() )
            {
                proj->save();
            }
            else
            {
                proj->save( writer );
            }
        }
        catch( const std::exception& e )
        {
            QString title = "Problem while saving project file.";
            QString message = "<b>Problem while saving project file.</b><br/><br/><b>File:  </b>" + ( *constIterator ) +
                              "<br/><b>Message:  </b>" + QString::fromStdString( e.what() );
            QMessageBox::critical( this, title, message );
        }
    }
}

void WMainWindow::projectSaveAll()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    // an empty list equals "all"
    projectSave( w );
}

void WMainWindow::projectSaveCameraOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getCameraWriter() );
    projectSave( w );
}

void WMainWindow::projectSaveROIOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getROIWriter() );
    projectSave( w );
}

void WMainWindow::projectSaveModuleOnly()
{
    std::vector< boost::shared_ptr< WProjectFileIO > > w;
    w.push_back( WProjectFile::getModuleWriter() );
    projectSave( w );
}

void WMainWindow::projectLoad()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Simple Project File (*.owproj)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if ( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for ( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                new WProjectFile( ( *constIterator ).toStdString() )
        );

        // This call is asynchronous. It parses the file and the starts a thread to actually do all the stuff
        proj->load();
    }
}

void WMainWindow::openLoadDialog()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Known file types (*.cnt *.edf *.asc *.nii *.nii.gz *.fib)"
            << "EEG files (*.cnt *.edf *.asc)"
            << "NIfTI (*.nii *.nii.gz)"
            << "Fibers (*.fib)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if ( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    std::vector< std::string > stdFileNames;

    QStringList::const_iterator constIterator;
    for ( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        stdFileNames.push_back( ( *constIterator ).toLocal8Bit().constData() );
    }

    //
    // WE KNOW THAT THIS IS KIND OF A HACK. Iis is only provided to prevent naive users from having trouble.
    //
    bool allowOnlyOneFiberDataSet = false;
    bool doubleFibersFound = false; // have we detected the multiple loading of fibers?
    if( WPreferences::getPreference( "general.allowOnlyOneFiberDataSet", &allowOnlyOneFiberDataSet ) && allowOnlyOneFiberDataSet )
    {
        for( std::vector< std::string >::iterator it = stdFileNames.begin(); it != stdFileNames.end(); ++it )
        {
            using wiotools::getSuffix;
            std::string suffix = getSuffix( *it );
            bool isFib = ( suffix == ".fib" );
            if( m_fibLoaded && isFib )
            {
                QCoreApplication::postEvent( this, new WModuleCrashEvent(
                                                 WModuleFactory::getModuleFactory()->getPrototypeByName( "Data Module" ),
                                                 std::string( "Tried to load two fiber data sets. This is not allowed by your preferences." ) ) );
                doubleFibersFound = true;
            }
            m_fibLoaded |= isFib;
        }
    }

    if( !doubleFibersFound )
    {
        m_loaderSignal( stdFileNames );
    }
}

void WMainWindow::openAboutDialog()
{
    QMessageBox::about( this, "About OpenWalnut",
                        "OpenWalnut ( http://www.openwalnut.org )\n\n"
                        "Copyright (C) 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS. "
                        "For more information see http://www.openwalnut.org/copying.\n\n"
                        "This program comes with ABSOLUTELY NO WARRANTY. "
                        "This is free software, and you are welcome to redistribute it "
                        "under the terms of the GNU Lesser General Public License. "
                        "You should have received a copy of the GNU Lesser General Public License "
                        "along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.\n"
                        "\n"
                        "Thank you for using OpenWalnut." );
}

boost::signals2::signal1< void, std::vector< std::string > >* WMainWindow::getLoaderSignal()
{
    return &m_loaderSignal;
}

WIconManager*  WMainWindow::getIconManager()
{
    return &m_iconManager;
}

void WMainWindow::closeEvent( QCloseEvent* e )
{
    // use some "Really Close?" Dialog here
    bool reallyClose = true;


    // handle close event
    if ( reallyClose )
    {
        // signal everybody to shut down properly.
        WKernel::getRunningKernel()->finalize();

        // now nobody acesses the osg anymore
        // clean up gl widgets
        m_mainGLWidget->close();
        if( m_navAxial )
        {
            m_navAxial->close();
        }
        if( m_navCoronal )
        {
            m_navCoronal->close();
        }
        if( m_navSagittal )
        {
            m_navSagittal->close();
        }

        // delete CustomDockWidgets
        boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
        for( std::map< std::string, boost::shared_ptr< WQtCustomDockWidget > >::iterator it = m_customDockWidgets.begin();
             it != m_customDockWidgets.end(); ++it )
        {
            it->second->close();
        }
        //m_customDockWidgetsLock.unlock();

        // finally close
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

void WMainWindow::customEvent( QEvent* event )
{
    if( event->type() == WOpenCustomDockWidgetEvent::CUSTOM_TYPE )
    {
        // OpenCustomDockWidgetEvent
        WOpenCustomDockWidgetEvent* ocdwEvent = static_cast< WOpenCustomDockWidgetEvent* >( event );
        std::string title = ocdwEvent->getTitle();

        boost::shared_ptr< WQtCustomDockWidget > widget;

        boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
        if( m_customDockWidgets.count( title ) == 0 )
        {
            // create new custom dock widget
            widget = boost::shared_ptr< WQtCustomDockWidget >(
                new WQtCustomDockWidget( title, this, ocdwEvent->getProjectionMode() ) );
            addDockWidget( Qt::BottomDockWidgetArea, widget.get() );

            // store it in CustomDockWidget list
            m_customDockWidgets.insert( make_pair( title, widget ) );
        }
        else
        {
            widget = m_customDockWidgets[title];
            widget->increaseUseCount();
        }
        //m_customDockWidgetsLock.unlock();

        ocdwEvent->getFlag()->set( widget );
    }
    else
    {
        // other event
        QMainWindow::customEvent( event );
    }
}

bool WMainWindow::event( QEvent* event )
{
    // a module got associated with the root container -> add it to the list
    if ( event->type() == WQT_READY_EVENT )
    {
        // convert event to ready event
        WModuleReadyEvent* e1 = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if ( e1 )
        {
            moduleSpecificSetup( e1->getModule() );
        }
    }

    if ( event->type() == WQT_CRASH_EVENT )
    {
        // convert event to ready event
        WModuleCrashEvent* e1 = dynamic_cast< WModuleCrashEvent* >( event );     // NOLINT
        if ( e1 )
        {
            QString title = "Problem in module: " + QString::fromStdString( e1->getModule()->getName() );
            QString message = "<b>Module Problem</b><br/><br/><b>Module:  </b>" + QString::fromStdString( e1->getModule()->getName() ) +
                              "<br/><b>Message:  </b>" + QString::fromStdString( e1->getMessage() );
            QMessageBox::critical( this, title, message );
        }
    }

    return QMainWindow::event( event );
}

boost::shared_ptr< WQtCustomDockWidget > WMainWindow::getCustomDockWidget( std::string title )
{
    boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
    boost::shared_ptr< WQtCustomDockWidget > out = m_customDockWidgets.count( title ) > 0 ?
        m_customDockWidgets[title] :
        boost::shared_ptr< WQtCustomDockWidget >();
    //m_customDockWidgetsLock.unlock();
    return out;
}


void WMainWindow::closeCustomDockWidget( std::string title )
{
    boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
    if( m_customDockWidgets.count( title ) > 0 )
    {
        if( m_customDockWidgets[title]->decreaseUseCount() )
        {
            // custom dock widget should be deleted
            m_customDockWidgets.erase( title );
        }
    }
    //m_customDockWidgetsLock.unlock();
}

void WMainWindow::newRoi()
{
    // do nothing if we can not get
    if( !WKernel::getRunningKernel()->getRoiManager()->getBitField() )
    {
        wlog::warn( "WMainWindow" ) << "Refused to add ROI, as ROIManager does not have computed its bitfield yet.";
        return;
    }

    wmath::WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    wmath::WPosition minROIPos = crossHairPos - wmath::WPosition( 10., 10., 10. );
    wmath::WPosition maxROIPos = crossHairPos + wmath::WPosition( 10., 10., 10. );

    if ( m_datasetBrowser->getFirstRoiInSelectedBranch().get() == NULL )
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
    }
    else
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, m_datasetBrowser->getFirstRoiInSelectedBranch()->getROI() );
    }
}

void WMainWindow::setFibersLoaded()
{
    m_fibLoaded = true;
}

void WMainWindow::openConfigDialog()
{
    m_configWidget = boost::shared_ptr< WQtConfigWidget >( new WQtConfigWidget );

    m_configWidget->initAndShow();
}
