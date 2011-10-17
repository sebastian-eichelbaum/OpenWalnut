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

#ifndef WDHIOFAILURE_H
#define WDHIOFAILURE_H

#include <stdexcept>
#include <string>

#include "WDHException.h"
#include "../WExportDataHandler.h"

/**
 * Use this for IO error handling.
 */
class OWDATAHANDLER_EXPORT WDHIOFailure : public WDHException // NOLINT
{
public:
    /**
     * Default constructor.
     * \param msg the exception message.
     */
    explicit WDHIOFailure( const std::string& msg = "DataHandler Exception: IO error occured." );

    /**
     * Destructor
     */
    virtual ~WDHIOFailure() throw();

protected:
private:
};

#endif  // WDHIOFAILURE_H
