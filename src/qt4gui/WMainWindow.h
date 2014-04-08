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

#ifndef WMAINWINDOW_H
#define WMAINWINDOW_H

#include <map>
#include <string>
#include <vector>

#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>

#include <QtGui/QMainWindow>
#include <QtGui/QSplashScreen>
#include <QtCore/QSettings>

#include "WIconManager.h"
#include "WQtToolBar.h"
#include "WQtGLWidget.h"
#include "WSettingAction.h"
#include "networkEditor/WQtNetworkEditor.h"
#include "WQtMessageDock.h"

// forward declarations
class QMenuBar;
class WModule;
class WProjectFileIO;
class WQtCombinerToolbar;
class WQtControlPanel;
class WQtCustomDockWidget;
class WQtNavGLWidget;
class WQtGLDockWidget;
class WQtPropertyBoolAction;
class WPropertyBase;
class WQtControlPanel;
class WQtGLScreenCapture;
class WQtWidgetBase;

/**
 * This class contains the main window and the layout of the widgets within the window.
 * \ingroup ui
 */
class WMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor of the main window.
     *
     * \param splash the splash screen currently show. This windows later handles its close stuff.
     */
    explicit WMainWindow( QSplashScreen* splash );

    /**
     * Destructor. Stores window state.
     */
    virtual ~WMainWindow();

    /**
     * Set up all widgets menus an buttons in the main window.
     */
    void setupGUI();

    /**
     * Returns a pointer to the network editor object.
     *
     * \return the network editor instance
     */
    WQtNetworkEditor* getNetworkEditor();

    /**
     * Returns a pointer to the control panel object.
     *
     * \return the control panel instance
     */
    WQtControlPanel* getControlPanel();

    /**
     * Return icon manager
     *
     * \return the icon manager instance
     */
    WIconManager* getIconManager();

    /**
     * Returns a pointer to a signal object which signals a list of filenames to load
     *
     * \return the signal
     */
    boost::signals2::signal1< void, std::vector< std::string > >* getLoaderSignal();

    /**
     * Searches for a custom dock widget with a given name and returns it, if
     * found.
     *
     * \param name the name of the widget
     * \returns a shared pointer to the widget or NULL if not found
     */
    boost::shared_ptr< WQtCustomDockWidget > getCustomDockWidget( std::string name );

    /**
     * Close one of the custom dock widget saved in the map of customDockWidgets. This method is thread-safe and ensures that the widget is
     * closed in the GUI thread. NEVER call this in the GUI thread. It will block the GUI.
     *
     * \note the widget might not be closed after this call. The widget is usage counted.
     *
     * \param title the title of the widget to close
     */
    void closeCustomDockWidget( std::string title );

    /**
     * This method removes the old compatibles toolbar and sets the specified one.
     *
     * \param toolbar the toolbar to set. If NULL, the toolbar gets reset.
     */
    void setCompatiblesToolbar( WQtCombinerToolbar* toolbar = NULL );

    /**
     * This method returns the a pointer to the current compatibles toolbar.
     *
     * \return a pointer to the current compatibles toolbar.
     */
    WQtCombinerToolbar* getCompatiblesToolbar();

    /**
     * Returns the settings object.
     *
     * \return settings object.
     */
    static QSettings& getSettings();

    /**
     * Set a setting specified by a key to a given value. Abbreviation for the lazy programmer as it ensures proper typecast of std::string to
     * the QSettings types (QString and QVariant).
     *
     * \param key the key
     * \param value the value
     */
    static void setSetting( std::string key, std::string value );

    /**
     * Get the current splash screen
     *
     * \return the splash screen. Might be closed.
     */
    QSplashScreen* getSplash() const;

    /**
     * Loads a given project asynchronously.
     *
     * \param filename the file to load.
     */
    void asyncProjectLoad( std::string filename );

    /**
     * This method checks whether a given drop event is acceptable.
     * Several widgets in the GUI should support drag and drop. Unfortunately, not all widgets automatically push these events to the MainWindow.
     * This is especially the case for QGraphics* based classes.
     *
     * \param mimeData the mime info of the dragged thing
     *
     * \return true if acceptable.
     */
    static bool isDropAcceptable( const QMimeData* mimeData );

    /**
     * Add the default OW menu to the given widget using addAction.
     *
     * \param widget the widget to add the menu to
     */
    void addGlobalMenu( QWidget* widget );

    /**
     * Allows comfortable error reporting using popups.
     *
     * \param parent the parent widget for this popup
     * \param title the title
     * \param message the message
     */
    void reportError( QWidget* parent, QString title, QString message );

    /**
     * Allows comfortable warning reporting using popups.
     *
     * \param parent the parent widget for this popup
     * \param title the title
     * \param message the message
     */
    void reportWarning( QWidget* parent, QString title, QString message );

    /**
     * Allows comfortable info reporting using popups.
     *
     * \param parent the parent widget for this popup
     * \param title the title
     * \param message the message
     */
    void reportInfo( QWidget* parent, QString title, QString message );

    /**
     * Return the message dock widget
     *
     * \return the message dock
     */
    const WQtMessageDock* getMessageDock() const;

    /**
     * Return the message dock widget
     *
     * \return the message dock
     */
    WQtMessageDock* getMessageDock();

    /**
     * Register a custom widget. This is important as the main window needs to manage the close/delete of these widgets. Only call from withing
     * the GUI thread.
     *
     * \param widget the widget.
     */
    void registerCustomWidget( WQtWidgetBase* widget );

    /**
     * De-register a custom widget.
     *
     * \param widget the widget.
     */
    void deregisterCustomWidget( WQtWidgetBase* widget );

