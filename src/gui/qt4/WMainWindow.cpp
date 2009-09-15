//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include "WMainWindow.h"
#include "WQtGLWidget.h"

#include "WQtPipelineBrowser.h"
#include "../../kernel/WKernel.h"

#include "../icons/WIcons.h"

WMainWindow::~WMainWindow()
{
    // clean up list with views
    m_glWidgets.clear();
}

void WMainWindow::addDockableGLWidget( QMainWindow *MainWindow )
{
    QDockWidget *dockWidget = new QDockWidget( "Graphics Display" );
    dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea
            | Qt::RightDockWidgetArea );
    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( dockWidget ) );
    m_glWidgets.push_back( widget );
    dockWidget->setWidget( widget.get() );
    MainWindow->addDockWidget( Qt::LeftDockWidgetArea, dockWidget );
}


void WMainWindow::connectSlots( QMainWindow *MainWindow )
{
    QObject::connect( m_actionExit, SIGNAL( activated() ), MainWindow, SLOT( close() ) );
    QObject::connect( m_actionLoad, SIGNAL( activated() ), this, SLOT( load() ) );
    QMetaObject::connectSlotsByName( MainWindow );
}


void WMainWindow::setupGUI( QMainWindow *MainWindow )
{
    QIcon mainWindowIcon;
    QIcon quitIcon;
    QIcon saveIcon;
    QIcon loadIcon;
    QIcon aboutIcon;

    mainWindowIcon.addPixmap( QPixmap( logoIcon_xpm ) );
    quitIcon.addPixmap( QPixmap( quit_xpm ) );
    saveIcon.addPixmap( QPixmap( disc_xpm ) );
    loadIcon.addPixmap( QPixmap( fileopen_xpm ) );
    aboutIcon.addPixmap( QPixmap( logoIcon_xpm ) );

    if( MainWindow->objectName().isEmpty() )
        MainWindow->setObjectName( QString::fromUtf8( "MainWindow" ) );
    MainWindow->resize( 946, 632 );
    MainWindow->setWindowIcon( mainWindowIcon );

    m_actionLoad = new QAction( MainWindow );
    m_actionLoad->setObjectName( QString::fromUtf8( "actionLoad" ) );
    m_actionLoad->setIcon( loadIcon );
    m_actionLoad->setText( QApplication::translate( "MainWindow", "Load", 0,
            QApplication::UnicodeUTF8 ) );
    m_actionLoad->setShortcut( QApplication::translate( "MainWindow", "Ctrl+L",
            0, QApplication::UnicodeUTF8 ) );

    m_actionSave = new QAction( MainWindow );
    m_actionSave->setObjectName( QString::fromUtf8( "actionSave" ) );
    m_actionSave->setIcon( saveIcon );
    m_actionSave->setText( QApplication::translate( "MainWindow", "Save", 0,
            QApplication::UnicodeUTF8 ) );

    m_actionPreferences = new QAction( MainWindow );
    m_actionPreferences->setObjectName( QString::fromUtf8( "actionPreferences" ) );
    m_actionPreferences->setText( QApplication::translate( "MainWindow",
            "Preferences", 0, QApplication::UnicodeUTF8 ) );

    m_actionExit = new QAction( MainWindow );
    m_actionExit->setObjectName( QString::fromUtf8( "actionExit" ) );
    m_actionExit->setIcon( quitIcon );
    m_actionExit->setText( QApplication::translate( "MainWindow", "E&xit", 0,
            QApplication::UnicodeUTF8 ) );
    m_actionExit->setShortcut( QApplication::translate( "MainWindow", "Ctrl+Q",
            0, QApplication::UnicodeUTF8 ) );

    m_actionAbout_OpenWalnut = new QAction( MainWindow );
    m_actionAbout_OpenWalnut->setObjectName( QString::fromUtf8(
            "actionAbout_OpenWalnut" ) );
    m_actionAbout_OpenWalnut->setIcon( aboutIcon );
    m_actionAbout_OpenWalnut->setText( QApplication::translate( "MainWindow",
            "About OpenWalnut", 0, QApplication::UnicodeUTF8 ) );

    m_centralwidget = new QWidget( MainWindow );
    m_centralwidget->setObjectName( QString::fromUtf8( "centralwidget" ) );
    MainWindow->setCentralWidget( m_centralwidget );

    m_menubar = new QMenuBar( MainWindow );
    m_menubar->setObjectName( QString::fromUtf8( "menubar" ) );
    m_menubar->setGeometry( QRect( 0, 0, 946, 24 ) );

    m_menuFile = new QMenu( m_menubar );
    m_menuFile->setObjectName( QString::fromUtf8( "menuFile" ) );

    m_menuHelp = new QMenu( m_menubar );
    m_menuHelp->setObjectName( QString::fromUtf8( "menuHelp" ) );
    MainWindow->setMenuBar( m_menubar );

    m_toolBar = new QToolBar( MainWindow );
    m_toolBar->setObjectName( QString::fromUtf8( "toolBar" ) );
    m_toolBar->addAction( m_actionSave );
    m_toolBar->addAction( m_actionLoad );
    m_toolBar->addSeparator();
    m_toolBar->addAction( m_actionAbout_OpenWalnut );
    MainWindow->addToolBar( Qt::TopToolBarArea, m_toolBar );

    m_statusBar = new QStatusBar( MainWindow );
    m_statusBar->setObjectName( QString::fromUtf8( "statusBar" ) );
    m_statusBar->showMessage( "No status message yet." );
    MainWindow->setStatusBar( m_statusBar );

    m_menubar->addAction( m_menuFile->menuAction() );
    m_menubar->addAction( m_menuHelp->menuAction() );

    m_menuFile->addAction( m_actionLoad );
    m_menuFile->addAction( m_actionSave );
    m_menuFile->addSeparator();
    m_menuFile->addAction( m_actionPreferences );
    m_menuFile->addSeparator();
    m_menuFile->addAction( m_actionExit );
    m_menuFile->setTitle( QApplication::translate( "MainWindow", "&File", 0,
            QApplication::UnicodeUTF8 ) );

    m_menuHelp->addAction( m_actionAbout_OpenWalnut );
    m_menuHelp->setTitle( QApplication::translate( "MainWindow", "&Help", 0,
            QApplication::UnicodeUTF8 ) );

    boost::shared_ptr<WQtGLWidget> widget = boost::shared_ptr<WQtGLWidget>( new WQtGLWidget( MainWindow ) );
    m_glWidgets.push_back( widget );
    MainWindow->setCentralWidget( widget.get() );

    // initially 3 views
    addDockableGLWidget( MainWindow );
    addDockableGLWidget( MainWindow );
    addDockableGLWidget( MainWindow );

    m_pipelineBrowser = new WQtPipelineBrowser();
    MainWindow->addDockWidget( Qt::RightDockWidgetArea, m_pipelineBrowser );

    MainWindow->setWindowTitle( QApplication::translate( "MainWindow",
            "OpenWalnut", 0, QApplication::UnicodeUTF8 ) );
    m_toolBar->setWindowTitle( QApplication::translate( "MainWindow",
            "toolBar", 0, QApplication::UnicodeUTF8 ) );

    connectSlots( MainWindow );
}

void WMainWindow::setEnabled( bool /* enable */ )
{
    // TODO(wiebel): implement here
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
