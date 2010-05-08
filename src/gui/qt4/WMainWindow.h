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

#include <list>
#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include <boost/signals2/signal.hpp>

#include <QtGui/QIcon>
#include <QtGui/QMainWindow>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include <QtGui/QCloseEvent>

#include "WQtNavGLWidget.h"
#include "WQtConfigWidget.h"
#include "ribbonMenu/WQtRibbonMenu.h"
#include "WQtCustomDockWidget.h"
#include "WQtToolBar.h"

#include "WIconManager.h"
#include "datasetbrowser/WQtDatasetBrowser.h"

#include "../../kernel/WModule.h"
#include "../../common/WProjectFileIO.h"

// forward declarations
class QMenuBar;
class WQtGLWidget;

/**
 * This class contains the main window and the layout
 * of the widgets within the window.
 * \ingroup gui
 */
class WMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * Constructor of the main window
     */
    explicit WMainWindow();

    /**
     * Set up all widgets menus an buttons in the main window.
     */
    void setupGUI();

    /**
     * returns a pointer to the dataset browser object
     */
    WQtDatasetBrowser* getDatasetBrowser();

    /**
     *  returns a pointer to the ribbon menu object
     */
    WQtRibbonMenu* getRibbonMenu();

    /**
     *  returns a pointer to the tool bar showing the compatible modules
     */
    WQtToolBar* getCompatiblesToolBar();

    /**
     * Return icon manager
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
     * Close one of the custom dock widget saved in the map of customDockWidgets
     *
     * \param title the title of the widget to close
     */
    void closeCustomDockWidget( std::string title );

protected:

    /**
     * Setup the GUI by handling special modules. NavSlices for example setup several toolbar buttons.
     *
     * \param module the module to setup the GUI for.
     */
    void moduleSpecificSetup( boost::shared_ptr< WModule > module );

    /**
     * We want to react on close events.
     *
     * \param e the close event.
     */
    void closeEvent( QCloseEvent* e );

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
     * Sets that a fiber data set has already been loaded. Thi shelps to prevent multiple fiber data sets to be loaded.
     */
    void setFibersLoaded();

    /**
    * gets called when menu option or toolbar button load is activated
    */
    void openConfigDialog();
private:
    /**
     * Sets up the permanent tool bar.
     */
    void setupPermanentToolBar();

    /**
     * Sets up the initial state of the tool bar showing the compatible modules
     */
    void setupCompatiblesToolBar();

    WIconManager m_iconManager; //!< manager to provide icons in the gui thread

    QMenuBar* m_menuBar; //!< The main menu bar of the GUI.

    QWidget* m_centralwidget; //!< the central widget of the docking facility. This can not be moved.

    WQtToolBar* m_permanentToolBar; //!< The permanent toolbar of the main window.

    WQtToolBar* m_compatiblesToolBar; //!< This toolbar shows the compatible modules if a module is selected in the dataset browser

    WQtDatasetBrowser* m_datasetBrowser; //!< dataset browser

    boost::shared_ptr<WQtGLWidget> m_mainGLWidget; //!< the main GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navAxial; //!< the axial view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navCoronal; //!< the coronal view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navSagittal; //!< the sgittal view widget GL widget of the GUI
    QDockWidget* m_dummyWidget; //!< The dummywidget serves as spacer in the dockwidget area;

    /**
    * shared pointer for the configuration widget
    */
    boost::shared_ptr< WQtConfigWidget > m_configWidget;

    bool m_fibLoaded; //!< Indicates whether a fiber data set is already loaded.

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
     */
    void autoAdd( boost::shared_ptr< WModule > module, std::string proto );
};

#endif  // WMAINWINDOW_H