protected:
    /**
     * Setup the GUI by handling special modules. NavSlices for example setup several toolbar buttons.
     *
     * \param module the module to setup the GUI for.
     */
    void moduleSpecificSetup( boost::shared_ptr< WModule > module );
    /**
     * Cleanup the GUI by handling special modules. NavSlices for example remove several toolbar buttons.
     *
     * \param module the module to setup the GUI for.
     */
    void moduleSpecificCleanup( boost::shared_ptr< WModule > module );

    /**
     * We want to react on close events.
     *
     * \param e the close event.
     */
    virtual void closeEvent( QCloseEvent* e );

    /**
     * Handle custom events.
     * Currently only WOpenCustomDockWidgetEvent.
     *
     * \param event the custom event
     */
    virtual void customEvent( QEvent* event );

    /**
     * Custom event dispatcher. Gets called by QT's Event system every time an event got sent to this widget. This event handler
     * processes several custom events, like WModuleAssocEvent.
     *
     * \note QT Doc says: use event() for custom events.
     * \param event the event that got transmitted.
     *
     * \return true if the event got handled properly.
     */
    virtual bool event( QEvent* event );

    /**
     * Called for each project save request.
     *
     * \param writer the list of writers to use.
     *
     * \returns true if saving was successful
     */
    virtual bool projectSave( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer );

    /**
     * drag and drop implementation for loading files
     * \param event the event to handle
     */
    void dropEvent( QDropEvent* event );

    /**
     * drag and drop implementation for loading files
     * \param event the event to handle
     */
     void dragMoveEvent( QDragMoveEvent* event );

    /**
     * drag and drop implementation for loading files
     * \param event the event to handle
     */
     void dragEnterEvent( QDragEnterEvent* event );

public slots:
    /**
     * Slot handling the pressed new project button.
     */
    void newProject();
    /**
     * gets called when menu option or toolbar button load is activated
     */
    void openLoadDialog();

    /**
     * gets called when menu entry "About OpenWalnut" is activated
     */
    void openAboutDialog();

    /**
     * Gets called when menu entry "About Qt" is activated
     */
    void openAboutQtDialog();

    /**
     * Gets called when menu entry "OpenWalnut Help" is activated
     */
    void openOpenWalnutHelpDialog();

    /**
     * Gets called when a menu entry that has no functionality yet is activated.
     */
    void openNotImplementedDialog();

    /**
     * gets called when the button new ROI is pressed
     */
    void newRoi();

    /**
     * Gets called whenever the user presses the project save button.
     */
    bool projectSaveAll();

    /**
     * Gets called by the save menu to only save the camera settings
     */
    bool projectSaveCameraOnly();

    /**
     * Gets called by the save menu to only save the ROI settings
     */
    bool projectSaveROIOnly();

    /**
     * Gets called by the save menu to only save the Module settings
     */
    bool projectSaveModuleOnly();

    /**
     * Is able to handle updates in the log-level setting.
     *
     * \param logLevel the new loglevel to set
     */
    void handleLogLevelUpdate( unsigned int logLevel );

    /**
     * Handles the given drop. Use this in conjunction with \ref isDropAcceptable.
     *
     * \param event the event to handle
     */
    void handleDrop( QDropEvent* event );

