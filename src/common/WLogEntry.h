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

#include "WTerminalColor.h"
#include "WExportCommon.h"

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
class OWCOMMON_EXPORT WLogEntry // NOLINT
{
public:

    /**
     * Creates a new log message.
     *
     * \param logTime  the time
     * \param message  the message
     * \param level    the log level
     * \param source   the source, sending the log
     * \param colored  true if colors should be used.
     */
    WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source = "", bool colored = true );

    /**
     * Destroys a log message entry.
     */
    virtual ~WLogEntry();

    /**
     * \param format A string describing the output format in c printf style
     * \return String of this log entry.
     */
    std::string getLogString( std::string format = "[%t] *%l* %m \n" );

    /**
     * \return log level of this entry.
     */
    LogLevel getLogLevel();

    /**
     * Set whether to use colors or not. Note: this is only useful on Linux systems currently.
     *
     * \param colors true if colors should be used.
     */
    void setColored( bool colors );

    /**
     * Getter determining whether to use colors or not.
     *
     * \return true if colors should be used.
     */
    bool isColored();

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

    /**
     * Flag determining whether colors should be used or not.
     */
    bool m_colored;

    /**
     * Color used for error logs.
     */
    WTerminalColor m_errorColor;

    /**
     * Color used for info logs
     */
    WTerminalColor m_infoColor;

    /**
     * Color used for debug logs.
     */
    WTerminalColor m_debugColor;

    /**
     * Color used for warning logs.
     */
    WTerminalColor m_warningColor;

    /**
     * Color used for source field.
     */
    WTerminalColor m_sourceColor;

    /**
     * Color used for time.
     */
    WTerminalColor m_timeColor;

    /**
     * Color used for the message.
     */
    WTerminalColor m_messageColor;
};

#endif  // WLOGENTRY_H
