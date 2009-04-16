//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
//---------------------------------------------------------------------------

#include <iostream>
#include <QtGui/QApplication>

#include "gui/qt4/BMainWindow.h"
using namespace std;

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
