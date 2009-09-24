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

#ifndef WLOGGER_H
#define WLOGGER_H

#include <queue>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "../common/WThreadedRunner.h"

#include "WLogEntry.h"

/**
 * TODO(schurade): Document this!
 */
class WLogger: public WThreadedRunner
{
public:
    /**
     * Default constructor.
     */
    WLogger();

    /**
     * Constructor
     */
    WLogger( std::string fileName, LogLevel level = LL_DEBUG );

    /**
     * Destructor.
     */
    virtual ~WLogger();

    /**
     *
     */
    void setLogLevel( LogLevel level );

    /**
     *
     */
    void setSTDOUTLevel( LogLevel level );

    /**
     *
     */
    void setSTDERRLevel( LogLevel level );

    /**
     *
     */
    void setLogFileLevel( LogLevel level );


    /**
     *
     */
    void setLogFileName( std::string fileName );

    /**
     *
     */
    void addLogMessage( std::string message, std::string source = "", LogLevel level = LL_DEBUG );

    /**
     *
     */
    void processQueue();

protected:
    /**
     *
     */
    LogLevel m_LogLevel;

    /**
     *
     */
    LogLevel m_STDOUTLevel;

    /**
     *
     */
    LogLevel m_STDERRLevel;

    /**
     *
     */
    LogLevel m_LogFileLevel;


    /**
     *
     */
    std::string m_LogFileName;

    /**
     *
     */
    std::vector< WLogEntry > m_SessionLog;

    /**
     *
     */
    std::queue< WLogEntry > m_LogQueue;

private:
    /**
     *
     */
    bool init();
};

#endif  // WLOGGER_H
