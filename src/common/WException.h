//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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


/**
 * \par Description:
 * Basic exception handler.
 */
class WException: public std::exception
{
/**
 * Only UnitTests are allowed to be a friend of this class.
 */
friend class WExceptionTest;

public:

    /**
     * \par Description
     * Default constructor.
     * \param msg Exception description.
     */
    explicit WException( const std::string& msg = std::string() );

    /**
     * \par Description
     * Destructor.
     */
    virtual ~WException() throw();

    /** 
     * \par Description
     * Returns the message string set on throw
     * \return Message text
     */
    virtual const char* what() const throw();

    /**
     * \par Description
     * Prints the trace of the call chain which caused this exception.
     * \return Calltrace as string
     */
    std::string getTrace( ) const;

    /** 
     * \par Description
     * Returns a call stacktrace.
     * \return The backtrace at the moment of "throw".
     */
    std::string getBacktrace() const;

protected:
    /**
     * Message given during throw.
     */
    std::string m_Msg;

    /**
     * Stack trace for identifying the source where this exception came from.
     */
    std::list< std::string > m_trace;

private:
};

#endif  // WEXCEPTION_H

