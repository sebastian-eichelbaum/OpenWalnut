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

#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>
#include "boost/signals2/signal.hpp"

#include <QtGui/QIcon>
#include <QtGui/QMainWindow>
#include <QtGui/QSlider>
#include <QtGui/QWidget>
#include <QtGui/QCloseEvent>

#include "WQtNavGLWidget.h"
#include "ribbonMenu/WQtRibbonMenu.h"

#include "WIconManager.h"
#include "WPropertyManager.h"
#include "datasetbrowser/WQtDatasetBrowser.h"
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
    explicit WMainWindow( boost::program_options::variables_map guiConfiguration );

    /**
     * Set up all widgets menus an buttons in the main window.
     */
    void setupGUI( boost::program_options::variables_map guiConfiguration );

    /**
     * Destructor.
     */
    virtual ~WMainWindow();

    /**
     * returns a pointer to the dataset browser object
     */
    WQtDatasetBrowser* getDatasetBrowser();

    /**
     *  returns a pointer to the toolbar object
     */
    WQtRibbonMenu* getToolBar();


    /**
     * Return property manager
     */
    WPropertyManager* getPropertyManager();

    /**
     * Return icon manager
     */
    WIconManager* getIconManager();

    /**
     *
     */
    boost::signals2::signal1< void, std::vector< std::string > >* getLoaderSignal();

    /**
     *
     */
    boost::signals2::signal1< void, std::string >* getPickSignal();

protected:

    /**
     * We want to react on close events.
     *
     * \param e the close event.
     */
    void closeEvent( QCloseEvent* e );

public slots:
    /**
     * gets called when menu option or toolbar button load is activated
     */
    void openLoadDialog();

private:
    void setupToolBar();

    WIconManager m_iconManager;

    WPropertyManager m_propertyManager;

    QWidget* m_centralwidget;
    WQtRibbonMenu* m_toolBar;

    WQtDatasetBrowser* m_datasetBrowser;

    boost::shared_ptr<WQtGLWidget> m_mainGLWidget;
    boost::shared_ptr< WQtNavGLWidget > m_navAxial;
    boost::shared_ptr< WQtNavGLWidget > m_navCoronal;
    boost::shared_ptr< WQtNavGLWidget > m_navSagittal;

    boost::signals2::signal1< void, std::vector< std::string > > m_loaderSignal;
};

#endif  // WMAINWINDOW_H
