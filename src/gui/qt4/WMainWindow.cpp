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
#include "events/WEventTypes.h"
#include "datasetbrowser/WPropertyBoolWidget.h"
#include "../../common/WColor.h"
#include "../../common/WPreferences.h"
#include "../../kernel/WKernel.h"
#include "../../modules/data/WMData.h"
#include "../../modules/navSlices/WMNavSlices.h"

#include "../../dataHandler/WEEG.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetFibers.h"

#include "../../graphicsEngine/WROIBox.h"

#include "../icons/WIcons.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_iconManager()
{
    setupGUI();
}

void WMainWindow::setupGUI()
{
    m_iconManager.addIcon( std::string( "load" ), fileopen_xpm );
    m_iconManager.addIcon( std::string( "logo" ), logoIcon_xpm );
    m_iconManager.addIcon( std::string( "help" ), question_xpm );
    m_iconManager.addIcon( std::string( "quit" ), quit_xpm );

    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    resize( 946, 632 );
    setWindowIcon( m_iconManager.getIcon( "logo" ) );
    setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

    m_menuBar = new QMenuBar( 0 );
    QMenu* fileMenu = m_menuBar->addMenu( "File" );
    fileMenu->addAction( m_iconManager.getIcon( "load" ), "Load", this, SLOT( openLoadDialog() ), QKeySequence( "Ctrl+L" ) );
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
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "axial", this, 160, "axialPos" ) );
            m_navAxial->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideCoronal", &hideWidget ) && hideWidget) )
        {
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "coronal", this, 200, "coronalPos" ) );
            m_navCoronal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideSagittal", &hideWidget ) && hideWidget) )
        {
            m_navSagittal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "sagittal", this, 160, "sagittalPos" ) );
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
        }
    }

    setupPermanentToolBar();

    setupCompatiblesToolBar();

    m_datasetBrowser = new WQtDatasetBrowser( this );
    m_datasetBrowser->setFeatures( QDockWidget::AllDockWidgetFeatures );
    addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );
    m_datasetBrowser->addSubject( "Default Subject" );

    // TODO(ebaum): adopt!
    //connect( m_datasetBrowser, SIGNAL( dataSetBrowserEvent( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
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

    connect( loadButton, SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );
    connect( roiButton, SIGNAL( pressed() ), this, SLOT( newRoi() ) );

    loadButton->setToolTip( "Load Data" );
    roiButton->setToolTip( "Create New ROI" );

    m_permanentToolBar->addWidget( loadButton );
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

            // if it already is running: add it
            if ( !WMNavSlices::isRunning() )
            {
                autoAdd( module, "Navigation Slice Module" );
            }
        }
        else if ( dataModule->getDataSet()->isA< WDataSetFibers >() )
        {
            // it is a fiber dataset -> add the FiberDisplay module
            autoAdd( module, "Fiber Display" );
        }
        else if ( dataModule->getDataSet()->isA< WEEG >() )
        {
            // it is a eeg dataset -> add the eegView module?
            autoAdd( module, "EEG View" );
        }
    }

    // nav slices use separate buttons for slice on/off switching
    if ( module->getName() == "Navigation Slice Module" )
    {
        boost::shared_ptr< WPropertyBase > prop = module->getProperties2()->findProperty( "showAxial" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"showAxial\", which is required by the GUI.", "GUI",
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

        prop = module->getProperties2()->findProperty( "showCoronal" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"showCoronal\", which is required by the GUI.", "GUI",
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

        prop = module->getProperties2()->findProperty( "showSagittal" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"showSagittal\", which is required by the GUI.", "GUI",
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
        prop = module->getProperties2()->findProperty( "axialPos" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"axialPos\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navAxial )
            {
                m_navAxial->setSliderProperty( prop->toPropInt() );
            }
        }

        prop = module->getProperties2()->findProperty( "coronalPos" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"coronalPos\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navCoronal )
            {
                m_navCoronal->setSliderProperty( prop->toPropInt() );
            }
        }

        prop = module->getProperties2()->findProperty( "sagittalPos" );
        if ( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slice Module does not provide the property \"sagittalPos\", which is required by the GUI.", "GUI",
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
    m_loaderSignal( stdFileNames );
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

boost::signals2::signal2< void, boost::shared_ptr< WModule >, boost::shared_ptr< WModule > >* WMainWindow::getModuleButtonSignal()
{
    return &m_moduleButtonSignal;
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

void WMainWindow::slotActivateModule( QString module )
{
    // TODO(schurade): do we really need the signal? Why can't we use the kernel directly?
    m_moduleButtonSignal( getDatasetBrowser()->getSelectedModule(), WModuleFactory::getModuleFactory()->getPrototypeByName( module.toStdString() ) );
}

void WMainWindow::newRoi()
{
    // do nothing if we can not get
    if( !WKernel::getRunningKernel()->getRoiManager()->getDataSet() )
    {
        wlog::warn( "WMainWindow" ) << "Refused to add ROI, as ROIManager does not have data set associated.";
        return;
    }

    if ( m_datasetBrowser->getSelectedRoi().get() == NULL )
    {
        boost::shared_ptr< WROIBox > newRoi = boost::shared_ptr< WROIBox >( new WROIBox( wmath::WPosition( 60., 60., 60. ),
                wmath::WPosition( 80., 80., 80. ) ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
    }
    else
    {
        boost::shared_ptr< WROIBox > newRoi = boost::shared_ptr< WROIBox >( new WROIBox( wmath::WPosition( 60., 60., 60. ),
                wmath::WPosition( 80., 80., 80. ) ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, m_datasetBrowser->getSelectedRoi()->getROI() );
    }
}
