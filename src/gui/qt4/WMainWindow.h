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

#ifndef WMAINWINDOW_H
#define WMAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

#include "../WGUI.h"

// forward declarations
class WQtGLWidget;
class WQtPipelineBrowser;

/**
 * \ingroup gui
 * This class contains the main window and the layout
 * of the widgets within the window.
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
     * \brief
     * enable/disable the access to the GUI
     * \post
     * GUI enabled/disabeld
     * \param enable true iff gui should be enabled
     */
    void setEnabled( bool enable );

private:
    /**
     * Helper routine for adding new docks with GL content
     */
    void addDockableGLWidget( QMainWindow *MainWindow );

    QAction* m_actionLoad;
    QAction* m_actionSave;
    QAction* m_actionPreferences;
    QAction* m_actionExit;
    QAction* m_actionAbout_OpenWalnut;
    QWidget* m_centralwidget;
    QMenuBar* m_menubar;
    QMenu* m_menuFile;
    QMenu* m_menuHelp;
    QStatusBar* m_statusBar;
    QToolBar* m_toolBar;

    WQtGLWidget* m_glWidget;
    WQtPipelineBrowser* m_pipelineBrowser;


private slots:
    /**
     * gets called when menu option or toolbar button load is activated
     */
    void load();
};

#endif  // WMAINWINDOW_H
