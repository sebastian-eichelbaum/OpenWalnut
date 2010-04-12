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
#include <string>
#include <vector>

#include <boost/regex.hpp>

#include "../kernel/WKernel.h"
#include "../kernel/combiner/WModuleProjectFileCombiner.h"
#include "exceptions/WFileNotFound.h"

#include "WProjectFile.h"

WProjectFile::WProjectFile( boost::filesystem::path project ):
    WThreadedRunner(),
    boost::enable_shared_from_this< WProjectFile >(),
    m_project( project )
{
    // initialize members

    // The module graph parser
    m_parsers.push_back( new WModuleProjectFileCombiner() );

    // The ROI parser
    // m_parsers.push_back( new W???() );

    // The Camera parser
    // m_parsers.push_back( new W???() );
}

WProjectFile::~WProjectFile()
{
    // cleanup
}

void WProjectFile::load()
{
    // the instance needs to be added here, as it else could be freed before the thread finishes ( remember: it is a shared_ptr ).
    WKernel::getRunningKernel()->getRootContainer()->addPendingThread( shared_from_this() );

    // actually run
    run();
}

void WProjectFile::save()
{
    wlog::info( "Project File" ) << "Saving project file \"" << m_project.file_string() << "\".";
    wlog::info( "Project File" ) << "SORRY. Not yet implemented.";
}

void WProjectFile::threadMain()
{
    try
    {
        // Parse the file
        wlog::info( "Project File" ) << "Loading project file \"" << m_project.file_string() << "\".";

        // read the file
        std::ifstream input( m_project.file_string().c_str() );
        if ( !input.is_open() )
        {
            throw WFileNotFound( "The project file \"" + m_project.file_string() + "\" does not exist." );
        }

        // the comment
        static const boost::regex commentRe( "^//.*$" );

        // read it line by line
        std::string line;       // the current line
        int i = 0;              // line counter
        bool match = false;     // true of a parser successfully parsed the line
        boost::smatch matches;  // the list of matches

        while ( std::getline( input, line ) )
        {
            ++i;    // line number
            match = false;

            // allow each parser to handle the line.
            for ( std::vector< WProjectFileParser* >::const_iterator iter = m_parsers.begin(); iter != m_parsers.end(); ++iter )
            {
                if ( ( *iter )->parse( line, i ) )
                {
                    match = true;
                    // the first parser matching this line -> next line
                    break;
                }
            }

            // did someone match this line? Or is it empty or a comment?
            if ( !match && !line.empty() && !boost::regex_match( line, matches, commentRe ) )
            {
                // no it is something else -> warning!
                wlog::warn( "Project Loader" ) << "Line " << i << ": Malformed. Skipping.";
            }
        }

        input.close();

        // finally, let every one know that we have finished
        for ( std::vector< WProjectFileParser* >::const_iterator iter = m_parsers.begin(); iter != m_parsers.end(); ++iter )
        {
            ( *iter )->done();
        }
    }
    catch( const std::exception& e )
    {
        // remove from thread list
        WKernel::getRunningKernel()->getRootContainer()->finishedPendingThread( shared_from_this() );

        // re-throw
        throw e;
    }

    // remove from thread list
    WKernel::getRunningKernel()->getRootContainer()->finishedPendingThread( shared_from_this() );
}

