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

#include <iostream>

#include "WMainApplication.h"
#include "WMainWindow.h"

#include <QtGui/QApplication>

#include "../../kernel/WKernel.h"

WMainApplication::WMainApplication():
    WGUI()
{
}

WMainApplication::~WMainApplication()
{
}

void WMainApplication::threadMain()
{
    // TODO(ebaum): currently removed argument stuff. will be done later in conjunction with a better
    // option handler.+
#ifdef __APPLE__
    char * dummy = "";
    int dummyInt = 0;
    QApplication appl( dummyInt, &dummy, 0 );
#else
    QApplication appl( 0, NULL );
#endif
    QMainWindow* widget = new QMainWindow;
    WMainWindow gui;
    gui.setupGUI( widget );

    widget->show();
    int qtExecResult;
    qtExecResult = appl.exec();

    // TODO(ebaum): how to handle return codes?
}

