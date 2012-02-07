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

#ifndef WDHNOSUCHSUBJECT_H
#define WDHNOSUCHSUBJECT_H

#include <string>

#include "WDHException.h"


/**
 * Should be thrown when an invalid index is used to get a WSubject from the
 * WDataHandler. An index is invalid if it's greater or equal than the number
 * of WSubjects in WDataHandler.
 *
 * \ingroup dataHandler
 */
class WDHNoSuchSubject : public WDHException
{
public:
    /**
     * Constructs new exception.
     * \param msg the reason for this exception.
     */
    explicit WDHNoSuchSubject( const std::string& msg = "DataHandler Exception: Requested subject does not exist." );

    /**
     * Destroys this exception
     */
    virtual ~WDHNoSuchSubject() throw();

protected:
private:
};

#endif  // WDHNOSUCHSUBJECT_H
