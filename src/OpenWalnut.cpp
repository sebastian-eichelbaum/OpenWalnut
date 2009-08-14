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

#include <iostream>

#include "common/WException.h"
#include "common/WSegmentationFault.h"

#include "kernel/WKernel.h"

/**
 * The main routine starting up the whole application.
 *
 * \mainpage OpenWalnut Inline Documentation
 *
 * For a list of the current modules see the "Modules" tab in the navigation bar above.
 */
int main( int argc, char* argv[] )
{
    std::cout << "OpenWalnut  Copyright (C) 2009  SomeCopyrightowner\n"
    "This program comes with ABSOLUTELY NO WARRANTY.\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under the terms of the GNU Lesser General Public License.\n"
    "You should have received a copy of the GNU Lesser General Public License\n"
    "along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>." << std::endl;
    std::cout << std::endl;  // Create new line after message for clarity.

    // install signal handler as early as possible
    WSegmentationFault::installSignalHandler();

    // init the kernel
    WKernel kernel = WKernel( argc, argv );
    int code = kernel.run();
    return code;
}
