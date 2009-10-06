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

#include <string>

#include <boost/algorithm/string.hpp>

#include "WLogEntry.h"

WLogEntry:: WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source )
    : m_time( logTime ),
      m_message( message ),
      m_level( level ),
      m_source( source )
{
}

WLogEntry::~WLogEntry()
{
}

std::string WLogEntry::getLogString( std::string format )
{
    std::string s = format;

    boost::ireplace_first( s, "%t", m_time );

    switch ( m_level )
    {
        case LL_DEBUG:
            boost::ireplace_first( s, "%l", "DEBUG  " );
            break;
        case LL_INFO:
            boost::ireplace_first( s, "%l", "INFO   " );
            break;
        case LL_WARNING:
            boost::ireplace_first( s, "%l", "WARNING" );
            break;
        case LL_ERROR:
            boost::ireplace_first( s, "%l", "ERROR  " );
            break;
        default:
            break;
    }

    boost::ireplace_first( s, "%m", m_message );

    boost::ireplace_first( s, "%s", m_source );

    return s;
}

LogLevel WLogEntry::getLogLevel()
{
    return m_level;
}
