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
#include <map>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QDockWidget>
#include <QtGui/QFileDialog>
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QMessageBox>
#include <QtGui/QTextEdit>
#include <QtWebKit/QWebView>
#include <QtGui/QShortcut>
#include <QtGui/QSlider>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <QtCore/QSettings>

#include "../../common/WColor.h"
#include "../../common/WPreferences.h"
#include "../../common/WIOTools.h"
#include "../../common/WProjectFileIO.h"
#include "../../common/WPathHelper.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WEEG2.h"
#include "../../graphicsEngine/WGEZoomTrackballManipulator.h"
#include "../../graphicsEngine/WROIBox.h"
#include "../../kernel/modules/data/WMData.h"
#include "../../kernel/WKernel.h"
#include "../../kernel/WModule.h"
#include "../../kernel/WModuleCombiner.h"
#include "../../kernel/WModuleCombinerTypes.h"
#include "../../kernel/WProjectFile.h"
#include "../../kernel/WROIManager.h"
#include "../../kernel/WSelectionManager.h"
#include "../icons/WIcons.h"
#include "controlPanel/WPropertyBoolWidget.h"
#include "controlPanel/WQtControlPanel.h"
#include "events/WEventTypes.h"
#include "events/WModuleCrashEvent.h"
#include "events/WModuleReadyEvent.h"
#include "events/WModuleRemovedEvent.h"
#include "events/WOpenCustomDockWidgetEvent.h"
#include "guiElements/WQtPropertyBoolAction.h"
#include "WQtCombinerToolbar.h"
#include "WQtCustomDockWidget.h"
#include "WQtNavGLWidget.h"
#include "WQtGLDockWidget.h"
#include "WQtGLScreenCapture.h"

#include "WMainWindow.h"

WMainWindow::WMainWindow() :
    QMainWindow(),
    m_currentCompatiblesToolbar( NULL ),
    m_iconManager(),
    m_navSlicesAlreadyLoaded( false )
{
}

