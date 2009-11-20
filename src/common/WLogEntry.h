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
     * Define possible attributes.
     */
    enum CColorAttrib
    {
        Off         = 0,
        Bold        = 1,
        Underscore  = 4,
        Blink       = 5,
        Reverse     = 7,
        Concealed   = 8
    };

    /** 
     * Foreground colors.
     */
    enum CColorForeground
    {
        FGBlack   = 30,
        FGRed     = 31,
        FGGreen   = 32,
        FGYellow  = 33,
        FGBlue    = 34,
        FGMagenta = 35,
        FGCyan    = 36,
        FGWhite   = 37
    };

    /** 
     * Background colors.
     */
    enum CColorBackground
    {
        BGBlack   = 40,
        BGRed     = 41,
        BGGreen   = 42,
        BGYellow  = 43,
        BGBlue    = 44,
        BGMagenta = 45,
        BGCyan    = 46,
        BGWhite   = 47
    };

    /** 
     * Create a control string to activate colors. It will be active until reset was called. When the background should be not set, leave it.
     *
     * \param attrib the attribute to set
     * \param foreground the foreground color
     * \param background the background color.
     * 
     * \return control string 
     */
    std::string color( CColorAttrib attrib, CColorForeground foreground, CColorBackground background );

    /** 
     * Create a control string to activate colors. It will be active until reset was called. When the background should be not set, leave it.
     *
     * \param attrib the attribute to set
     * \param foreground the foreground color
     * \param background the background color.
     * 
     * \return control string 
     */
    std::string color( CColorAttrib attrib, CColorForeground foreground );

    /** 
     * Resets all attributes and colors to default.
     * 
     * \return the proper control string.
     */
    std::string reset();
};

#endif  // WLOGENTRY_H
