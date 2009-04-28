//---------------------------------------------------------------------------
//
// Project: BrainCognize
//
// Copyright 2009 SomeCopyrightowner
//
// This file is part of BrainCognize.
//
// BrainCognize is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BrainCognize is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BrainCognize. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#include <iostream>

#include <QtGui/QApplication>

#include "gui/qt4/BMainWindow.h"

/**
 * The main routine starting up the whole application.
 *
 * \mainpage BrainCognize Inline Documentation
 *
 * For a list of the current modules see the "Modules" tab in the naviagtion bar above.
 */
int main( int argc, char* argv[] )
{
    std::cout << "BrainCognize  Copyright (C) 2009  SomeCopyrightowner\n\
    This program comes with ABSOLUTELY NO WARRANTY.\n\
    This is free software, and you are welcome to redistribute it\n\
    under the terms of the GNU Lesser General Public License.\n\
    You should have received a copy of the GNU Lesser General Public License\n\
    along with BrainCognize. If not, see <http://www.gnu.org/licenses/>." << std::endl;

    QApplication appl( argc, argv );
    QMainWindow *widget = new QMainWindow;
    BMainWindow gui;
    gui.setupGUI( widget );

    widget->show();
    int qtExecResult;
    qtExecResult = appl.exec();
    return qtExecResult;
}
