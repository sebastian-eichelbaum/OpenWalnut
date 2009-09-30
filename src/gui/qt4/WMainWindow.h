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

#include <boost/shared_ptr.hpp>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>

#include <QtGui/QSlider>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

#include "../WGUI.h"

// forward declarations
class WQtGLWidget;
class WQtDatasetBrowser;
class WQtRibbonMenu;

/**
 * This class contains the main window and the layout
 * of the widgets within the window.
 * \ingroup gui
 */
class WMainWindow : public QObject, public WGUI
{
    Q_OBJECT

public:

    /**
     * Set up all widgets menus an buttons in the main window.
     */
    void setupGUI( QMainWindow *MainWindow );

    /**
     * Destructor.
     */
    virtual ~WMainWindow();

    /**
     *
     */
    WQtDatasetBrowser* getDatasetBrowser();

private:
    /**
     * Helper routine for adding new docks with GL content
     */
    QSlider* addNavigationGLWidget( QMainWindow *MainWindow, QString title );


    /**
     * Connects some signals with some slots
     */
    void connectSlots( QMainWindow *MainWindow );

    QIcon m_mainWindowIcon;

    QWidget* m_centralwidget;
    QStatusBar* m_statusBar;
    WQtRibbonMenu* m_toolBar;

    std::list<boost::shared_ptr<WQtGLWidget> > m_glWidgets;
    WQtDatasetBrowser* m_datasetBrowser;


private slots:
    /**
     * gets called when menu option or toolbar button load is activated
     */
    void load();
};

#endif  // WMAINWINDOW_H
