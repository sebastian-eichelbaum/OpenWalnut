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
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "WLogEntry.h"

WLogEntry:: WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source, bool colored )
    : m_time( logTime ),
      m_message( message ),
      m_level( level ),
      m_source( source ),
      m_colored( colored )
{
}

WLogEntry::~WLogEntry()
{
}

std::string WLogEntry::getLogString( std::string format )
{
    std::string s = format;

    std::string red = color( Bold, FGRed );
    std::string green = color( Bold, FGGreen );
    std::string blue = color( Bold, FGBlue );
    std::string yellow = color( Bold, FGYellow );
    std::string magenta = color( Off, FGMagenta );
    std::string gray = color( Bold, FGBlack );

    boost::ireplace_first( s, "%t", gray + m_time + reset() );

    switch ( m_level )
    {
        case LL_DEBUG:
            boost::ireplace_first( s, "%l", blue + "DEBUG  " + reset() );
            break;
        case LL_INFO:
            boost::ireplace_first( s, "%l", green + "INFO   " + reset() );
            break;
        case LL_WARNING:
            boost::ireplace_first( s, "%l", yellow + "WARNING" + reset() );
            break;
        case LL_ERROR:
            boost::ireplace_first( s, "%l", red + "ERROR  " + reset() );
            break;
        default:
            break;
    }

    boost::ireplace_first( s, "%m", m_message );

    boost::ireplace_first( s, "%s", magenta + m_source + reset() );

    return s;
}

LogLevel WLogEntry::getLogLevel()
{
    return m_level;
}

std::string WLogEntry::color( CColorAttrib attrib, CColorForeground foreground, CColorBackground background )
{
#ifdef __linux__
    if ( m_colored )
    {
        std::ostringstream ss;

        char cStart = 0x1B;
        ss << cStart << "[" << attrib << ";" << foreground << ";" << background << "m";

        return ss.str();
    }
#endif
    return "";
}

std::string WLogEntry::color( CColorAttrib attrib, CColorForeground foreground )
{
#ifdef __linux__
    if ( m_colored )
    {
        std::ostringstream ss;

        char cStart = 0x1B;
        ss << cStart << "[" << attrib << ";" << foreground << "m";

        return ss.str();
    }
#endif
    return "";
}

std::string WLogEntry::reset()
{
#ifdef __linux__
    if ( m_colored )
    {
        std::ostringstream ss;

        char cStart = 0x1B;
        ss << cStart << "[0m";
        return ss.str();
    }
#endif
    return "";
}

void WLogEntry::setColored( bool colors )
{
    m_colored = colors;
}

bool WLogEntry::isColored()
{
    return m_colored;
}

