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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "core/common/WLogger.h"
#include "core/common/WIOTools.h"
#include "core/common/WLogStream.h"
#include "core/common/WThreadedRunner.h"
#include "core/common/WSegmentationFault.h"

#include "WQtGui.h"
#include "core/WVersion.h"   // NOTE: this file is auto-generated by CMAKE

/**
 * \mainpage OpenWalnut Inline Documentation
 * \par
 * http://www.openwalnut.org
 * \par
 * Copyright 2009-2013 OpenWalnut Community, BSV\@Uni-Leipzig and CNCF\@MPI-CBS.
 * For more information see http://www.openwalnut.org/copying
 */

void printVersion()
{
    std::cout << "OpenWalnut -- Version: " << W_VERSION << " ( http://www.openwalnut.org )"
              << std::endl
              << std::endl;

    std::cout <<
    "Copyright 2009-2013 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS\n"
    "For more information see http://www.openwalnut.org/copying\n"
    "This program comes with ABSOLUTELY NO WARRANTY.\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under the terms of the GNU Lesser General Public License.\n"
    "You should have received a copy of the GNU Lesser General Public License\n"
    "along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>." << std::endl;
    std::cout << std::endl;  // Create new line after message for clarity.

    std::cout << "Written by the OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS." << std::endl;
}

/**
 * The main routine starting up the whole application.
 */
int main( int argc, char** argv )
{
    WThreadedRunner::setThisThreadName( "QT5 Gui" );

    // where to write the by default?
    std::string logFile = "openwalnut.log";

    // process user parameter
    namespace po = boost::program_options; // since the namespace is far to big we use a shortcut here
    po::options_description desc( "Options" );

    // NOTE: if you modify this, also modify the manual pages! (use help2man or do it manually) But be careful. There need
    // to be several manual changes to be done in the manual after help2man has done its job.
    desc.add_options()
        ( "help,h", "Prints this help message" )
        ( "version,v", "Prints the version information" )
        ( "log,l", po::value< std::string >(), ( std::string( "The log-file to use. If not specified, \"" ) + logFile +
                                                 std::string( "\" is used in the current directory." ) ).c_str() )
        ( "project,p", po::value< std::string >(), "Project file to be loaded on startup." )
        ( "input,i", po::value< std::vector< std::string > >(), "Input data files that should be loaded automatically" );

    po::positional_options_description pOptDesc;
    pOptDesc.add( "input", -1 );

    boost::program_options::variables_map optionsMap;
    try
    {
        po::store( po::command_line_parser( argc, argv ).options( desc ).positional( pOptDesc ).run(), optionsMap );
    }
    catch( const po::error &e )
    {
        std::cerr << e.what() << std::endl;
        return false;
    }

    po::notify( optionsMap );

    // print usage information if command line asks for help.
    if( optionsMap.count( "help" ) )
    {
        // NOTE: if you modify this, check that help2man still works properly! (http://www.gnu.org/software/help2man) But be careful. There need
        // to be several manual changes to be done in the manual after help2man has done its job.
        std::cout << "OpenWalnut is a highly expansible visualization system with focus on brain- and neurological data." << std::endl
                  << std::endl
                  << "Usage: openwalnut [OPTION]... [FILE]..." << std::endl
                  << std::endl
                  << desc << std::endl
                  << std::endl
                  << "Examples:" << std::endl
                  << "  openwalnut\t\t\t\tStartup OpenWalnut." << std::endl
                  << "  openwalnut -p myproject.owp\t\tStart OpenWalnut and load the project." << std::endl
                  << "  openwalnut t1.nii.gz fibers.fib\tStart OpenWalnut and load the two datasets." << std::endl
                  << std::endl;
        return 0;
    }
    else if( optionsMap.count( "version" ) )
    {
        printVersion();
        return 0;
    }

    printVersion();

    // install signal handler as early as possible
    WSegmentationFault::installSignalHandler();

    // initialize logger here. It will be started by the GUI with one default console stream
    WLogger::startup();

    // add a crash-log.
    if( optionsMap.count( "log" ) )
    {
        logFile = optionsMap["log"].as< std::string >();
    }

    // determine log paths
    boost::filesystem::path logPath( logFile );
    logPath = boost::filesystem::system_complete( logPath );
    bool fallbackLog = false; // if true, the original log file could not be opened. A fallback is provided.
    boost::filesystem::path fallbackLogFile = tempFilename( "OpenWalnutLog-%%%%%%%%.log" );

    // is the log writeable?
    std::ofstream crashLogFile( logPath.string().c_str() );
    if( !crashLogFile.is_open() )
    {
        // try to create fallback
        crashLogFile.open( fallbackLogFile.string().c_str() );
        fallbackLog = true;
    }

    // create log stream
    if( crashLogFile.is_open() )
    {
        // create the WLogStream. Set special format and DISABLE colors.
        WLogStream::SharedPtr crashLog = WLogStream::SharedPtr( new WLogStream( crashLogFile, LL_DEBUG, "%t %l %s: %m\n", false ) );
        WLogger::getLogger()->addStream( crashLog );

        // NOTE: the stream flushes after each entry. This is needed if a crash occurs.
        if( !fallbackLog )
        {
            wlog::info( "Walnut" ) << "Using the file \"" << logPath.string() << "\" for logging.";
        }
        else
        {
            wlog::info( "Walnut" ) << "Using the fallback file \"" << fallbackLogFile.string() << "\" for logging.";
        }
    }
    else
    {
        wlog::warn( "Walnut" ) << "Could not open \"" << logPath.string() << "\" for writing. You will have no log-file.";
    }

    // the kernel, and the gui should print their version info. This helps processing crashlogs from users.
    wlog::info( "Walnut" ) << "Version: " << W_VERSION;

    // Help Qt find the platform specific libs/dlls
    #ifdef _WIN32
        QCoreApplication::addLibraryPath( "../libExt/qtPlugins" );
        QCoreApplication::addLibraryPath( "libExt/qtPlugins" );
    #endif
    
    // initialize GUI
    // NOTE: we need a shared_ptr here since WUI uses enable_shared_from_this.
    boost::shared_ptr< WQtGui > gui( new WQtGui( optionsMap, argc, argv ) );
    
    // Start GUI
    int result = gui->run();

    std::cout << "Closed OpenWalnut smoothly. Goodbye!" << std::endl;

    return result;
}

