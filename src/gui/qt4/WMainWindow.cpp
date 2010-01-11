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
#include <QtGui/QSlider>
#include <QtGui/QShortcut>
#include <QtGui/QVBoxLayout>

#include "WMainWindow.h"
#include "WOpenCustomDockWidgetEvent.h"
#include "WQtGLWidget.h"
#include "WQtNavGLWidget.h"
#include "WQtCustomDockWidget.h"
#include "../../common/WColor.h"
#include "../../common/WPreferences.h"
#include "../../kernel/WKernel.h"

#include "../icons/WIcons.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_iconManager(),
    m_propertyManager()
{
    setupGUI();
}

void WMainWindow::setupGUI()
{
    m_iconManager.addIcon( std::string( "logo" ), logoIcon_xpm );

    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    resize( 946, 632 );
    setWindowIcon( m_iconManager.getIcon( "logo" ) );
    setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

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
            addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );
            connect( m_navAxial.get(), SIGNAL( navSliderValueChanged( QString, int ) ),
                     &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideCoronal", &hideWidget ) && hideWidget) )
        {
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "coronal", this, 200, "coronalPos" ) );
            addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );
            connect( m_navCoronal.get(), SIGNAL( navSliderValueChanged( QString, int ) ),
                     &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideSagittal", &hideWidget ) && hideWidget) )
        {
            m_navSagittal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "sagittal", this, 160, "sagittalPos" ) );
            addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );
            connect( m_navSagittal.get(), SIGNAL( navSliderValueChanged( QString, int ) ),
                     &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
        }
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

    setupRibbonMenu();
    m_ribbonMenu->clearNonPersistentTabs();

    setupCompatiblesToolBar();

    m_datasetBrowser = new WQtDatasetBrowser( this );
    addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );
    m_datasetBrowser->addSubject( "Default Subject" );

    connect( m_datasetBrowser, SIGNAL( dataSetBrowserEvent( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
}

void WMainWindow::setupRibbonMenu()
{
    m_ribbonMenu = new WQtRibbonMenu( this );

    m_iconManager.addIcon( std::string( "quit" ), quit_xpm );
    m_iconManager.addIcon( std::string( "save" ), disc_xpm );
    m_iconManager.addIcon( std::string( "load" ), fileopen_xpm );

    m_ribbonMenu->addTab( QString( "File" ) );
    m_ribbonMenu->addPushButton( QString( "buttonLoad" ), QString( "File" ), m_iconManager.getIcon( "load" ), QString( "Load" ) );
    m_ribbonMenu->addPushButton( QString( "buttonSave" ), QString( "File" ), m_iconManager.getIcon( "save" ), QString( "Save" ) );
    m_ribbonMenu->addPushButton( QString( "buttonQuit" ), QString( "File" ), m_iconManager.getIcon( "quit" ), QString( "Exit" ) );

    // the parent (this) will take care for deleting the shortcut
    QShortcut* shortcut = new QShortcut( QKeySequence( tr( "Ctrl+Q", "File|Exit" ) ), this );
    connect( shortcut, SIGNAL( activated() ), this, SLOT( close() ) );

    connect( m_ribbonMenu->getButton( QString( "buttonQuit" ) ), SIGNAL( pressed() ), this, SLOT( close() ) );
    connect( m_ribbonMenu->getButton( QString( "buttonLoad" ) ), SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );

    m_ribbonMenu->addTab( QString( "Modules" ) );
    m_ribbonMenu->addTab( QString( "Help" ) );

    m_iconManager.addIcon( std::string( "axial" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal" ), sag_xpm );

    m_ribbonMenu->addPushButton( QString( "showAxial" ), QString( "Modules" ), m_iconManager.getIcon( "axial" ) );
    m_ribbonMenu->addPushButton( QString( "showCoronal" ), QString( "Modules" ), m_iconManager.getIcon( "coronal" ) );
    m_ribbonMenu->addPushButton( QString( "showSagittal" ), QString( "Modules" ), m_iconManager.getIcon( "sagittal" ) );

    m_ribbonMenu->getButton( QString( "showAxial" ) )->setMaximumSize( 24, 24 );
    m_ribbonMenu->getButton( QString( "showCoronal" ) )->setMaximumSize( 24, 24 );
    m_ribbonMenu->getButton( QString( "showSagittal" ) )->setMaximumSize( 24, 24 );

    m_ribbonMenu->getButton( QString( "showAxial" ) )->setCheckable( true );
    m_ribbonMenu->getButton( QString( "showCoronal" ) )->setCheckable( true );
    m_ribbonMenu->getButton( QString( "showSagittal" ) )->setCheckable( true );

    m_ribbonMenu->getButton( QString( "showAxial" ) )->setChecked( true );
    m_ribbonMenu->getButton( QString( "showCoronal" ) )->setChecked( true );
    m_ribbonMenu->getButton( QString( "showSagittal" ) )->setChecked( true );

    m_ribbonMenu->getButton( QString( "showAxial" ) )->setToolTip( "Axial" );
    m_ribbonMenu->getButton( QString( "showCoronal" ) )->setToolTip( "Coronal" );
    m_ribbonMenu->getButton( QString( "showSagittal" ) )->setToolTip( "Sagittal" );

    connect( m_ribbonMenu->getButton( QString( "showAxial" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_ribbonMenu->getButton( QString( "showCoronal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_ribbonMenu->getButton( QString( "showSagittal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );

    addToolBar( Qt::TopToolBarArea, m_ribbonMenu );
}

void WMainWindow::setupCompatiblesToolBar()
{
    m_compatiblesToolBar = new WQtRibbonMenu( this );

    addToolBar( Qt::TopToolBarArea, m_compatiblesToolBar );
}

WQtDatasetBrowser* WMainWindow::getDatasetBrowser()
{
    return m_datasetBrowser;
}

WQtRibbonMenu* WMainWindow::getRibbonMenu()
{
    return m_ribbonMenu;
}

WQtRibbonMenu* WMainWindow::getCompatiblesToolBar()
{
    return m_compatiblesToolBar;
}

void WMainWindow::openLoadDialog()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Known file types (*.edf *.asc *.nii *.nii.gz *.fib)"
            << "EEG files (*.asc *.edf)"
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

    std::vector< std::string >stdFileNames;

    QStringList::const_iterator constIterator;
    for ( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        stdFileNames.push_back( ( *constIterator ).toLocal8Bit().constData() );
    }
    m_loaderSignal( stdFileNames );
}

boost::signals2::signal1< void, std::vector< std::string > >* WMainWindow::getLoaderSignal()
{
    return &m_loaderSignal;
}

boost::signals2::signal2< void, boost::shared_ptr< WModule >, boost::shared_ptr< WModule > >* WMainWindow::getModuleButtonSignal()
{
    return &m_moduleButtonSignal;
}


boost::signals2::signal1< void, std::string >* WMainWindow::getPickSignal()
{
    return m_mainGLWidget->getPickSignal();
}

WPropertyManager*  WMainWindow::getPropertyManager()
{
    return &m_propertyManager;
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

        m_navAxial->close();
        m_navCoronal->close();
        m_navSagittal->close();

        // delete CustomDockWidgets
        boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
        for( std::map< std::string, boost::shared_ptr< WQtCustomDockWidget > >::iterator it = m_customDockWidgets.begin();
             it != m_customDockWidgets.end(); ++it )
        {
            it->second->close();
        }
        m_customDockWidgetsLock.unlock();

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
        m_customDockWidgetsLock.unlock();

        ocdwEvent->getFlag()->set( widget );
    }
    else
    {
        // other event
        QMainWindow::customEvent( event );
    }
}

boost::shared_ptr< WQtCustomDockWidget > WMainWindow::getCustomDockWidget( std::string title )
{
    boost::mutex::scoped_lock lock( m_customDockWidgetsLock );
    boost::shared_ptr< WQtCustomDockWidget > out = m_customDockWidgets.count( title ) > 0 ?
        m_customDockWidgets[title] :
        boost::shared_ptr< WQtCustomDockWidget >();
    m_customDockWidgetsLock.unlock();
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
    m_customDockWidgetsLock.unlock();
}

void WMainWindow::slotActivateModule( QString module )
{
    // TODO(schurade): do we really need the signal? Why can't we use the kernel directly?
    m_moduleButtonSignal( getDatasetBrowser()->getSelectedModule(), WModuleFactory::getModuleFactory()->getPrototypeByName( module.toStdString() ) );
}

