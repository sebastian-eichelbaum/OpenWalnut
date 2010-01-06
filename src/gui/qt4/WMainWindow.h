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
#include "ribbonMenu/WQtRibbonMenu.h"
#include "WQtCustomDockWidget.h"

#include "WIconManager.h"
#include "WPropertyManager.h"
#include "datasetbrowser/WQtDatasetBrowser.h"

#include "../../kernel/WModule.h"

// forward declarations
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
     * \param guiConfiguration configuration information taken from config file or command line
     */
    explicit WMainWindow( boost::program_options::variables_map guiConfiguration );

    /**
     * Set up all widgets menus an buttons in the main window.
     * \param guiConfiguration configuration information taken from config file or command line
     */
    void setupGUI( boost::program_options::variables_map guiConfiguration );

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
    WQtRibbonMenu* getCompatiblesToolBar();


    /**
     * Return property manager
     */
    WPropertyManager* getPropertyManager();

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
     * Returns the signal emitted by those module buttons in the compatibility tab.
     *
     * \return the signal.
     */
    boost::signals2::signal2< void, boost::shared_ptr< WModule >, boost::shared_ptr< WModule > >* getModuleButtonSignal();

    /**
     * Returns a point to a boost signal object which emits the coordinates of a mouseclick
     *
     * \return the signal
     */
    boost::signals2::signal1< void, std::string >* getPickSignal();

    /**
     * Close one of the custom dock widget saved in the map of customDockWidgets
     *
     * \param title the title of the widget to close
     */
    void closeCustomDockWidget( std::string title );

protected:

    /**
     * We want to react on close events.
     *
     * \param e the close event.
     */
    void closeEvent( QCloseEvent* e );

    /**
     * Handle custom events.
     * Currently only WCreateCustomDockWidgetEvent.
     *
     * \param event the custom event
     */
    virtual void customEvent( QEvent* event );

public slots:
    /**
     * gets called when menu option or toolbar button load is activated
     */
    void openLoadDialog();

    /**
     * get called when a module button
     * \param module name of teh module
     */
    void slotActivateModule( QString module );

private:
    /**
     * Sets up the initial state of the ribbon menu
     */
    void setupRibbonMenu();

    /**
     * Sets up the initial state of the tool bar showing the compatible modules
     */
    void setupCompatiblesToolBar();

    WIconManager m_iconManager; //!< manager to provide icons in the gui thread

    WPropertyManager m_propertyManager; //!< transmit gui interactions to connected modules

    QWidget* m_centralwidget; //!< the central widget of the docking facility. Thsi can not be moved.

    WQtRibbonMenu* m_ribbonMenu; //!< The ribbon menu of the main window.

    WQtRibbonMenu* m_compatiblesToolBar; //!< This toolbar shows the compatible modules if a module is selcted in the dataset browser

    WQtDatasetBrowser* m_datasetBrowser; //!< dataset browser

    boost::shared_ptr<WQtGLWidget> m_mainGLWidget; //!< the main GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navAxial; //!< the axial view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navCoronal; //!< the coronal view widget GL widget of the GUI
    boost::shared_ptr< WQtNavGLWidget > m_navSagittal; //!< the sgittal view widget GL widget of the GUI

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
     * boost signal object
     */
    boost::signals2::signal2< void, boost::shared_ptr< WModule >, boost::shared_ptr< WModule > > m_moduleButtonSignal;
};

#endif  // WMAINWINDOW_H
