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

#ifndef WDHNOSUCHFILE_H
#define WDHNOSUCHFILE_H

#include <stdexcept>
#include <string>

#include "WDHException.h"
#include "../WExportDataHandler.h"

/**
 * File not found exception.
 */
class OWDATAHANDLER_EXPORT WDHNoSuchFile : public WDHException // NOLINT
{
public:
    /**
     * Default constructor.
     * \param fname the exception message.
     */
    explicit WDHNoSuchFile( const std::string &fname = "unknown file not found" );

    /**
     * Destructor
     */
    virtual ~WDHNoSuchFile() throw();

protected:
private:
};

#endif  // WDHNOSUCHFILE_H
