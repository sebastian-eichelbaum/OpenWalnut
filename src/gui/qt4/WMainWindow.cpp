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
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>

#include <QtGui/QSlider>

#include <QtGui/QVBoxLayout>


#include "WMainWindow.h"
#include "WQtGLWidget.h"
#include "WQtRibbonMenu.h"

#include "datasetbrowser/WQtDatasetBrowser.h"
#include "../../kernel/WKernel.h"

#include "../icons/logoIcon.xpm"

WMainWindow::~WMainWindow()
{
    // clean up list with views
    m_glWidgets.clear();
}


void WMainWindow::connectSlots( QMainWindow *MainWindow )
{
    QObject::connect( m_toolBar->getQuitButton(), SIGNAL( pressed() ), MainWindow, SLOT( close() ) );
    QObject::connect( m_toolBar->getLoadButton(), SIGNAL( pressed() ), this, SLOT( load() ) );
    QMetaObject::connectSlotsByName( MainWindow );
}


void WMainWindow::setupGUI( QMainWindow *MainWindow )
{
    m_mainWindowIcon.addPixmap( QPixmap( logoIcon_xpm ) );

    if( MainWindow->objectName().isEmpty() )
    {
        MainWindow->setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    MainWindow->resize( 946, 632 );
    MainWindow->setWindowIcon( m_mainWindowIcon );


    m_centralwidget = new QWidget( MainWindow );
    m_centralwidget->setObjectName( QString::fromUtf8( "centralwidget" ) );
    MainWindow->setCentralWidget( m_centralwidget );

    m_toolBar = new WQtRibbonMenu( MainWindow );
    MainWindow->addToolBar( Qt::TopToolBarArea, m_toolBar );

    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( MainWindow ) );
    m_glWidgets.push_back( widget );
    MainWindow->setCentralWidget( widget.get() );

    // initially 3 views
    addNavigationGLWidget( MainWindow, QString( "axial" ) );
    addNavigationGLWidget( MainWindow, QString( "coronal" ) );
    addNavigationGLWidget( MainWindow, QString( "sagittal" ) );

    m_datasetBrowser = new WQtDatasetBrowser();
    MainWindow->addDockWidget( Qt::RightDockWidgetArea, m_datasetBrowser );

    m_datasetBrowser->addSubject( "subject1" );
    m_datasetBrowser->addDataset( 0, "mr188_t1" );
    m_datasetBrowser->addDataset( 0, "mr188_evec" );
    m_datasetBrowser->addDataset( 0, "mr188_t2" );
    m_datasetBrowser->addDataset( 0, "mr188_rgb" );

    MainWindow->setWindowTitle( QApplication::translate( "MainWindow",
            "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

    connectSlots( MainWindow );
}


QSlider* WMainWindow::addNavigationGLWidget( QMainWindow *MainWindow, QString title )
{
    QDockWidget *dockWidget = new QDockWidget( title );
    dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
    dockWidget->setFeatures( QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );

    QSlider *slider = new QSlider( Qt::Horizontal );
    QWidget* panel = new QWidget( dockWidget );

    QVBoxLayout* layout = new QVBoxLayout();

    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( panel ) );
    // TODO(all): add some way to remove QtGLWidgets on destruction to delete QtGLWidget
    m_glWidgets.push_back( widget );

    layout->addWidget( widget.get() );
    layout->addWidget( slider );

    panel->setLayout( layout );

    dockWidget->setWidget( panel );

    MainWindow->addDockWidget( Qt::LeftDockWidgetArea, dockWidget );

    return slider;
}


void WMainWindow::load()
{
    std::cout << "test output: load function" << std::endl;
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

    WKernel::getRunningKernel()->doLoadDataSets( stdFileNames );
}


WQtDatasetBrowser* WMainWindow::getDatasetBrowser()
{
    return m_datasetBrowser;
}