WMainWindow::~WMainWindow()
{
    // cleanup
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
    m_iconManager.addIcon( std::string( "config" ), preferences_system_xpm );
    m_iconManager.addIcon( std::string( "view" ), camera_xpm );
    m_iconManager.addIcon( std::string( "video" ), video_xpm );
    m_iconManager.addIcon( std::string( "image" ), image_xpm );
    m_iconManager.addIcon( std::string( "preferences" ), preferences_xpm );

    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }
    // TODO(all): what is this?
    resize( 946, 632 );
    setWindowIcon( m_iconManager.getIcon( "logo" ) );
    setWindowTitle( QApplication::translate( "MainWindow", "OpenWalnut (development version)", 0, QApplication::UnicodeUTF8 ) );

    setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks );

    //network Editor
    m_networkEditor = new WQtNetworkEditor( this );
    m_networkEditor->setFeatures( QDockWidget::AllDockWidgetFeatures );

    // the control panel instance is needed for the menu
    m_controlPanel = new WQtControlPanel( this );
    m_controlPanel->setFeatures( QDockWidget::AllDockWidgetFeatures );
    m_controlPanel->addSubject( "Default Subject" );

    // add all docks
    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel->getModuleDock() );
    addDockWidget( Qt::RightDockWidgetArea, m_networkEditor );
    tabifyDockWidget( m_networkEditor, m_controlPanel->getModuleDock() );

    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel->getColormapperDock() );
    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel->getRoiDock() );
    addDockWidget( Qt::RightDockWidgetArea, m_mainGLWidgetScreenCapture );
    tabifyDockWidget( m_controlPanel->getColormapperDock(), m_controlPanel->getRoiDock() );

    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel );

    m_glDock = new QMainWindow();
    m_glDock->setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks );
    m_glDock->setDocumentMode( true );
    setCentralWidget( m_glDock );
    WQtGLDockWidget* mainGLDock = new WQtGLDockWidget( "main", "3D View", m_glDock );
    mainGLDock->getGLWidget()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_mainGLWidget = mainGLDock->getGLWidget();
    m_mainGLWidgetScreenCapture = m_mainGLWidget->getScreenCapture( this );
    m_glDock->addDockWidget( Qt::RightDockWidgetArea, mainGLDock );

    m_permanentToolBar = new WQtToolBar( "Permanent Toolbar", this );

    // Set the style of the toolbar
    // NOTE: this only works if the toolbar is used with QActions instead of buttons and other widgets
    m_permanentToolBar->setToolButtonStyle( getToolbarStyle() );

    m_iconManager.addIcon( std::string( "ROI icon" ), box_xpm );
    m_iconManager.addIcon( std::string( "Reset icon" ), o_xpm );
    m_iconManager.addIcon( std::string( "axial icon" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal icon" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal icon" ), sag_xpm );

    m_loadButton = new QAction( m_iconManager.getIcon( "load" ), "load", m_permanentToolBar );
    QAction* roiButton = new QAction( m_iconManager.getIcon( "ROI icon" ), "ROI", m_permanentToolBar );
    QAction* resetButton = new QAction( m_iconManager.getIcon( "view" ), "Reset", m_permanentToolBar );
    resetButton->setShortcut( QKeySequence( Qt::Key_Escape ) );
    QAction* projectLoadButton = new QAction( m_iconManager.getIcon( "loadProject" ), "loadProject", m_permanentToolBar );
    QAction* projectSaveButton = new QAction( m_iconManager.getIcon( "saveProject" ), "saveProject", m_permanentToolBar );

    connect( m_loadButton, SIGNAL(  triggered( bool ) ), this, SLOT( openLoadDialog() ) );
    connect( resetButton, SIGNAL(  triggered( bool ) ), m_mainGLWidget.get(), SLOT( reset() ) );
    connect( roiButton, SIGNAL(  triggered( bool ) ), this, SLOT( newRoi() ) );
    connect( projectLoadButton, SIGNAL(  triggered( bool ) ), this, SLOT( projectLoad() ) );
    connect( projectSaveButton, SIGNAL( triggered( bool ) ), this, SLOT( projectSaveAll() ) );

    m_loadButton->setToolTip( "Load Data" );
    resetButton->setToolTip( "Reset main view" );
    roiButton->setToolTip( "Create New ROI" );
    projectLoadButton->setToolTip( "Load a project from file" );
    projectSaveButton->setToolTip( "Save current project to file" );

    // we want the upper most tree item to be selected. This helps to make the always compatible modules
    // show up in the tool bar from the beginning. And ... it doesn't hurt.
    m_controlPanel->selectUpperMostEntry();

    // NOTE: Please be aware that not every menu needs a shortcut key. If you add a shortcut, you should use one of the
    // QKeySequence::StandardKey defaults and avoid ambiguities like Ctrl-C for the configure dialog is not the best choice as Ctrl-C, for the
    // most users is the Copy shortcut.

    m_menuBar = new QMenuBar( this );

    // hide menu?
    bool hideMenu = false;
    WPreferences::getPreference( "qt4gui.hideMenuBar", &hideMenu );
    m_menuBar->setVisible( !hideMenu );

    QMenu* fileMenu = m_menuBar->addMenu( "File" );

    fileMenu->addAction( m_iconManager.getIcon( "load" ), "Load Dataset", this, SLOT( openLoadDialog() ), QKeySequence(  QKeySequence::Open ) );
    fileMenu->addSeparator();
    fileMenu->addAction( m_iconManager.getIcon( "loadProject" ), "Load Project", this, SLOT( projectLoad() ) );
    QMenu* saveMenu = fileMenu->addMenu( m_iconManager.getIcon( "saveProject" ), "Save Project" );
    saveMenu->addAction( "Save Project", this, SLOT( projectSaveAll() ), QKeySequence::Save );
    saveMenu->addAction( "Save Modules Only", this, SLOT( projectSaveModuleOnly() ) );
    saveMenu->addAction( "Save Camera Only", this, SLOT( projectSaveCameraOnly() ) );
    saveMenu->addAction( "Save ROIs Only", this, SLOT( projectSaveROIOnly() ) );
    projectSaveButton->setMenu( saveMenu );

    fileMenu->addSeparator();
    fileMenu->addAction( m_iconManager.getIcon( "config" ), "Config", this, SLOT( openConfigDialog() ) );
    fileMenu->addSeparator();
    // TODO(all): If all distributions provide a newer QT version we should use QKeySequence::Quit here
    //fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ), QKeySequence( QKeySequence::Quit ) );
    fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ),  QKeySequence( Qt::CTRL + Qt::Key_Q ) );

    // This QAction stuff is quite ugly and complicated some times ... There is no nice constructor which takes name, slot keysequence and so on
    // directly -> set shortcuts, and some further properties using QAction's interface
    QMenu* viewMenu = m_menuBar->addMenu( "View" );

    QAction* controlPanelTrigger = m_controlPanel->toggleViewAction();
    QList< QKeySequence > controlPanelShortcut;
    controlPanelShortcut.append( QKeySequence( Qt::Key_F9 ) );
    controlPanelTrigger->setShortcuts( controlPanelShortcut );
    this->addAction( controlPanelTrigger );  // this enables the action even if the menu bar is invisible

    // NOTE: the shortcuts for these view presets should be chosen carefully. Most keysequences have another meaning in the most applications
    // so the user may get confused. It is also not a good idea to take letters as they might be used by OpenSceneGraph widget ( like "S" for
    // statistics ).
    // By additionally adding the action to the main window, we ensure the action can be triggered even if the menu bar is hidden.
    QAction* tmpAction = viewMenu->addAction( m_iconManager.getIcon( "sagittal icon" ), "Left", this, SLOT( setPresetViewLeft() ),
                                             QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = viewMenu->addAction( m_iconManager.getIcon( "sagittal icon" ), "Right", this, SLOT( setPresetViewRight() ),
                                     QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_R ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = viewMenu->addAction( m_iconManager.getIcon( "axial icon" ), "Superior", this, SLOT( setPresetViewSuperior() ),
                                     QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = viewMenu->addAction( m_iconManager.getIcon( "axial icon" ), "Inferior", this, SLOT( setPresetViewInferior() ),
                                     QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = viewMenu->addAction( m_iconManager.getIcon( "coronal icon" ), "Anterior", this, SLOT( setPresetViewAnterior() ),
                                     QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_A ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = viewMenu->addAction( m_iconManager.getIcon( "coronal icon" ), "Posterior", this, SLOT( setPresetViewPosterior() ),
                                     QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    resetButton->setMenu( viewMenu );

    QMenu* helpMenu = m_menuBar->addMenu( "Help" );
    helpMenu->addAction( m_iconManager.getIcon( "help" ), "OpenWalnut Help", this, SLOT( openOpenWalnutHelpDialog() ),
                         QKeySequence( QKeySequence::HelpContents ) );
    helpMenu->addSeparator();
    helpMenu->addAction( m_iconManager.getIcon( "logo" ), "About OpenWalnut", this, SLOT( openAboutDialog() ) );
    helpMenu->addAction( "About Qt", this, SLOT( openAboutQtDialog() ) );

    setMenuBar( m_menuBar );

    // initially 3 navigation views
    {
        bool hideWidget;
        if( !( WPreferences::getPreference( "qt4gui.hideAxial", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Axial View", "Axial View", this, "Axial Slice",
                                                                                  m_mainGLWidget.get() ) );
#else
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Axial View", "Axial View", this, "Axial Slice" ) );
#endif
            m_navAxial->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideCoronal", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Coronal View", "Coronal View", this, "Coronal Slice",
                                                                                    m_mainGLWidget.get() ) );
#else
            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Coronal View", "Coronal View", this, "Coronal Slice" ) );
#endif
            m_navCoronal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );
        }
        if( !( WPreferences::getPreference( "qt4gui.hideSagittal", &hideWidget ) && hideWidget) )
        {
#ifndef _MSC_VER
            m_navSagittal =
                boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Sagittal View", "Sagittal View", this, "Sagittal Slice",
                                                                         m_mainGLWidget.get() ) );
#else
            m_navSagittal =
                boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Sagittal View", "Sagittal View", this, "Sagittal Slice" ) );
#endif
            m_navSagittal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );
        }
    }

    // Default background color from config file
    WColor bgColor( 1.0, 1.0, 1.0, 1.0 );
    double r;
    double g;
    double b;
    if( WPreferences::getPreference( "ge.bgColor.r", &r )
        && WPreferences::getPreference( "ge.bgColor.g", &g )
        && WPreferences::getPreference( "ge.bgColor.b", &b ) )
    {
        bgColor.set( r, g, b, 1.0 );
        m_mainGLWidget->setBgColor( bgColor );

        if( m_navAxial )
        {
            m_navAxial->getGLWidget()->setBgColor( bgColor );
        }
        if( m_navCoronal )
        {
            m_navCoronal->getGLWidget()->setBgColor( bgColor );
        }
        if( m_navSagittal )
        {
            m_navSagittal->getGLWidget()->setBgColor( bgColor );
        }
    }

    // create command prompt toolbar
    m_commandPrompt = new WQtCommandPromptToolbar( "Command Prompt", this );
    addToolBar( Qt::TopToolBarArea, m_commandPrompt );
    this->addAction( m_commandPrompt->toggleViewAction() );  // this enables the action even if the menu bar is invisible

    // setup permanent toolbar
    m_permanentToolBar->addAction( m_loadButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addAction( projectLoadButton );
    m_permanentToolBar->addAction( projectSaveButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addAction( resetButton );
    m_permanentToolBar->addAction( roiButton );
    m_permanentToolBar->addSeparator();

    addToolBar( Qt::TopToolBarArea, m_permanentToolBar );

    // after creating the GUI, restore its saved state
    restoreSavedState();
}

void WMainWindow::autoAdd( boost::shared_ptr< WModule > module, std::string proto )
{
    // get the prototype.
    if( !WKernel::getRunningKernel()->getRootContainer()->applyModule( module, proto, true ) )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active but module " + proto + " could not be added.",
                                             "GUI", LL_ERROR );
    }
}

