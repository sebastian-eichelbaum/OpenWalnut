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
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/fstream.hpp>

#include "WLogger.h"

/**
 * Used for program wide access to the logger.
 */
WLogger* logger = NULL;

WLogger::WLogger( std::string fileName, LogLevel level ):
    WThreadedRunner(),
    m_LogLevel( level ),

    m_STDOUTLevel( level ),
    m_STDERRLevel( LL_ERROR ),
    m_LogFileLevel( level ),
    m_LogFileName( fileName ),
    m_QueueMutex()
{
    logger = this;

    addLogMessage( "Initalizing Logger", "Logger", LL_DEBUG );
    addLogMessage( "===============================================================================", "Logger", LL_INFO );
    addLogMessage( "=                          Starting Log Session                               =", "Logger", LL_INFO );
    addLogMessage( "===============================================================================", "Logger", LL_INFO );
}

WLogger::~WLogger()
{
}

WLogger* WLogger::getLogger()
{
    return logger;
}

void WLogger::setLogLevel( LogLevel level )
{
    m_LogLevel = level;
}

void WLogger::setSTDOUTLevel( LogLevel level )
{
    m_STDOUTLevel = level;
}

void WLogger::setSTDERRLevel( LogLevel level )
{
    m_STDERRLevel = level;
}

void WLogger::setLogFileLevel( LogLevel level )
{
    m_LogFileLevel = level;
}

void WLogger::setLogFileName( std::string fileName )
{
    boost::filesystem::path p( fileName );

    // TODO(schurade): check if this is a _VALID_ path (existence is not needed)

    m_LogFileName = fileName;
}

void WLogger::addLogMessage( std::string message, std::string source, LogLevel level )
{
    if ( m_LogLevel > level || m_FinishRequested )
    {
        return;
    }

    boost::posix_time::ptime t( boost::posix_time::second_clock::local_time() );
    std::string timeString( to_simple_string( t ) );
    WLogEntry entry( timeString, message, level, source );

    boost::mutex::scoped_lock l( m_QueueMutex );
    m_LogQueue.push( entry );
}

void WLogger::processQueue()
{
    boost::mutex::scoped_lock l( m_QueueMutex );

    while ( !m_LogQueue.empty() )
    {
        WLogEntry entry = m_LogQueue.front();
        m_LogQueue.pop();

        m_SessionLog.push_back( entry );

        if ( entry.getLogLevel() >= m_STDOUTLevel )
        {
            std::cout << entry.getLogString();
        }

        if ( entry.getLogLevel() >= m_STDERRLevel )
        {
            std::cerr << entry.getLogString();
        }

        if ( entry.getLogLevel() >= m_LogFileLevel )
        {
            // TODO(schurade): first open file, then write to file, then close the file
            // for atomic file usage.
            boost::filesystem::path p( "walnut.log" );
            boost::filesystem::ofstream ofs( p, boost::filesystem::ofstream::app );
            ofs << entry.getLogString();
        }
    }
}

void WLogger::threadMain()
{
    // Since the modules run in a separate thread: such loops are possible
    while ( !m_FinishRequested )
    {
        processQueue();
        // do fancy stuff
        sleep( 1 );
    }

    // clean up stuff
}
