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

#include <QtGui/QApplication>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>

#include "WMainWindow.h"
#include "WQtGLWidget.h"

#include "WQtNavGLWidget.h"

#include "../icons/WIcons.h"

WMainWindow::WMainWindow()
{
    m_propertyManager = new WPropertyManager();
}

WMainWindow::~WMainWindow()
{
    // clean up list with views
    m_glWidgets.clear();
}


void WMainWindow::setupGUI( QMainWindow *mainWindow )
{
    m_mainWindowIcon.addPixmap( QPixmap( logoIcon_xpm ) );

    if( mainWindow->objectName().isEmpty() )
    {
        mainWindow->setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    mainWindow->resize( 946, 632 );
    mainWindow->setWindowIcon( m_mainWindowIcon );
    mainWindow->setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

    m_centralwidget = new QWidget( mainWindow );
    m_centralwidget->setObjectName( QString::fromUtf8( "centralwidget" ) );
    mainWindow->setCentralWidget( m_centralwidget );

    std::cout << "init main gl" << std::endl;
    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( mainWindow ) );
    m_glWidgets.push_back( widget );
    mainWindow->setCentralWidget( widget.get() );

    // initially 3 views
    std::cout << "init nav gl 1" << std::endl;
    m_navAxial = new WQtNavGLWidget( "axial", 160, "axialPos" );
    m_glWidgets.push_back( m_navAxial->getGLWidget() );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navAxial );

    std::cout << "init nav gl 2" << std::endl;
    m_navCoronal = new WQtNavGLWidget( "coronal", 200, "coronalPos" );
    m_glWidgets.push_back( m_navCoronal->getGLWidget() );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal );

    std::cout << "init nav gl 3" << std::endl;
    m_navSagittal = new WQtNavGLWidget( "sagittal", 160, "sagittalPos" );
    m_glWidgets.push_back( m_navSagittal->getGLWidget() );
    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal );

    connect( m_navAxial, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
    connect( m_navCoronal, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
    connect( m_navSagittal, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );

    m_datasetBrowser = new WQtDatasetBrowser();
    mainWindow->addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );
    m_datasetBrowser->addSubject( "subject1" );

    connect( m_datasetBrowser, SIGNAL( dataSetBrowserEvent( QString, bool ) ), m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );

    setupToolBar( mainWindow );
}

void WMainWindow::setupToolBar( QMainWindow *mainWindow )
{
    m_toolBar = new WQtRibbonMenu( mainWindow );

    m_quitIcon.addPixmap( QPixmap( quit_xpm ) );
    m_saveIcon.addPixmap( QPixmap( disc_xpm ) );
    m_loadIcon.addPixmap( QPixmap( fileopen_xpm ) );

    m_toolBar->addTab( QString( "File" ) );
    m_toolBar->addPushButton( QString( "buttonLoad" ), QString( "File" ), m_loadIcon, QString( "load" ) );
    m_toolBar->addPushButton( QString( "buttonSave" ), QString( "File" ), m_loadIcon, QString( "save" ) );
    m_toolBar->addPushButton( QString( "buttonQuit" ), QString( "File" ), m_loadIcon, QString( "exit" ) );

    m_toolBar->getButton( QString( "buttonLoad" ) )->setMaximumSize( 50, 24 );
    m_toolBar->getButton( QString( "buttonSave" ) )->setMaximumSize( 50, 24 );
    m_toolBar->getButton( QString( "buttonQuit" ) )->setMaximumSize( 50, 24 );

    connect( m_toolBar->getButton( QString( "buttonQuit" ) ), SIGNAL( pressed() ), mainWindow, SLOT( close() ) );
    connect( m_toolBar->getButton( QString( "buttonLoad" ) ), SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );

    m_toolBar->addTab( QString( "Modules" ) );
    m_toolBar->addTab( QString( "Help" ) );

    m_axiIcon.addPixmap( QPixmap( axial_xpm ) );
    m_corIcon.addPixmap( QPixmap( cor_xpm ) );
    m_sagIcon.addPixmap( QPixmap( sag_xpm ) );

    m_toolBar->addPushButton( QString( "showAxial" ), QString( "Modules" ), m_axiIcon );
    m_toolBar->addPushButton( QString( "showCoronal" ), QString( "Modules" ), m_corIcon );
    m_toolBar->addPushButton( QString( "showSagittal" ), QString( "Modules" ), m_sagIcon );

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
            SIGNAL( pushButtonToggled( QString, bool ) ), m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_toolBar->getButton( QString( "showCoronal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
    connect( m_toolBar->getButton( QString( "showSagittal" ) ),
            SIGNAL( pushButtonToggled( QString, bool ) ), m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );

    mainWindow->addToolBar( Qt::TopToolBarArea, m_toolBar );
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
    return m_propertyManager;
}
