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

#include <ostream>
#include <string>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem/fstream.hpp>

#include "exceptions/WSignalSubscriptionInvalid.h"

#include "WLogger.h"

/**
 * Used for program wide access to the logger.
 */
WLogger* logger = NULL;

WLogStream::WLogStream( std::ostream& output, LogLevel logLevel, std::string format,  bool colored ):
    m_output( output ),
    m_logLevel( logLevel ),
    m_format( format ),
    m_color( colored )
{
    // do nothing
}

void WLogStream::printEntry( const WLogEntry& entry )
{
    // level test
    if ( m_logLevel > entry.getLogLevel() )
    {
        return;
    }

    m_output << entry.getLogString( m_format, m_color );
}

void WLogStream::setLogLevel( LogLevel logLevel )
{
    m_logLevel = logLevel;
}

LogLevel WLogStream::getLogLevel() const
{
    return m_logLevel;
}

void WLogStream::setFormat( std::string format )
{
    m_format = format;
}

std::string WLogStream::getFormat() const
{
    return m_format;
}

void WLogStream::setColored( bool colors )
{
    m_color = colors;
}

bool WLogStream::isColored() const
{
    return m_color;
}

WLogger::WLogger( std::ostream& output, LogLevel level ):
    m_outputs()
{
    logger = this;
    m_outputs.push_back( WLogStream::SharedPtr( new WLogStream( output, level ) ) );

    addLogMessage( "Initalizing Logger", "Logger", LL_INFO );
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

boost::signals2::connection WLogger::subscribeSignal( LogEvent event, LogEntryCallback callback )
{
    switch ( event ) // subscription
    {
    case AddLog:
        return m_addLogSignal.connect( callback );
    default:
        throw new WSignalSubscriptionInvalid( std::string( "Signal could not be subscribed. The event is not compatible with the callback." ) );
    }
}

void WLogger::addLogMessage( std::string message, std::string source, LogLevel level )
{
    boost::posix_time::ptime t( boost::posix_time::second_clock::local_time() );
    std::string timeString( to_simple_string( t ) );
    WLogEntry entry( timeString, message, level, source );

    // signal to all interested
    m_addLogSignal( entry );

    // output
    Outputs::ReadTicket r = m_outputs.getReadTicket();
    for ( Outputs::ConstIterator i = r->get().begin(); i != r->get().end(); ++i )
    {
        ( *i )->printEntry( entry );
    }
}

void WLogger::setDefaultFormat( std::string format )
{
    m_outputs[0]->setFormat( format );
}

std::string WLogger::getDefaultFormat()
{
    return m_outputs[0]->getFormat();
}

void WLogger::addStream( WLogStream::SharedPtr s )
{
    m_outputs.push_back( s );
}

