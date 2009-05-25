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

#ifndef WNOSUCHDATASETEXCEPTION_H
#define WNOSUCHDATASETEXCEPTION_H

#include <list>
#include <stdexcept>
#include <string>

/**
 * Should be thrown when an invalid index is used to get a WDataSet from the
 * WDataHandler. An index is invalid if it's greater or equal than the number
 * of WDataSets in that WDataHandler.
 *
 * It's subclassed from std::logic_error since it represents a mistake by a
 * programmer, not by the runtime system (e.g. allocation memory) or other
 * libraries.
 */
class WNoSuchDataSetException : public std::logic_error
{
/**
 * Only UnitTests are allowed to be a friend of this class.
 */
friend class WNoSuchDataSetExceptionTest;

public:
    /**
     * Constructs new exception.
     */
    WNoSuchDataSetException( const std::string &s = std::string() ) throw()
        : std::logic_error( s )
    {
    }

    /**
     * Destroys this exception
     */
    virtual ~WNoSuchDataSetException() throw()
    {
    };

    /**
     * Prints the trace of the call chain which caused this exception.
     */
    std::string getTrace( ) const;

protected:

private:
    /**
     * Stack trace for identifying the source where this exception came from.
     */
    std::list< std::string > m_trace;
};

#endif  // WNOSUCHDATASETEXCEPTION_H
