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

#include <boost/thread.hpp>

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QSlider>
#include <QtGui/QShortcut>
#include <QtGui/QVBoxLayout>

#include "WMainWindow.h"
#include "WQtGLWidget.h"
#include "../../kernel/WKernel.h"

#include "WQtNavGLWidget.h"

#include "../icons/WIcons.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_iconManager(),
    m_propertyManager()
{
    setupGUI();
}

WMainWindow::~WMainWindow()
{
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

    m_mainGLWidget = boost::shared_ptr< WQtGLWidget >( new WQtGLWidget( this, WGECamera::ORTHOGRAPHIC ) );
    m_mainGLWidget->initialize();
    setCentralWidget( m_mainGLWidget.get() );

    //TODO(all): this is commented out
    // initially 3 views
    // m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "axial", 160, "axialPos" ) );
    //m_navAxial->getGLWidget()->initialize();
    //addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );

    //m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "coronal", 200, "coronalPos" ) );
    //m_navCoronal->getGLWidget()->initialize();
    //addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );

    //m_navSagittal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "sagittal", 160, "sagittalPos" ) );
    //m_navSagittal->getGLWidget()->initialize();
    //addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );

    //connect( m_navAxial.get(), SIGNAL( navSliderValueChanged( QString, int ) ), &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
    //connect( m_navCoronal.get(), SIGNAL( navSliderValueChanged( QString, int ) ), &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
    //connect( m_navSagittal.get(), SIGNAL( navSliderValueChanged( QString, int ) ), &m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );

    m_datasetBrowser = new WQtDatasetBrowser();
    addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );
    m_datasetBrowser->addSubject( "subject1" );

    connect( m_datasetBrowser, SIGNAL( dataSetBrowserEvent( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );

    setupToolBar();
}

void WMainWindow::setupToolBar()
{
    m_toolBar = new WQtRibbonMenu( this );

    m_iconManager.addIcon( std::string( "quit" ), quit_xpm );
    m_iconManager.addIcon( std::string( "save" ), disc_xpm );
    m_iconManager.addIcon( std::string( "load" ), fileopen_xpm );

    m_toolBar->addTab( QString( "File" ) );
    m_toolBar->addPushButton( QString( "buttonLoad" ), QString( "File" ), m_iconManager.getIcon( "load" ), QString( "Load" ) );
    m_toolBar->addPushButton( QString( "buttonSave" ), QString( "File" ), m_iconManager.getIcon( "save" ), QString( "Save" ) );
    m_toolBar->addPushButton( QString( "buttonQuit" ), QString( "File" ), m_iconManager.getIcon( "quit" ), QString( "Exit" ) );

    // the parent (this) will take care for deleting the shortcut
    QShortcut* shortcut = new QShortcut( QKeySequence( tr( "Ctrl+Q", "File|Exit" ) ), this );
    connect( shortcut, SIGNAL( activated() ), this, SLOT( close() ) );

    connect( m_toolBar->getButton( QString( "buttonQuit" ) ), SIGNAL( pressed() ), this, SLOT( close() ) );
    connect( m_toolBar->getButton( QString( "buttonLoad" ) ), SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );

    m_toolBar->addTab( QString( "Modules" ) );
    m_toolBar->addTab( QString( "Help" ) );

    m_iconManager.addIcon( std::string( "axial" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal" ), sag_xpm );

    m_toolBar->addPushButton( QString( "showAxial" ), QString( "Modules" ), m_iconManager.getIcon( "axial" ) );
    m_toolBar->addPushButton( QString( "showCoronal" ), QString( "Modules" ), m_iconManager.getIcon( "coronal" ) );
    m_toolBar->addPushButton( QString( "showSagittal" ), QString( "Modules" ), m_iconManager.getIcon( "sagittal" ) );

    m_toolBar->getButton( QString( "showAxial" ) )->setMaximumSize( 24, 24 );
    m_toolBar->getButton( QString( "showCoronal" ) )->setMaximumSize( 24, 24 );
    m_toolBar->getButton( QString( "showSagittal" ) )->setMaximumSize( 24, 24 );

    m_toolBar->getButton( QString( "showAxial" ) )->setCheckable( true );
    m_toolBar->getButton( QString( "showCoronal" ) )->setCheckable( true );
    m_toolBar->getButton( QString( "showSagittal" ) )->setCheckable( true );

    m_toolBar->getButton( QString( "showAxial" ) )->setChecked( true );
    m_toolBar->getButton( QString( "showCoronal" ) )->setChecked( true );
    m_toolBar->getButton( QString( "showSagittal" ) )->setChecked( true );

    connect( m_toolBar->getButton( QString( "showAxial" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_toolBar->getButton( QString( "showCoronal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_toolBar->getButton( QString( "showSagittal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), &m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );

    addToolBar( Qt::TopToolBarArea, m_toolBar );
}


WQtDatasetBrowser* WMainWindow::getDatasetBrowser()
{
    return m_datasetBrowser;
}

WQtRibbonMenu* WMainWindow::getToolBar()
{
    return m_toolBar;
}

void WMainWindow::openLoadDialog()
{
    QFileDialog fd;
    fd.setFileMode( QFileDialog::ExistingFiles );

    QStringList filters;
    filters << "Any files (*)"
            << "EEG files (*.cnt *.edf)"
            << "NIfTI (*.nii *.nii.gz)"
            << "VTK (*.vtk)";
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

boost::signal1< void, std::vector< std::string > >* WMainWindow::getLoaderSignal()
{
    return &m_loaderSignal;
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

        //TODO(all): this is commented out
        //m_navAxial->close();
        //m_navCoronal->close();
        //m_navSagittal->close();

        // finally close
        e->accept();
    }
    else
    {
        e->ignore();
    }
}

