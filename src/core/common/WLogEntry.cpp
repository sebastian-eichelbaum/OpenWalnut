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

#include "WTerminalColor.h"

#include "WLogEntry.h"

LogLevel logLevelFromString( const std::string& str )
{
    // get lower-case version
    std::string strLower = str; // NOTE: this reserves the needed space
    std::transform( str.begin(), str.end(), strLower.begin(), tolower );
    if( !strLower.compare( "debug" ) )
    {
        return LL_DEBUG;
    }
    else if( !strLower.compare( "info" ) )
    {
        return LL_INFO;
    }
    else if( !strLower.compare( "warning" ) )
    {
        return LL_WARNING;
    }
    else if( !strLower.compare( "error" ) )
    {
        return LL_ERROR;
    }
    return LL_DEBUG;
}

WLogEntry::WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source )
    : m_time( logTime ),
      m_message( message ),
      m_level( level ),
      m_source( source ),
      m_errorColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGRed ) ),
      m_infoColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGGreen ) ),
      m_debugColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGBlue ) ),
      m_warningColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGYellow ) ),
      m_sourceColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGMagenta ) ),
      m_timeColor( WTerminalColor( WTerminalColor::Bold, WTerminalColor::FGBlack ) ),
      m_messageColor( WTerminalColor() )
{
}

WLogEntry::~WLogEntry()
{
}

std::string WLogEntry::getLogString( std::string format, bool colors ) const
{
    std::string s = format;

    m_errorColor.setEnabled( colors );
    m_infoColor.setEnabled( colors );
    m_debugColor.setEnabled( colors );
    m_warningColor.setEnabled( colors );
    m_sourceColor.setEnabled( colors );
    m_timeColor.setEnabled( colors );
    m_messageColor.setEnabled( colors );

    boost::ireplace_first( s, "%t", m_timeColor + m_time + !m_timeColor );

    switch( m_level )
    {
        case LL_DEBUG:
            boost::ireplace_first( s, "%l", m_debugColor + "DEBUG  " + !m_debugColor );
            break;
        case LL_INFO:
            boost::ireplace_first( s, "%l", m_infoColor + "INFO   " + !m_infoColor );
            break;
        case LL_WARNING:
            boost::ireplace_first( s, "%l", m_warningColor + "WARNING" + !m_warningColor );
            break;
        case LL_ERROR:
            boost::ireplace_first( s, "%l", m_errorColor + "ERROR  " + !m_errorColor );
            break;
        default:
            break;
    }

    boost::ireplace_first( s, "%m", m_messageColor + m_message + !m_messageColor );

    boost::ireplace_first( s, "%s", m_sourceColor + m_source + !m_sourceColor );

    return s;
}

LogLevel WLogEntry::getLogLevel() const
{
    return m_level;
}

std::string WLogEntry::getMessage() const
{
    return m_message;
}

std::string WLogEntry::getSource() const
{
    return m_source;
}

std::string WLogEntry::getTime() const
{
    return m_time;
}

