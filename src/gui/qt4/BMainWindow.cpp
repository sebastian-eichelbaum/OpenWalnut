//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include <QtGui/QDockWidget>

#include "BMainWindow.h"
#include "BQtGLWidget.h"
#include "BQtPipelineBrowser.h"

#include "../icons/BIcons.h"

void BMainWindow::addDockableGLWidget( QMainWindow *MainWindow )
{
    m_glWidget = new BQtGLWidget();
    QDockWidget *dockWidget = new QDockWidget( "Graphics Display" );
    dockWidget->setAllowedAreas( Qt::LeftDockWidgetArea
            | Qt::RightDockWidgetArea );
    dockWidget->setWidget( m_glWidget );
    MainWindow->addDockWidget( Qt::LeftDockWidgetArea, dockWidget );
}

void BMainWindow::setupGUI( QMainWindow *MainWindow )
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

    m_actionAbout_BrainCognize = new QAction( MainWindow );
    m_actionAbout_BrainCognize->setObjectName( QString::fromUtf8(
            "actionAbout_BrainCognize" ) );
    m_actionAbout_BrainCognize->setIcon( aboutIcon );
    m_actionAbout_BrainCognize->setText( QApplication::translate( "MainWindow",
            "About BrainCognize", 0, QApplication::UnicodeUTF8 ) );

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
    m_toolBar->addAction( m_actionExit );
    m_toolBar->addAction( m_actionSave );
    m_toolBar->addAction( m_actionLoad );
    m_toolBar->addSeparator();
    m_toolBar->addAction( m_actionAbout_BrainCognize );
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

    m_menuHelp->addAction( m_actionAbout_BrainCognize );
    m_menuHelp->setTitle( QApplication::translate( "MainWindow", "&Help", 0,
            QApplication::UnicodeUTF8 ) );

    m_glWidget = new BQtGLWidget();
    MainWindow->setCentralWidget( m_glWidget );

    // initially 3 views
    addDockableGLWidget( MainWindow );
    addDockableGLWidget( MainWindow );
    addDockableGLWidget( MainWindow );

    m_pipelineBrowser = new BQtPipelineBrowser();
    MainWindow->addDockWidget( Qt::RightDockWidgetArea, m_pipelineBrowser );

    MainWindow->setWindowTitle( QApplication::translate( "MainWindow",
            "BrainCognize", 0, QApplication::UnicodeUTF8 ) );
    m_toolBar->setWindowTitle( QApplication::translate( "MainWindow",
            "toolBar", 0, QApplication::UnicodeUTF8 ) );

    QObject::connect( m_actionExit, SIGNAL( activated() ), MainWindow,
            SLOT( close() ) );

    QMetaObject::connectSlotsByName( MainWindow );
}

void BMainWindow::setEnabled( bool enable )
{
    // TODO(wiebel) implement here
}
