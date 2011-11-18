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

//#include <boost/program_options.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>

#include <QtGui/QMainWindow>
#include <QtCore/QSettings>

#include "WIconManager.h"
#include "WQtToolBar.h"
#include "WQtGLWidget.h"
#include "WSettingAction.h"
#include "networkEditor/WQtNetworkEditor.h"
#include "commandPrompt/WQtCommandPromptToolbar.h"

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

/**
 * This class contains the main window and the layout of the widgets within the window.
 * \ingroup gui
 */
class WMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor of the main window
     */
    WMainWindow();

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
     * Forces the main gl widget to have the desired size. This is mainly useful for screenshots and similar.
     *
     * \param w width
     * \param h height
     */
    void forceMainGLWidgetSize( size_t w, size_t h );

    /**
     * Restores the main GL widget size if it was fixed with forceMainGLWidgetSize() previously.
     */
    void restoreMainGLWidgetSize();

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
     */
    virtual void projectSave( const std::vector< boost::shared_ptr< WProjectFileIO > >& writer );

public slots:
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
     * Sets the left preset view of the main viewer.
     */
    void setPresetViewLeft();

    /**
     * Sets the right preset view of the main viewer.
     */
    void setPresetViewRight();

    /**
     * Sets the superior preset view of the main viewer.
     */
    void setPresetViewSuperior();

    /**
     * Sets the inferior preset view of the main viewer.
     */
    void setPresetViewInferior();

    /**
     * Sets the anterior preset view of the main viewer.
     */
    void setPresetViewAnterior();

    /**
     * Sets the posterior preset view of the main viewer.
     */
    void setPresetViewPosterior();

    /**
     * Gets called when a menu entry that has no functionality yet is activated.
     */
    void openNotImplementedDialog();

    /**
     * gets called when the button new roi is pressed
     */
    void newRoi();

    /**
     * Gets called whenever the user presses the project button.
     */
    void projectLoad();

    /**
     * Gets called whenever the user presses the project save button.
     */
    void projectSaveAll();

    /**
     * Gets called by the save menu to only save the camera settings
     */
    void projectSaveCameraOnly();

    /**
     * Gets called by the save menu to only save the ROI settings
     */
    void projectSaveROIOnly();

    /**
     * Gets called by the save menu to only save the Module settings
     */
    void projectSaveModuleOnly();

    /**
     * Is able to handle updates in the log-level setting.
     *
     * \param logLevel the new loglevel to set
     */
    void handleLogLevelUpdate( unsigned int logLevel );

private:
    /**
     * The currently set compatibles toolbar
     */
    WQtCombinerToolbar* m_currentCompatiblesToolbar;

    WIconManager m_iconManager; //!< manager to provide icons in the gui thread

    QMenuBar* m_menuBar; //!< The main menu bar of the GUI.

    WQtToolBar* m_permanentToolBar; //!< The permanent toolbar of the main window.

    QAction* m_loadButton; //!< the load Data Button

    WQtControlPanel* m_controlPanel; //!< control panel

    WQtNetworkEditor* m_networkEditor; //!< network editor

    WQtCommandPromptToolbar* m_commandPrompt; //!< command prompt

    boost::shared_ptr< WQtGLWidget > m_mainGLWidget; //!< the main GL widget of the GUI
    WQtGLScreenCapture* m_mainGLWidgetScreenCapture; //!< screen recorder in m_mainGLWidget

    boost::shared_ptr< WQtNavGLWidget > m_navAxial; //!< the axial view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navCoronal; //!< the coronal view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navSagittal; //!< the sgittal view widget GL widget of the GUI
    QMainWindow* m_glDock;  //!< the dock that is used for gl widgets
    QDockWidget* m_dummyWidget; //!< The dummywidget serves as spacer in the dockwidget area;

    /**
     * All registered WQtCustomDockWidgets.
     */
    std::map< std::string, boost::shared_ptr< WQtCustomDockWidget > > m_customDockWidgets;

    /**
     * Mutex used to lock the map of WQtCustomDockWidgets.
     */
    boost::mutex m_customDockWidgetsLock;

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

private slots:
    /**
     * Handles some special GL vendors and shows the user a dialog.
     */
    void handleGLVendor();

    /**
     * Shows startup info messages
     */
    void handleStartMessages();
};

#endif  // WMAINWINDOW_H
