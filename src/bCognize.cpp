/*
 * bCognize.cpp
 *
 *  Created on: Mar 25, 2009
 *      Author: wiebel
 */

#include <iostream>
#include <QtGui/QApplication>

#include "gui/qt4/BCMainWindow.h"
using namespace std;

int main( int argc, char* argv[] )
{
	cout << "To start the new project there has to be this main." << endl;
    QApplication appl( argc, argv );
    QMainWindow *widget = new QMainWindow;
    BCMainWindow gui;
    gui.setupGUI(widget);


    widget->show();
	int qtExecResult;
    qtExecResult = appl.exec ();
	return qtExecResult;
}