private:
    /**
     * The splash screen object opened on startup.
     */
    QSplashScreen* m_splash;

    /**
     * The currently set compatibles toolbar
     */
    WQtCombinerToolbar* m_currentCompatiblesToolbar;

    WIconManager m_iconManager; //!< manager to provide icons in the gui thread

    QMenuBar* m_menuBar; //!< The main menu bar of the GUI.

    WQtToolBar* m_permanentToolBar; //!< The permanent toolbar of the main window.

    /**
     * Message and log window
     */
    WQtMessageDock* m_messageDock;

    // several menu items
    QAction* m_newAction; //!< the new project button
    QAction* m_loadButton; //!< the load button
    QAction* m_quitAction; //!< quit OpenWalnut
    QAction* m_saveAction; //!< the save menu
    QMenu* m_saveMenu; //!< the menu using m_saveAction

    QMenu* m_viewMenu; //!< view and camera control menu

    QAction* m_settingsAction; //!< the settings
    QMenu* m_settingsMenu; //!< the settings

    QAction* m_helpAction; //!< the help menu
    QMenu* m_helpMenu; //!< the help menu

    WQtControlPanel* m_controlPanel; //!< control panel

    WQtNetworkEditor* m_networkEditor; //!< network editor

    WQtGLWidget* m_mainGLWidget; //!< the main GL widget of the GUI

    boost::shared_ptr< WQtNavGLWidget > m_navAxial; //!< the axial view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navCoronal; //!< the coronal view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navSagittal; //!< the sgittal view widget GL widget of the GUI
    QMainWindow* m_glDock;  //!< the dock that is used for gl widgets
    QDockWidget* m_dummyWidget; //!< The dummywidget serves as spacer in the dockwidget area;
    WQtGLDockWidget* m_mainGLDock; //!< the dock containing the main gl widget

    /**
     * Container for core/UI widgetd
     */
    typedef std::vector< WQtWidgetBase* > CustomWidgets;

    /**
     * All registered widgets created by the core/UI api.
     */
    CustomWidgets m_customWidgets;

    boost::signals2::signal1< void, std::vector< std::string > > m_loaderSignal; //!< boost signal for open file dialog

    /**
     * Combine a module with a prototype only if the module is available. Else, nothing happens.
     *
     * \param module the module to be combined.
     * \param proto the prototype to combine with the module.
     * \param onlyOnce if true, it is ensured that only one module is in the container.
     */
    void autoAdd( boost::shared_ptr< WModule > module, std::string proto, bool onlyOnce = false );

    /**
     * Loads the window states and geometries from a file.
     */
    void restoreSavedState();

    /**
     * Saves the current window states and geometries to a file.
     */
    void saveWindowState();

    /**
     * The action controlling the auto-display feature.
     */
    WSettingAction* m_autoDisplaySetting;

    /**
     * The action for allowing editing min and max of sliders.
     */
    WSettingAction* m_sliderMinMaxEditSetting;

    /**
     * Called whenever a async load has finished. Used by \ref asyncProjectLoad. It might be called from outside the GUI thread.
     *
     * \param file the filename
     * \param errors the list of errors
     * \param warnings the list of warnings
     */
    void slotLoadFinished( boost::filesystem::path file, std::vector< std::string > errors, std::vector< std::string > warnings );

private slots:
    /**
     * Handles some special GL vendors and shows the user a dialog.
     */
    void handleGLVendor();

    /**
     * Shows startup info messages
     */
    void handleStartMessages();

    /**
     * Shows the welcome message.
     *
     * \param force if true, the dialog is shown even if the user said "do not show again".
     */
    void showWelcomeDialog( bool force = true );

    /**
     * Finally closes the splash screen.
     */
    void closeSplash();
};

#endif  // WMAINWINDOW_H
