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
#include <QtGui/QInputDialog>

#include "core/common/WColor.h"
#include "core/common/WIOTools.h"
#include "core/common/WProjectFileIO.h"
#include "core/common/WPathHelper.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetSingle.h"
#include "core/dataHandler/WEEG2.h"
#include "core/graphicsEngine/WGEZoomTrackballManipulator.h"
#include "core/graphicsEngine/WROIBox.h"
#include "core/kernel/modules/data/WMData.h"
#include "core/kernel/WKernel.h"
#include "core/kernel/WModule.h"
#include "core/kernel/WModuleCombiner.h"
#include "core/kernel/WModuleCombinerTypes.h"
#include "core/kernel/WProjectFile.h"
#include "core/kernel/WROIManager.h"
#include "core/kernel/WSelectionManager.h"
#include "../icons/WIcons.xpm"
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
#include "WQt4Gui.h"
#include "WSettingAction.h"
#include "WSettingMenu.h"

#include "WMainWindow.h"
#include "WMainWindow.moc"

WMainWindow::WMainWindow():
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
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Setting setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    WSettingAction* hideMenuAction = new WSettingAction( this, "qt4gui/showMenu",
                                                               "Show Menubar",
                                                               "Allows you to hide the menu. Can be restored using CTRL-M.",
                                                               true,
                                                               false,
                                                               QKeySequence( Qt::CTRL + Qt::Key_M ) );

    WSettingAction* showNavWidgets = new WSettingAction( this, "qt4gui/showNavigationWidgets",
                                                               "Show Navigation Views",
                                                               "Disables the navigation views completely. This can lead to a speed-up and is "
                                                               "recommended for those who do not need them.",
                                                               true,
                                                               true    // this requires a restart
                                                       );
    m_autoDisplaySetting = new WSettingAction( this, "qt4gui/useAutoDisplay",
                                                     "Auto-Display",
                                                     "If enabled, the best matching module is automatically added if some data was loaded.",
                                                     true );

    WSettingAction* mtViews = new WSettingAction( this, "qt4gui/ge/multiThreadedViewer",
                                                        "Multi-threaded Views",
                                                        "If enabled, the graphic windows are rendered in different threads. This can speed-up "
                                                        "rendering on machines with multiple cores.",
                                                        false,
                                                        true // require restart
                                                );
    // NOTE: the multi-threading feature needs to be activated BEFORE the first viewer is created. To ensure this we do it here.
    WGraphicsEngine::getGraphicsEngine()->setMultiThreadedViews( mtViews->get() );

    // set the log-level setting.
    // NOTE: see WQt4Gui which reads the setting.
    QList< QString > logOptions;
    logOptions.push_back( "Debug" );
    logOptions.push_back( "Info" );
    logOptions.push_back( "Warning" );
    logOptions.push_back( "Error" );
    WSettingMenu* logLevels = new WSettingMenu( this, "qt4gui/logLevel",
                                                      "Log-Level",
                                                      "Allows to set the log verbosity.",
                                                      1,    // info is the default
                                                      logOptions
                                              );
    connect( logLevels, SIGNAL( change( unsigned int ) ), this, SLOT( handleLogLevelUpdate( unsigned int ) ) );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GUI setup
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    m_iconManager.addIcon( std::string( "missingModule" ), QuestionMarks_xpm );
    m_iconManager.addIcon( std::string( "none" ), empty_xpm );
    m_iconManager.addIcon( std::string( "DefaultModuleIcon" ), moduleDefault_xpm );

    if( objectName().isEmpty() )
    {
        setObjectName( QString::fromUtf8( "MainWindow" ) );
    }

    // NOTE: this only is an initial size. The state reloaded from QSettings will set it to the value the user had last session.
    resize( 800, 600 );
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
    tabifyDockWidget( m_controlPanel->getColormapperDock(), m_controlPanel->getRoiDock() );

    addDockWidget( Qt::RightDockWidgetArea, m_controlPanel );

    m_glDock = new QMainWindow();
    m_glDock->setObjectName( "GLDock" );
    m_glDock->setDockOptions( QMainWindow::AnimatedDocks |  QMainWindow::AllowNestedDocks | QMainWindow::AllowTabbedDocks );
    m_glDock->setDocumentMode( true );
    setCentralWidget( m_glDock );
    WQtGLDockWidget* mainGLDock = new WQtGLDockWidget( "main", "3D View", m_glDock );
    mainGLDock->getGLWidget()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_mainGLWidget = mainGLDock->getGLWidget();
    m_glDock->addDockWidget( Qt::RightDockWidgetArea, mainGLDock );

    m_permanentToolBar = new WQtToolBar( "Standard Toolbar", this );
    addToolBar( Qt::TopToolBarArea, m_permanentToolBar );

    m_iconManager.addIcon( std::string( "ROI icon" ), box_xpm );
    m_iconManager.addIcon( std::string( "Reset icon" ), o_xpm );
    m_iconManager.addIcon( std::string( "axial icon" ), axial_xpm );
    m_iconManager.addIcon( std::string( "coronal icon" ), cor_xpm );
    m_iconManager.addIcon( std::string( "sagittal icon" ), sag_xpm );

    m_loadButton = new QAction( m_iconManager.getIcon( "load" ), "Load Dataset", m_permanentToolBar );
    m_loadButton->setShortcut( QKeySequence(  QKeySequence::Open ) );
    QAction* roiButton = new QAction( m_iconManager.getIcon( "ROI icon" ), "ROI", m_permanentToolBar );
    QAction* resetButton = new QAction( m_iconManager.getIcon( "view" ), "Reset", m_permanentToolBar );
    resetButton->setShortcut( QKeySequence( Qt::Key_Escape ) );
    QAction* projectLoadButton = new QAction( m_iconManager.getIcon( "loadProject" ), "Load Project", m_permanentToolBar );
    QAction* projectSaveButton = new QAction( m_iconManager.getIcon( "saveProject" ), "Save Project", m_permanentToolBar );
    projectLoadButton->setShortcut( QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_O ) );

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
    m_menuBar->setVisible( hideMenuAction->get() );
    connect( hideMenuAction, SIGNAL( change( bool ) ), m_menuBar, SLOT( setVisible( bool ) ) );
    addAction( hideMenuAction );

    QMenu* fileMenu = m_menuBar->addMenu( "File" );

    fileMenu->addAction( m_loadButton );
    fileMenu->addSeparator();
    fileMenu->addAction( projectLoadButton );
    QMenu* saveMenu = fileMenu->addMenu( m_iconManager.getIcon( "saveProject" ), "Save Project" );
    saveMenu->addAction( "Save Project", this, SLOT( projectSaveAll() ), QKeySequence::Save );
    saveMenu->addAction( "Save Modules Only", this, SLOT( projectSaveModuleOnly() ) );
    saveMenu->addAction( "Save Camera Only", this, SLOT( projectSaveCameraOnly() ) );
    saveMenu->addAction( "Save ROIs Only", this, SLOT( projectSaveROIOnly() ) );
    projectSaveButton->setMenu( saveMenu );

    // TODO(all): If all distributions provide a newer QT version we should use QKeySequence::Quit here
    //fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ), QKeySequence( QKeySequence::Quit ) );
    fileMenu->addAction( m_iconManager.getIcon( "quit" ), "Quit", this, SLOT( close() ),  QKeySequence( Qt::CTRL + Qt::Key_Q ) );

    // This QAction stuff is quite ugly and complicated some times ... There is no nice constructor which takes name, slot keysequence and so on
    // directly -> set shortcuts, and some further properties using QAction's interface
    QMenu* viewMenu = m_menuBar->addMenu( "View" );
    viewMenu->addAction( hideMenuAction );
    viewMenu->addSeparator();
    viewMenu->addAction( showNavWidgets );
    viewMenu->addSeparator();
    viewMenu->addMenu( m_permanentToolBar->getStyleMenu() );

    // Camera menu
    QMenu* bgColorMenu = new QMenu( "Background Colors" );
    bgColorMenu->addAction( mainGLDock->getGLWidget()->getBackgroundColorAction() );

    QMenu* cameraMenu = m_menuBar->addMenu( "Camera" );
    cameraMenu->addAction( mainGLDock->getGLWidget()->getThrowingSetting() );
    cameraMenu->addMenu( bgColorMenu );
    cameraMenu->addSeparator();

    QMenu* settingsMenu = m_menuBar->addMenu( "Settings" );
    settingsMenu->addAction( m_autoDisplaySetting );
    settingsMenu->addAction( m_controlPanel->getModuleExcluder().getConfigureAction() );
    settingsMenu->addSeparator();
    settingsMenu->addAction( mtViews );
    settingsMenu->addSeparator();
    settingsMenu->addMenu( logLevels );

    // a separate menu for some presets
    QMenu* cameraPresetMenu = cameraMenu->addMenu( "Presets" );

    QAction* controlPanelTrigger = m_controlPanel->toggleViewAction();
    QList< QKeySequence > controlPanelShortcut;
    controlPanelShortcut.append( QKeySequence( Qt::Key_F9 ) );
    controlPanelTrigger->setShortcuts( controlPanelShortcut );
    this->addAction( controlPanelTrigger );  // this enables the action even if the menu bar is invisible

    // NOTE: the shortcuts for these view presets should be chosen carefully. Most keysequences have another meaning in the most applications
    // so the user may get confused. It is also not a good idea to take letters as they might be used by OpenSceneGraph widget ( like "S" for
    // statistics ).
    // By additionally adding the action to the main window, we ensure the action can be triggered even if the menu bar is hidden.
    QAction* tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "sagittal icon" ), "Left", this, SLOT( setPresetViewLeft() ),
                                             QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_L ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "sagittal icon" ), "Right", this, SLOT( setPresetViewRight() ),
                                       QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_R ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "axial icon" ), "Superior", this, SLOT( setPresetViewSuperior() ),
                                       QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_S ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "axial icon" ), "Inferior", this, SLOT( setPresetViewInferior() ),
                                       QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "coronal icon" ), "Anterior", this, SLOT( setPresetViewAnterior() ),
                                       QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_A ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    tmpAction = cameraPresetMenu->addAction( m_iconManager.getIcon( "coronal icon" ), "Posterior", this, SLOT( setPresetViewPosterior() ),
                                       QKeySequence( Qt::CTRL + Qt::SHIFT + Qt::Key_P ) );
    tmpAction->setIconVisibleInMenu( true );
    this->addAction( tmpAction );

    resetButton->setMenu( cameraPresetMenu );

    QMenu* helpMenu = m_menuBar->addMenu( "Help" );
    helpMenu->addAction( m_iconManager.getIcon( "help" ), "OpenWalnut Help", this, SLOT( openOpenWalnutHelpDialog() ),
                         QKeySequence( QKeySequence::HelpContents ) );
    helpMenu->addSeparator();
    helpMenu->addAction( m_iconManager.getIcon( "logo" ), "About OpenWalnut", this, SLOT( openAboutDialog() ) );
    helpMenu->addAction( "About Qt", this, SLOT( openAboutQtDialog() ) );

    setMenuBar( m_menuBar );

    // initially 3 navigation views
    {
        if( showNavWidgets->get() )
        {
            m_navAxial = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Axial View", "Axial View", this, "Axial Slice",
                                                                                  m_mainGLWidget.get() ) );
            m_navAxial->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navAxial.get() );

            m_navCoronal = boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Coronal View", "Coronal View", this, "Coronal Slice",
                                                                                    m_mainGLWidget.get() ) );
            m_navCoronal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navCoronal.get() );

            m_navSagittal =
                boost::shared_ptr< WQtNavGLWidget >( new WQtNavGLWidget( "Sagittal View", "Sagittal View", this, "Sagittal Slice",
                                                                         m_mainGLWidget.get() ) );
            m_navSagittal->setFeatures( QDockWidget::AllDockWidgetFeatures );
            m_glDock->addDockWidget( Qt::LeftDockWidgetArea, m_navSagittal.get() );

            bgColorMenu->addAction( m_navAxial->getGLWidget()->getBackgroundColorAction() );
            bgColorMenu->addAction( m_navCoronal->getGLWidget()->getBackgroundColorAction() );
            bgColorMenu->addAction( m_navSagittal->getGLWidget()->getBackgroundColorAction() );
        }
    }

    // Temporarily disabled. We need a proper command prompt implementation first.
    // create command prompt toolbar
    // m_commandPrompt = new WQtCommandPromptToolbar( "Command Prompt", this );
    // addToolBar( Qt::TopToolBarArea, m_commandPrompt );
    // this->addAction( m_commandPrompt->toggleViewAction() );  // this enables the action even if the menu bar is invisible

    // setup permanent toolbar
    m_permanentToolBar->addAction( m_loadButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addAction( projectLoadButton );
    m_permanentToolBar->addAction( projectSaveButton );
    m_permanentToolBar->addSeparator();
    m_permanentToolBar->addAction( resetButton );
    m_permanentToolBar->addAction( roiButton );
    m_permanentToolBar->addSeparator();

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
    bool useAutoDisplay = m_autoDisplaySetting->get();
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
            if( !m_navSlicesAlreadyLoaded )
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

    // we want to keep the tool-button styles in sync
    m_currentCompatiblesToolbar->setToolButtonStyle( m_permanentToolBar->toolButtonStyle() );
    connect( m_permanentToolBar, SIGNAL( toolButtonStyleChanged( Qt::ToolButtonStyle ) ),
             m_currentCompatiblesToolbar, SLOT( setToolButtonStyle( Qt::ToolButtonStyle ) ) );

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
        if( filename.rfind( ".owp" ) == std::string::npos )
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
    std::string filename( WPathHelper::getDocPath().file_string() + "/qt4gui/OpenWalnutAbout.html" );
    std::string content = readFileIntoString( filename );
    QMessageBox::about( this, "About OpenWalnut", content.c_str() );
}

