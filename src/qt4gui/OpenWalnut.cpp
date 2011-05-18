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

#include "core/common/WSegmentationFault.h"
#include "core/common/WLogger.h"

#include "qt4/WQt4Gui.h"

#include "version.h"    // this header gets created by our build system.

/**
 * \mainpage OpenWalnut Inline Documentation
 * \par
 * http://www.openwalnut.org
 * \par
 * There exist two versions of this API.
 * A simple one at http://berkeley.informatik.uni-leipzig.de/api/
 * and one with callgraphs at http://berkeley.informatik.uni-leipzig.de/apiFull/
 * \par
 * Copyright 2009-2010 OpenWalnut Community, BSV\@Uni-Leipzig and CNCF\@MPI-CBS.
 * For more information see http://www.openwalnut.org/copying
 */

/**
 * The main routine starting up the whole application.
 */
int main( int argc, char** argv )
{
    std::cout << "OpenWalnut ( http://www.openwalnut.org )" << std::endl <<
    "Build from: " << W_VERSION << std::endl << std::endl <<
    "Copyright 2009-2010 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS\n"
    "For more information see http://www.openwalnut.org/copying\n"
    "This program comes with ABSOLUTELY NO WARRANTY.\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under the terms of the GNU Lesser General Public License.\n"
    "You should have received a copy of the GNU Lesser General Public License\n"
    "along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>." << std::endl;
    std::cout << std::endl;  // Create new line after message for clarity.

    // install signal handler as early as possible
    WSegmentationFault::installSignalHandler();

    // initialize logger here. It will be started by the GUI.
    WLogger::startup();

    // initialize GUI
    // NOTE: we need a shared_ptr here since WGUI uses enable_shared_from_this.
    boost::shared_ptr< WQt4Gui > gui = boost::shared_ptr< WQt4Gui > ( new WQt4Gui( argc, argv ) );
    int result = gui->run();

    std::cout << "Closed OpenWalnut smoothly. Goodbye!" << std::endl;

    return result;
}

