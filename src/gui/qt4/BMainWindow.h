//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------
#ifndef BMAINWINDOW_H
#define BMAINWINDOW_H

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

#include "../icons/icons.h"
/**
 * This class contains the main window and the layout
 * of the widgets within the window.
 */
class BMainWindow
{
public:
    /**
     * Set up all widgets menus an buttons in the main window.
     */
    void setupGUI( QMainWindow *MainWindow );
private:
    QAction* m_actionLoad;
    QAction* m_actionSave;
    QAction* m_actionPreferences;
    QAction* m_actionExit;
    QAction* m_actionAbout_BrainCognize;
    QWidget* m_centralwidget;
    QMenuBar* m_menubar;
    QMenu* m_menuFile;
    QMenu* m_menuHelp;
    //  QStatusBar* m_statusBar;
    QToolBar* m_toolBar;
};

#endif // BMAINWINDOW_H
