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

#ifndef WEXCEPTION_H
#define WEXCEPTION_H

#include <exception>
#include <list>
#include <string>
#include <sstream>

#include "WTerminalColor.h"
#include "WExportCommon.h"

/**
 * Basic exception handler.
 */
class OWCOMMON_EXPORT WException: public std::exception
{
/**
 * Only UnitTests are allowed to be a friend of this class.
 */
friend class WExceptionTest;

public:

    /**
     * Default constructor.
     * \param msg Exception description.
     */
    explicit WException( const std::string& msg = std::string() );

    /**
     * Copy a std::exception and encapsulate it.
     *
     * \param e the exception.
     */
    explicit WException( const std::exception& e );

    /**
     * Destructor.
     */
    virtual ~WException() throw();

    /**
     * Returns the message string set on throw
     * \return Message text
     */
    virtual const char* what() const throw();

    /**
     * Prints the trace of the call chain which caused this exception.
     * \return Calltrace as string
     * \note Isn't this useless? Should be removed.
     */
    std::string getTrace( ) const;

    /**
     * Returns a call stacktrace.
     * \return The backtrace at the moment of "throw".
     */
    std::string getBacktrace() const;

    /**
     * Function disables backtraces. Please note that the backtrace can not be reactivated to avoid people from dis/enabling them
     * at will.
     */
    static void disableBacktrace();

protected:
    /**
     * Message given during throw.
     */
    std::string m_msg;

    /**
     * Stack trace for identifying the source where this exception came from.
     * \note Isn't this useless? Should be removed.
     */
    std::list< std::string > m_trace;

    /**
     * True if the backtrace should NOT be printed.
     */
    static bool noBacktrace;
private:

    /**
     * Color used for the "trace:" label.
     */
    WTerminalColor m_labelColor;

    /**
     * Color used for function name.
     */
    WTerminalColor m_functionColor;

    /**
     * Color used for symbols.
     */
    WTerminalColor m_symbolColor;

    /**
     * Color used for exception headline.
     */
    WTerminalColor m_headlineColor;
};

#endif  // WEXCEPTION_H

