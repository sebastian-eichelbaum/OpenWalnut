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

#ifndef WDHVALUESETMISMATCH_H
#define WDHVALUESETMISMATCH_H

#include <string>

#include "WDHException.h"
#include "../WExportDataHandler.h"

/**
 * An exception that should be used whenever two valuesets are used which need to be of same size, dim, order or whatever.
 *
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDHValueSetMismatch : public WDHException
{
public:
    /**
     * Constructs new exception.
     * \param msg the reason for this exception.
     */
    explicit WDHValueSetMismatch( const std::string& msg = "ValueSets do not match." );

    /**
     * Destroys this exception
     */
    virtual ~WDHValueSetMismatch() throw();

protected:

private:
};

#endif  // WDHVALUESETMISMATCH_H