void WMainWindow::moduleSpecificCleanup( boost::shared_ptr< WModule > module )
{
    // nav slices use separate buttons for slice on/off switching
    if( module->getName() == "Navigation Slices" )
    {
        boost::shared_ptr< WPropertyBase > prop;

        prop = module->getProperties()->findProperty( "showAxial" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );

        prop = module->getProperties()->findProperty( "showCoronal" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );

        prop = module->getProperties()->findProperty( "showSagittal" );
        m_permanentToolBar->removeAction( propertyActionMap[prop] );
        propertyActionMap.erase( prop );


        prop = module->getProperties()->findProperty( "Axial Slice" );
        if( m_navAxial )
        {
            m_navAxial->removeSliderProperty( prop );
        }

        prop = module->getProperties()->findProperty( "Coronal Slice" );
        if( m_navCoronal )
        {
            m_navCoronal->removeSliderProperty( prop );
        }

        prop = module->getProperties()->findProperty( "Sagittal Slice" );
        if( m_navSagittal )
        {
            m_navSagittal->removeSliderProperty( prop );
        }
    }
}

void WMainWindow::moduleSpecificSetup( boost::shared_ptr< WModule > module )
{
    // Add all special handlings here. This method is called whenever a module is marked "ready". You can set up the gui for special modules,
    // load certain modules for datasets and so on.

    // The Data Modules also play an special role. To have modules being activated when certain data got loaded, we need to hook it up here.
    bool useAutoDisplay = true;
    WPreferences::getPreference( "qt4gui.useAutoDisplay", &useAutoDisplay );
    if( useAutoDisplay && module->getType() == MODULE_DATA )
    {
        WLogger::getLogger()->addLogMessage( "Auto Display active and Data module added. The proper module will be added.",
                                             "GUI", LL_DEBUG );

        // data modules contain an member denoting the real data type. Currently we only have one data module and a not very modulated data
        // structures.
        boost::shared_ptr< WMData > dataModule = boost::shared_static_cast< WMData >( module );

        // grab data and identify type
        if( dataModule->getDataSet()->isA< WDataSetSingle >() && dataModule->getDataSet()->isTexture() )
        {
            // it is a dataset single
            // load a nav slice module if a WDataSetSingle is available!?
            if ( !m_navSlicesAlreadyLoaded )
            {
                autoAdd( module, "Navigation Slices" );
                m_navSlicesAlreadyLoaded = true;
            }
        }
        else if( dataModule->getDataSet()->isA< WDataSetFibers >() )
        {
            // it is a fiber dataset -> add the FiberDisplay module
            autoAdd( module, "Fiber Display" );
        }
        else if( dataModule->getDataSet()->isA< WEEG2 >() )
        {
            // it is a eeg dataset -> add the eegView module
            autoAdd( module, "EEG View" );
        }
    }

    // nav slices use separate buttons for slice on/off switching
    if( module->getName() == "Navigation Slices" )
    {
        boost::shared_ptr< WPropertyBase > prop = module->getProperties()->findProperty( "Slices/Show Axial" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Show Axial\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Axial Slice" );
            a->setText( "Toggle Axial Slice" );
            a->setIcon( m_iconManager.getIcon( "axial icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        prop = module->getProperties()->findProperty( "Slices/Show Coronal" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Show Coronal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Coronal Slice" );
            a->setText( "Toggle Coronal Slice" );
            a->setIcon( m_iconManager.getIcon( "coronal icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        prop = module->getProperties()->findProperty( "Slices/Show Sagittal" );
        if( !prop )
        {
               WLogger::getLogger()->
                   addLogMessage( "Navigation Slices module does not provide the property \"Show Sagittal\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            WQtPropertyBoolAction* a = new WQtPropertyBoolAction( prop->toPropBool(), m_permanentToolBar );
            a->setToolTip( "Toggle Saggital Slice" );
            a->setText( "Toggle Saggital Slice" );
            a->setIcon( m_iconManager.getIcon( "sagittal icon" ) );
            m_permanentToolBar->addAction( a );
            propertyActionMap[prop] = a;
        }

        // now setup the nav widget sliders
        prop = module->getProperties()->findProperty( "Slices/Axial Position" );
        if( !prop )
        {
               WLogger::getLogger()->
               addLogMessage( "Navigation Slices module does not provide the property \"Axial Position\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navAxial )
            {
                //m_navAxial->setSliderProperty( prop );
            }
        }

        prop = module->getProperties()->findProperty( "Slices/Coronal Position" );
        if( !prop )
        {
               WLogger::getLogger()->
               addLogMessage( "Navigation Slices module does not provide the property \"Coronal Position\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navCoronal )
            {
                //m_navCoronal->setSliderProperty( prop );
            }
        }

        prop = module->getProperties()->findProperty( "Slices/Sagittal Position" );
        if( !prop )
        {
               WLogger::getLogger()->
               addLogMessage( "Navigation Slices module does not provide the property \"Sagittal Position\", which is required by the GUI.", "GUI",
                                  LL_ERROR );
        }
        else
        {
            if( m_navSagittal )
            {
               //m_navSagittal->setSliderProperty( prop );
            }
        }
    }
}

Qt::ToolButtonStyle WMainWindow::getToolbarStyle() const
{
    // this sets the toolbar style
    int toolBarStyle = 0;
    WPreferences::getPreference( "qt4gui.toolBarStyle", &toolBarStyle );
    if( ( toolBarStyle < 0 ) || ( toolBarStyle > 3 ) ) // ensure a valid value
    {
        toolBarStyle = 0;
    }

    // cast and return
    return static_cast< Qt::ToolButtonStyle >( toolBarStyle );
}

void WMainWindow::setCompatiblesToolbar( WQtCombinerToolbar* toolbar )
{
    if( m_currentCompatiblesToolbar )
    {
        delete m_currentCompatiblesToolbar;
    }
    m_currentCompatiblesToolbar = toolbar;

    if( !toolbar )
    {
        // ok, reset the toolbar
        // So create a dummy to permanently reserve the space
        m_currentCompatiblesToolbar = new WQtCombinerToolbar( this );
    }

    // and the position of the toolbar
    addToolBar( Qt::TopToolBarArea, m_currentCompatiblesToolbar );
}

WQtCombinerToolbar* WMainWindow::getCompatiblesToolbar()
{
    return m_currentCompatiblesToolbar;
}

WQtControlPanel* WMainWindow::getControlPanel()
{
    return m_controlPanel;
}

WQtNetworkEditor* WMainWindow::getNetworkEditor()
{
    return m_networkEditor;
}

void WMainWindow::projectSave( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer )
{
    QFileDialog fd;
    fd.setWindowTitle( "Save Project as" );
    fd.setFileMode( QFileDialog::AnyFile );
    fd.setAcceptMode( QFileDialog::AcceptSave );

    QStringList filters;
    filters << "Project File (*.owproj *.owp)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        std::string filename = ( *constIterator ).toStdString();
        // append owp if not existent
        if ( filename.rfind( ".owp" ) == std::string::npos )
        {
            filename += ".owp";
        }

        boost::shared_ptr< WProjectFile > proj = boost::shared_ptr< WProjectFile >(
                new WProjectFile( filename )
        );

        try
        {
            // This call is synchronous.
            if( writer.empty() )
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
    filters << "Simple Project File (*.owproj *.owp)"
            << "Any files (*)";
    fd.setNameFilters( filters );
    fd.setViewMode( QFileDialog::Detail );
    QStringList fileNames;
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
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
#ifdef _MSC_VER
    QStringList fileNames;
    QString filters;
    filters = QString::fromStdString( std::string( "Known file types (*.cnt *.edf *.asc *.nii *.nii.gz *.fib);;" )
        + std::string( "EEG files (*.cnt *.edf *.asc);;" )
        + std::string( "NIfTI (*.nii *.nii.gz);;" )
        + std::string( "Fibers (*.fib);;" )
        + std::string( "Any files (*)" ) );

    fileNames = QFileDialog::getOpenFileNames( this, "Open", "", filters  );
#else
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
    if( fd.exec() )
    {
        fileNames = fd.selectedFiles();
    }
#endif

    std::vector< std::string > stdFileNames;

    QStringList::const_iterator constIterator;
    for( constIterator = fileNames.constBegin(); constIterator != fileNames.constEnd(); ++constIterator )
    {
        stdFileNames.push_back( ( *constIterator ).toLocal8Bit().constData() );
    }

    m_loaderSignal( stdFileNames );
}

void WMainWindow::openAboutQtDialog()
{
    QMessageBox::aboutQt( this, "About Qt" );
}

void WMainWindow::openAboutDialog()
{
    std::string filename( WPathHelper::getAppPath().file_string() + "/../share/OpenWalnut/OpenWalnutAbout.html" );
    std::string content = readFileIntoString( filename );
    QMessageBox::about( this, "About OpenWalnut", content.c_str() );
}

void WMainWindow::openOpenWalnutHelpDialog()
{
    std::string filename( WPathHelper::getAppPath().file_string() + "/../share/OpenWalnut/OpenWalnutHelp.html" );
    std::string content = readFileIntoString( filename );

    QWidget* window = new QWidget( this, Qt::Window );

    // specify intial layout
    QVBoxLayout *layout = new QVBoxLayout( window );
    window->setLayout( layout );
    window->resize( 500, 500 );

    window->show();

    QWebView *view = new QWebView( this );
    QString location( QString( "file://" ) + WPathHelper::getAppPath().file_string().c_str() + "/../share/OpenWalnut/" );
    view->setHtml( content.c_str(), QUrl( location  ) );
    view->show();
    layout->addWidget( view );
}

void WMainWindow::setPresetViewLeft()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0.5, -0.5, -0.5, 0.5 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewRight()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( -0.5, -0.5, -0.5, -0.5 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewSuperior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., 0., 0., 1 );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewInferior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., -1., 0., 0. );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewAnterior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0., -0.707107, -0.707107, 0. );
    cm->setRotation( q );
}

void WMainWindow::setPresetViewPosterior()
{
    boost::shared_ptr< WGEViewer > viewer;
    viewer = WKernel::getRunningKernel()->getGraphicsEngine()->getViewerByName( "main" );
    osg::ref_ptr<osgGA::TrackballManipulator>  cm = osg::dynamic_pointer_cast<osgGA::TrackballManipulator>( viewer->getCameraManipulator() );
    osg::Quat q( 0.707107, 0., 0., 0.707107 );
    cm->setRotation( q );
}

void WMainWindow::openNotImplementedDialog()
{
    QMessageBox::information( this, "Not yet implemented!",
                              "This functionality is planned for future versions of OpenWalnut. "
                              "It is not yet implemented." );
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
    if( reallyClose )
    {
        saveWindowState();

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
                new WQtCustomDockWidget( title, m_glDock, ocdwEvent->getProjectionMode() ) );
            // m_glDock->addDockWidget( Qt::BottomDockWidgetArea, widget.get() );

            // restore state and geometry
            m_glDock->restoreDockWidget( widget.get() );

            // store it in CustomDockWidget list
            m_customDockWidgets.insert( make_pair( title, widget ) );
        }
        else
        {
            widget = m_customDockWidgets[title];
            widget->increaseUseCount();
        }

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
    if( event->type() == WQT_READY_EVENT )
    {
        // convert event to ready event
        WModuleReadyEvent* e1 = dynamic_cast< WModuleReadyEvent* >( event );     // NOLINT
        if( e1 )
        {
            moduleSpecificSetup( e1->getModule() );
        }
    }

    if( event->type() == WQT_CRASH_EVENT )
    {
        // convert event to ready event
        WModuleCrashEvent* e1 = dynamic_cast< WModuleCrashEvent* >( event );     // NOLINT
        if( e1 )
        {
            QString title = "Problem in module: " + QString::fromStdString( e1->getModule()->getName() );
            QString description = "<b>Module Problem</b><br/><br/><b>Module:  </b>" + QString::fromStdString( e1->getModule()->getName() );

            QString message = QString::fromStdString( e1->getMessage() );
            QMessageBox msgBox;
            msgBox.setText( description );
            msgBox.setInformativeText( message  );
            msgBox.setStandardButtons( QMessageBox::Ok );
            msgBox.exec();
        }
    }

    if( event->type() == WQT_MODULE_REMOVE_EVENT )
    {
        // convert event to ready event
        WModuleRemovedEvent* e1 = dynamic_cast< WModuleRemovedEvent* >( event );     // NOLINT
        if( e1 )
        {
            moduleSpecificCleanup( e1->getModule() );
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
    WPosition crossHairPos = WKernel::getRunningKernel()->getSelectionManager()->getCrosshair()->getPosition();
    WPosition minROIPos = crossHairPos - WPosition( 10., 10., 10. );
    WPosition maxROIPos = crossHairPos + WPosition( 10., 10., 10. );

    if( m_controlPanel->getFirstRoiInSelectedBranch().get() == NULL )
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi );
    }
    else
    {
        osg::ref_ptr< WROIBox > newRoi = osg::ref_ptr< WROIBox >( new WROIBox( minROIPos, maxROIPos ) );
        WKernel::getRunningKernel()->getRoiManager()->addRoi( newRoi, m_controlPanel->getFirstRoiInSelectedBranch() );
    }
}

void WMainWindow::openConfigDialog()
{
    // TODO(all): we need a nice dialog box here.
    QString msg = "OpenWalnut allows you to configure several features. Most of these options are only useful to advanced users. "
                  "You can have a user-scope configuration in your HOME directory as \".walnut.cfg\". "
                  "If this file exists, OpenWalnut loads this file. You can also specify a \"walnut.cfg\" in your OpenWalnut directory under "
                  "\"share/OpenWalnut/\". A default file will be there after installation. The default file is very well documented.";
    QMessageBox::information( this, "OpenWalnut - Configuration", msg );
}

void WMainWindow::restoreSavedState()
{
    // should we do it?
    bool saveStateEnabled = true;
    WPreferences::getPreference( "qt4gui.saveState", &saveStateEnabled );
    if ( !saveStateEnabled )
    {
        return;
    }

    // the state name postfix allows especially developers to have multiple OW with different GUI settings.
    std::string stateName = "";
    if ( WPreferences::getPreference( "qt4gui.stateNamePostfix", &stateName ) )
    {
        stateName = "-" + stateName;
    }
    stateName = "OpenWalnut" + stateName;
    wlog::info( "MainWindow" ) << "Restoring window state from \"" << stateName << "\"";

    QSettings setting( "OpenWalnut.org", QString::fromStdString( stateName ) );
    restoreGeometry( setting.value( "MainWindowGeometry", "" ).toByteArray() );
    restoreState( setting.value( "MainWindowState", "" ).toByteArray() );

    m_glDock->restoreGeometry( setting.value( "GLDockWindowGeometry", "" ).toByteArray() );
    m_glDock->restoreState( setting.value( "GLDockWindowState", "" ).toByteArray() );
}

void WMainWindow::saveWindowState()
{
    // should we do it?
    bool saveStateEnabled = true;
    WPreferences::getPreference( "qt4gui.saveState", &saveStateEnabled );
    if ( !saveStateEnabled )
    {
        return;
    }

    std::string stateName = "";
    if ( WPreferences::getPreference( "qt4gui.stateNamePostfix", &stateName ) )
    {
        stateName = "-" + stateName;
    }
    stateName = "OpenWalnut" + stateName;
    wlog::info( "MainWindow" ) << "Saving window state for \"" << stateName << "\"";

    // this saves the window state to some common location on the target OS in user scope.
    QSettings setting( "OpenWalnut.org", QString::fromStdString( stateName ) );
    setting.setValue( "MainWindowState", saveState() );
    setting.setValue( "GLDockWindowState", m_glDock->saveState() );

    // NOTE: Qt Doc says that saveState also saves geometry. But this somehow is wrong (at least for 4.6.3)
    setting.setValue( "MainWindowGeometry", saveGeometry() );
    setting.setValue( "GLDockWindowGeometry", m_glDock->saveGeometry() );
}

void WMainWindow::forceMainGLWidgetSize( size_t w, size_t h )
{
    m_mainGLWidget->setFixedSize( w, h );
}

void WMainWindow::restoreMainGLWidgetSize()
{
    m_mainGLWidget->setMinimumHeight( 250 );
    m_mainGLWidget->setMaximumHeight( QWIDGETSIZE_MAX );
    m_mainGLWidget->setMinimumWidth( 250 );
    m_mainGLWidget->setMaximumWidth( QWIDGETSIZE_MAX );
}

