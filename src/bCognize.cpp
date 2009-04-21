//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include <iostream>
#include <QtGui/QApplication>

#include "gui/qt4/BMainWindow.h"

/**
 * The main routine starting up the whole application.
 */
int main( int argc, char* argv[] )
{
    QApplication appl( argc, argv );
    QMainWindow *widget = new QMainWindow;
    BMainWindow gui;
    gui.setupGUI( widget );

    widget->show();
    int qtExecResult;
    qtExecResult = appl.exec();
    return qtExecResult;
}
