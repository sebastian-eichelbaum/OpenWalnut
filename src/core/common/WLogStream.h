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

#ifndef WLOGSTREAM_H
#define WLOGSTREAM_H

#include <ostream>
#include <string>
#include <boost/shared_ptr.hpp>

#include "WLogEntry.h"

/**
 * Class implementing a capsule for an output stream and the needed level and format information.
 */
class WLogStream // NOLINT
{
public:
    typedef boost::shared_ptr< WLogStream > SharedPtr;  //!< shared pointer type
    typedef WLogStream* Ptr; //!< pointer type
    typedef WLogStream& Ref; //!< reference
    typedef const WLogStream& ConstRef; //!< const reference

    /**
     * Constructor. Create a new stream instance. The output stream is a mandatory parameter. The others are predefined with some defaults.
     *
     * \param output the stream where to print log messages to
     * \param logLevel logging level, i.e. verboseness
     * \param format the format used for output
     * \param colored true if coloring should be used.
     */
    WLogStream( std::ostream& output, LogLevel logLevel = LL_DEBUG, std::string format = "*%l [%s] %m \n", bool colored = true );  // NOLINT - we need this non-const ref here

    /**
     * Prints the specified entry to the output stream in the right format if the log level matches.
     *
     * \param entry the entry to print-
     */
    void printEntry( const WLogEntry& entry );

    /**
     * Sets the new log level. All new incoming logs will be filtered according to this level.
     *
     * \param logLevel the level
     */
    void setLogLevel( LogLevel logLevel );

    /**
     * Gets the currently set log level.
     *
     * \return the current log level
     */
    LogLevel getLogLevel() const;

    /**
     * Sets the format string.
     *
     * \param format the format string.
     */
    void setFormat( std::string format );

    /**
     * Returns the currently set format string.
     *
     * \return format string.
     */
    std::string getFormat() const;

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
    bool isColored() const;

private:
    /**
     * Disallow copy.
     *
     * \param rhs the stream to copy
     */
    WLogStream( const WLogStream& rhs );

    /**
     * Disallow assignment.
     *
     * \param rhs the stream to assign to this
     *
     * \return this
     */
    WLogStream& operator=( const WLogStream& rhs );

    /**
     * The output stream.
     */
    std::ostream& m_output;

    /**
     * The logging level. All messages below this level are discarded.
     */
    LogLevel m_logLevel;

    /**
     * The format of the message.
     */
    std::string m_format;

    /**
     * True if colors should be used. This requires a compatible terminal.
     */
    bool m_color;
};

#endif  // WLOGSTREAM_H

