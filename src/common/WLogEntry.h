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

typedef enum
{
    LL_DEBUG = 0,
    LL_INFO,
    LL_WARNING,
    LL_ERROR
}
LogLevel;

/**
 * TODO(schurade): Document this!
 */
class WLogEntry
{
public:
    /**
     *
     */
    WLogEntry( std::string logTime, std::string message, LogLevel level, std::string source );

    /**
     *
     */
    virtual ~WLogEntry();

    /**
     *
     */
    std::string getLogString( std::string format = "[%t] *%l* %m \n" );

    /**
     *
     */
    LogLevel getLogLevel();

protected:

private:
    /**
     * Private standard constructor to prevent empty log entries
     */
    WLogEntry();

    /**
     *
     */
    std::string m_time;

    /**
     *
     */
    std::string m_message;

    /**
     *
     */
    LogLevel m_level;

    /**
     *
     */
    std::string m_source;
};

#endif  // WLOGENTRY_H
