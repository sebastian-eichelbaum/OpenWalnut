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

#include "../icons/logoIcon.xpm"

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


    m_centralwidget = new QWidget( mainWindow );
    m_centralwidget->setObjectName( QString::fromUtf8( "centralwidget" ) );
    mainWindow->setCentralWidget( m_centralwidget );

    m_toolBar = new WQtRibbonMenu( mainWindow );
    mainWindow->addToolBar( Qt::TopToolBarArea, m_toolBar );

    std::cout << "init main gl" << std::endl;
    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( mainWindow ) );
    m_glWidgets.push_back( widget );
    mainWindow->setCentralWidget( widget.get() );

//    // initially 3 views
//    std::cout << "init nav gl 1" << std::endl;
//    m_navAxial = new WQtNavGLWidget( "axial", 160, "axialPos" );
//    m_glWidgets.push_back( m_navAxial->getGLWidget() );
//    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navAxial );
//
//    std::cout << "init nav gl 2" << std::endl;
//    m_navCoronal = new WQtNavGLWidget( "coronal", 200, "coronalPos" );
//    m_glWidgets.push_back( m_navCoronal->getGLWidget() );
//    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal );
//
//    std::cout << "init nav gl 3" << std::endl;
//    m_navSagittal = new WQtNavGLWidget( "sagittal", 160, "sagittalPos" );
//    m_glWidgets.push_back( m_navSagittal->getGLWidget() );
//    mainWindow->addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal );
//
//    connect( m_navAxial, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
//    connect( m_navCoronal, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );
//    connect( m_navSagittal, SIGNAL( navSliderValueChanged( QString, int ) ), m_propertyManager, SLOT( slotIntChanged( QString, int ) ) );

    m_datasetBrowser = new WQtDatasetBrowser();
    mainWindow->addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );

    m_datasetBrowser->addSubject( "subject1" );

    mainWindow->setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

    connect( m_toolBar->getQuitButton(), SIGNAL( pressed() ), mainWindow, SLOT( close() ) );
    connect( m_toolBar->getLoadButton(), SIGNAL( pressed() ), this, SLOT( openLoadDialog() ) );
    connect( m_toolBar->getAxiButton(), SIGNAL( toggled( bool ) ), this, SLOT( toggleAxial( bool ) ) );
    connect( m_toolBar->getCorButton(), SIGNAL( toggled( bool ) ), this, SLOT( toggleCoronal( bool ) ) );
    connect( m_toolBar->getSagButton(), SIGNAL( toggled( bool ) ), this, SLOT( toggleSagittal( bool ) ) );

    connect( m_datasetBrowser, SIGNAL( dataSetBrowserEvent( QString, bool ) ), m_propertyManager, SLOT( slotBoolChanged( QString, bool ) ) );
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

void WMainWindow::toggleAxial(  bool check )
{
    m_propertyManager->slotBoolChanged( "showAxial", check );
}

void WMainWindow::toggleCoronal(  bool check )
{
    m_propertyManager->slotBoolChanged( "showCoronal", check );
}

void WMainWindow::toggleSagittal(  bool check )
{
    m_propertyManager->slotBoolChanged( "showSagittal", check );
}

boost::signal1< void, std::vector< std::string > >* WMainWindow::getLoaderSignal()
{
    return &m_loaderSignal;
}

WPropertyManager*  WMainWindow::getPropertyManager()
{
    return m_propertyManager;
}
