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
#include <boost/thread/locks.hpp>
#include "../common/WThreadedRunner.h"

#include "WLogEntry.h"

/**
 * Does actual logging of WLogEntries down to stdout or something similar.
 */
class WLogger: public WThreadedRunner
{
public:
    /**
     * Constructor
     */
    WLogger( std::string fileName = "walnut.log", LogLevel level = LL_DEBUG );

    /**
     * Destructor.
     */
    virtual ~WLogger();

    /**
     * Returns pointer to the currently running logger instance.
     *
     * \return pointer to logger instance.
     */
    static WLogger* getLogger();

    /**
     * Sets the global log level
     */
    void setLogLevel( LogLevel level );

    /**
     * Sets the log level for stdout.
     */
    void setSTDOUTLevel( LogLevel level );

    /**
     * Sets the log level for stderr.
     */
    void setSTDERRLevel( LogLevel level );

    /**
     * Sets the log level for the given log file.
     */
    void setLogFileLevel( LogLevel level );

    /**
     * Specifies the path for logging to this file and checks if the path
     * exists by an assertion.
     */
    void setLogFileName( std::string fileName );

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

    /**
     * Set the default format used for log entries.
     * 
     * \param format the format string. See WLogEntry for details.
     */
    void setDefaultFormat( std::string format );

    /**
     * Gets the default format used for log entries.
     * 
     * \return format string. See WLogEntry for details.
     */
    std::string getDefaultFormat();

    /**
     * Set the default format used for log entries in log files.
     * 
     * \param format the format string. See WLogEntry for details.
     */
    void setDefaultFileFormat( std::string format );

    /**
     * Gets the default format used for log entries in log files.
     * 
     * \return format string. See WLogEntry for details.
     */
    std::string getDefaultFileFormat();
 
    /**
     * Appends a log message to the logging queue.
     */
    void addLogMessage( std::string message, std::string source = "", LogLevel level = LL_DEBUG );

    /**
     * Locks this logging instance for threadsafeness and prints the
     * items of the queue.
     */
    void processQueue();

    /**
     * Entry point after loading the module. Runs in separate thread.
     */
    virtual void threadMain();

protected:

private:
    /**
     * We do not want a copy constructor, so we define it private.
     */
    WLogger( const WLogger& );

    /**
     * The actual level of logging so messages with a lower level will be
     * discarded.
     */
    LogLevel m_LogLevel;

    /**
     * LogLevel for stdout
     */
    LogLevel m_STDOUTLevel;

    /**
     * LogLevel for stderr
     */
    LogLevel m_STDERRLevel;

    /**
     * LogLevel for the given log file
     */
    LogLevel m_LogFileLevel;

    /**
     * Filename of the log file
     */
    std::string m_LogFileName;

    /**
     * Storage for all WLogEntries that were given to our logging instance
     */
    std::vector< WLogEntry > m_SessionLog;

    /**
     * Queue for storing pending WLogEntries.
     */
    std::queue< WLogEntry > m_LogQueue;

    /**
     * Mutex for doing locking due to thread-safety.
     */
    boost::mutex m_QueueMutex;

    /**
     * Flag determining whether log entries can be colored or not.
     */
    bool m_colored;

    /**
     * The default format used for new log entries.
     */
    std::string m_defaultFormat;

    /**
     * The default format used for new log entries in files.
     */
    std::string m_defaultFileFormat;
};

#endif  // WLOGGER_H
