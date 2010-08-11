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
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

#include "WLogEntry.h"
#include "WStringUtils.h"
#include "WThreadedRunner.h"
#include "WExportCommon.h"

/**
 * Does actual logging of WLogEntries down to stdout or something similar.
 */
class OWCOMMON_EXPORT WLogger: public WThreadedRunner
{
public:
    /**
     * Constructor
     * \param fileName the log will be stored in this file
     * \param level logging level, i.e. verboseness
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
     * \param level the new global logging level
     */
    void setLogLevel( LogLevel level );

    /**
     * Sets the log level for stdout.
     * \param level the new logging level for stdout
     */
    void setSTDOUTLevel( LogLevel level );

    /**
     * Sets the log level for stderr.
     * \param level the new logging level for stderr
     */
    void setSTDERRLevel( LogLevel level );

    /**
     * Sets the log level for the given log file.
     * \param level the new level for logging to file
     */
    void setLogFileLevel( LogLevel level );

    /**
     * Specifies the path for logging to this file and checks if the path
     * exists by an assertion.
     * \param fileName the name and path of the file to be used for logging.
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
     * \param message the log entry
     * \param source indicates where this entry comes from
     * \param level The logging level of the current message
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

/**
 * This namespace collects several convinient access points such as wlog::err
 * for logging with streams to our WLogger.
 */
namespace wlog
{
    /**
     * Resource class for streamed logging.
     */
    class WStreamedLogger
    {
    public:
        /**
         * Creates new streamed logger instance. Logging is deferred until
         * destruction of this instance.
         *
         * \param source Source from which the log message originates
         * \param level The LogLevel of the message
         */
        WStreamedLogger( const std::string& source, LogLevel level );

        /**
         * Appends something loggable (to std::string castable) to the log.
         *
         * \param loggable Token that should be streamed into the Logger
         * \return The streamed logger for further use
         */
        template< typename T > WStreamedLogger operator<<( const T& loggable );

        // Doxygen should ignore the TypeDef below which are just an alias for std::endl etc.
        // \cond
        typedef std::basic_ostream< char, std::char_traits< char > > OutStreamType;
        typedef OutStreamType& ( *StreamManipulatorFunctor )( OutStreamType& );
        // \endcond


        /**
         * This is totally crazy man! Don't get dizzy on that, watch out and
         * ask a C++ guru next to your side, which is probably named Christian
         * or have a look on that: http://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator
         *
         * Allow std::endl to be streamed into log messages.
         *
         * \param manip Function pointer e.g. std::endl, std::flush, std::ends
         * \return The streamed logger for further use
         */
        WStreamedLogger operator<<( StreamManipulatorFunctor manip );

    protected:
    private:
        /**
         * Actually implementing the streaming functionality.
         */
        class Buffer
        {
        public: // NOLINT inner classes may have also lables
            /**
             * Constructs a new stream for logging.
             *
             * \param source String identifying the source of the message
             * \param level LogLevel of the message
             */
            Buffer( const std::string& source, LogLevel level );

            /**
             * Commits the logging expression to our WLogger
             */
            virtual ~Buffer();

            std::ostringstream m_logString; //!< queuing up parts of the log message
            LogLevel m_level; //!< Default logging level for this stream
            std::string m_source; //!< The source of the logging message
        };

        /**
         * Forbid assignment
         *
         * \param rhs The instance which SHOULD be copied over
         */
        WStreamedLogger& operator=( const WStreamedLogger& rhs );

        boost::shared_ptr< Buffer > m_buffer; //!< Collects the message parts.
    };

    inline WStreamedLogger::WStreamedLogger( const std::string& source, LogLevel level )
        : m_buffer( new Buffer( source, level ) )
    {
    }

    template< typename T > inline WStreamedLogger WStreamedLogger::operator<<( const T& loggable )
    {
        using string_utils::operator<<; // incase we want to log arrays or vectors
        m_buffer->m_logString << loggable;
        return *this;
    }

    inline WStreamedLogger WStreamedLogger::operator<<( StreamManipulatorFunctor manip )
    {
        manip( m_buffer->m_logString );
        return *this;
    }

    inline WStreamedLogger::Buffer::~Buffer()
    {
        WLogger::getLogger()->addLogMessage( m_logString.str(), m_source, m_level );
    }

    inline WStreamedLogger::Buffer::Buffer( const std::string& source, LogLevel level )
        : m_logString(),
        m_level( level ),
        m_source( source )
    {
    }

    /**
     * Convinient function for logging messages to our WLogger but not for
     * public use outside of this module.
     *
     * \param source Indicate the source where this log message origins.
     * \param level The LogLevel of this message
     */
    inline WStreamedLogger _wlog( const std::string& source, LogLevel level )
    {
        return WStreamedLogger( source, level );
    }

    /**
     * Logging an error message.
     *
     * \param source Indicate the source where this log message origins.
     */
    inline WStreamedLogger error( const std::string& source )
    {
        return _wlog( source, LL_ERROR );
    }

    /**
     * Loggin a warning message.
     *
     * \param source Indicate the source where this log message origins.
     */
    inline WStreamedLogger warn( const std::string& source )
    {
        return _wlog( source, LL_WARNING );
    }

    /**
     * Loggin an information message.
     *
     * \param source Indicate the source where this log message origins.
     */
    inline WStreamedLogger info( const std::string& source )
    {
        return _wlog( source, LL_INFO );
    }

    /**
     * Loggin a debug message.
     *
     * \param source Indicate the source where this log message origins.
     */
    inline WStreamedLogger debug( const std::string& source )
    {
        return _wlog( source, LL_DEBUG );
    }
} // end of namespace log

#endif  // WLOGGER_H
