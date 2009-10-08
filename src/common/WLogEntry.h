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

#ifndef WLOGENTRY_H
#define WLOGENTRY_H

#include <string>

/**
 * Various log levels, to distinguish output on its level.
 */
typedef enum
{
    LL_DEBUG = 0,
    LL_INFO,
    LL_WARNING,
    LL_ERROR
}
LogLevel;

/**
 * Represents a simple log message with some attributes.
 */
class WLogEntry
{
public:
    /**
     * Construtcs a log message entry
     */
    WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source = "" );

    /**
     * Destroys a log message entry.
     */
    virtual ~WLogEntry();

    /**
     * \return String of this log entry.
     */
    std::string getLogString( std::string format = "[%t] *%l* %m \n" );

    /**
     * \return log level of this entry.
     */
    LogLevel getLogLevel();

protected:
private:
    /**
     * The time the log message was received
     */
    std::string m_time;

    /**
     * The actual message
     */
    std::string m_message;

    /**
     * Log level
     */
    LogLevel m_level;

    /**
     * Source (e.g. module name) where this log message comes from.
     */
    std::string m_source;
};

#endif  // WLOGENTRY_H