void WMainWindow::openOpenWalnutHelpDialog()
{
    std::string filename( WPathHelper::getDocPath().file_string() + "/qt4gui/OpenWalnutHelp.html" );
    std::string content = readFileIntoString( filename );

    QWidget* window = new QWidget( this, Qt::Window );

    // specify intial layout
    QVBoxLayout *layout = new QVBoxLayout( window );
    window->setLayout( layout );
    window->resize( 500, 500 );

    window->show();

    QWebView *view = new QWebView( this );
    QString location( QString( "file://" ) + WPathHelper::getDocPath().file_string().c_str() + "/qt4gui/" );
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

void WMainWindow::restoreSavedState()
{
    wlog::info( "MainWindow" ) << "Restoring window state.";

    restoreGeometry( WQt4Gui::getSettings().value( "MainWindowGeometry", "" ).toByteArray() );
    restoreState( WQt4Gui::getSettings().value( "MainWindowState", "" ).toByteArray() );

    m_glDock->restoreGeometry( WQt4Gui::getSettings().value( "GLDockWindowGeometry", "" ).toByteArray() );
    m_glDock->restoreState( WQt4Gui::getSettings().value( "GLDockWindowState", "" ).toByteArray() );
}

void WMainWindow::saveWindowState()
{
    wlog::info( "MainWindow" ) << "Saving window state.";

    // this saves the window state to some common location on the target OS in user scope.
    WQt4Gui::getSettings().setValue( "MainWindowState", saveState() );
    WQt4Gui::getSettings().setValue( "GLDockWindowState", m_glDock->saveState() );

    // NOTE: Qt Doc says that saveState also saves geometry. But this somehow is wrong (at least for 4.6.3)
    WQt4Gui::getSettings().setValue( "MainWindowGeometry", saveGeometry() );
    WQt4Gui::getSettings().setValue( "GLDockWindowGeometry", m_glDock->saveGeometry() );
}

QSettings& WMainWindow::getSettings()
{
    return WQt4Gui::getSettings();
}

void WMainWindow::handleLogLevelUpdate( unsigned int logLevel )
{
    WLogger::getLogger()->setDefaultLogLevel( static_cast< LogLevel >( logLevel ) );
}

