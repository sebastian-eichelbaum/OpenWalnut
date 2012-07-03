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

#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include "WLogEntry.h"
#include "WLogStream.h"
#include "WStringUtils.h"
#include "WSharedSequenceContainer.h"


/**
 * This class defines the interface for adding logs and managing several output streams for them. The actual log entry is in \ref WLogEntry and
 * the output is done in \ref WLogStream.
 */
class WLogger       // NOLINT
{
public:
    /**
     * Create the first and only instance of the logger as it is a singleton.
     *
     * \param output the output stream to use
     * \param level the default log level
     */
    static void startup( std::ostream& output = std::cout, LogLevel level = LL_DEBUG );  // NOLINT - we need this non-const ref here

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
     * Adds a new stream to the logger. This is useful to register file streams or uncolored GUI based outputs.
     * \note It is not intended to allow getting streams or modifying them except you are the owner/creator.
     *
     * \param s the stream to add.
     */
    void addStream( WLogStream::SharedPtr s );

    /**
     * Remove the given stream.
     *
     * \param s the stream to remove
     */
    void removeStream( WLogStream::SharedPtr s );

    /**
     * Set the default format used for log entries.
     *
     * \param format the format string. See WLogEntry for details.
     */
    void setDefaultFormat( std::string format );

    /**
     * Set the default log-level used for log entries in default console-output
     *
     * \param level the log-level
     */
    void setDefaultLogLevel( const LogLevel& level );

    /**
     * Gets the default format used for log entries. This actually returns the format of the first log stream.
     *
     * \return format string. See WLogEntry for details.
     */
    std::string getDefaultFormat();

    /**
     * Appends a log message to the logging queue.
     * \param message the log entry
     * \param source indicates where this entry comes from
     * \param level The logging level of the current message
     */
    void addLogMessage( std::string message, std::string source = "", LogLevel level = LL_DEBUG );

    /**
     * Types of signals supported by the logger
     */
    typedef enum
    {
        AddLog = 0 //!< for added logs
    }
    LogEvent;

    /**
     * The type for all callbacks which get a log entry as parameter.
     */
    typedef boost::function< void ( WLogEntry& ) > LogEntryCallback;

    /**
     * Subscribe to the specified signal.
     *
     * \note If you want to listen to incoming log entries, you can also utilize the WLogStream class.
     *
     * \param event the kind of signal the callback should be used for.
     * \param callback the callback.
     *
     * \return the connection object. Disconnect it explicitly!
     */
    boost::signals2::connection subscribeSignal( LogEvent event, LogEntryCallback callback );

protected:
private:
    /**
     * Constructor. The logger is created using the static method startup.
     *
     * \param output the stream where to print log messages to
     * \param level logging level, i.e. verboseness
     */
    WLogger( std::ostream& output, LogLevel level );  // NOLINT - we need this non-const ref here

    /**
     * We do not want a copy constructor, so we define it private.
     */
    WLogger( const WLogger& );

    /**
     * The output stream list type.
     */
    typedef WSharedSequenceContainer< std::vector< WLogStream::SharedPtr > > Outputs;

    /**
     * The list of outputs to print the messages to.
     */
    Outputs m_outputs;

    /**
     * Signal called whenever a new log message arrives.
     */
    boost::signals2::signal< void( WLogEntry& ) > m_addLogSignal;
};

/**
 * This namespace collects several convenient access points such as wlog::err
 * for logging with streams to our WLogger.
 */
namespace wlog
{
    /**
     * Resource class for streamed logging.
     */
    class WStreamedLogger // NOLINT
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
        // \cond Suppress_Doxygen
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
         * \return A reference to the variable for which assignment was INTENDED.
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
        using string_utils::operator<<; // in case we want to log arrays or vectors
        m_buffer->m_logString << loggable;
        return *this;
    }

    inline WStreamedLogger WStreamedLogger::operator<<( StreamManipulatorFunctor manip )
    {
        manip( m_buffer->m_logString );
        return *this;
    }

    inline WStreamedLogger::Buffer::Buffer( const std::string& source, LogLevel level )
        : m_logString(),
        m_level( level ),
        m_source( source )
    {
    }

    /**
     * Convenient function for logging messages to our WLogger but not for
     * public use outside of this module.
     *
     * \param source Indicate the source where this log message origins.
     * \param level The LogLevel of this message
     * \return The logger created using the functions parameters
     */
    inline WStreamedLogger _wlog( const std::string& source, LogLevel level )
    {
        return WStreamedLogger( source, level );
    }

    /**
     * Logging an error message.
     *
     * \param source Indicate the source where this log message origins.
     * \return The logger with the error message.
     */
    inline WStreamedLogger error( const std::string& source )
    {
        return _wlog( source, LL_ERROR );
    }

    /**
     * Logging a warning message.
     *
     * \param source Indicate the source where this log message origins.
     * \return The logger with the warning message.
     */
    inline WStreamedLogger warn( const std::string& source )
    {
        return _wlog( source, LL_WARNING );
    }

    /**
     * Logging an information message.
     *
     * \param source Indicate the source where this log message origins.
     * \return The logger with the warning message.
     */
    inline WStreamedLogger info( const std::string& source )
    {
        return _wlog( source, LL_INFO );
    }

    /**
     * Logging a debug message.
     *
     * \param source Indicate the source where this log message origins.
     * \return The logger with the debug message.
     */
    inline WStreamedLogger debug( const std::string& source )
    {
        return _wlog( source, LL_DEBUG );
    }
} // end of namespace log

#endif  // WLOGGER_H
