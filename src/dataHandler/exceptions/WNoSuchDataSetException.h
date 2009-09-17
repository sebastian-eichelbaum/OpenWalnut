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

#ifndef WNOSUCHDATASETEXCEPTION_H
#define WNOSUCHDATASETEXCEPTION_H

#include <string>

#include "../../common/WException.h"

/**
 * Should be thrown when an invalid index is used to get a WSubject from
 * the WDataHandler or a WDataSet from the
 * WSubject. An index is invalid if it's greater or equal than the number
 * of WDataSets in that WDataHandler.
 *
 * It's subclassed from std::logic_error since it represents a mistake by a
 * programmer, not by the runtime system (e.g. allocation memory) or other
 * libraries.
 *
 * \ingroup dataHandler
 */
class WNoSuchDataSetException : public WException
{
public:
    /**
     * Constructs new exception.
     */
    WNoSuchDataSetException( const std::string& s = std::string() ) throw()
        : WException( s )
    {
    }

    /**
     * Destroys this exception
     */
    virtual ~WNoSuchDataSetException() throw()
    {
    };

protected:

private:
};

#endif  // WNOSUCHDATASETEXCEPTION_H
