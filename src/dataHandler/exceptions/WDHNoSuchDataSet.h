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

#ifndef WDHNOSUCHDATASET_H
#define WDHNOSUCHDATASET_H

#include <string>

#include "WDHException.h"
#include "../WExportDataHandler.h"

/**
 * Should be thrown when an invalid index is used to get a WDataSet from the
 * WSubject. An index is invalid if it's greater or equal than the number
 * of WDataSets in that WSubject.
 *
 * \ingroup dataHandler
 */
class OWDATAHANDLER_EXPORT WDHNoSuchDataSet : public WDHException // NOLINT
{
public:
    /**
     * Constructs new exception.
     * \param msg the reason for this exception.
     */
    explicit WDHNoSuchDataSet( const std::string& msg = "DataHandler Exception: Invalid DataSet Access" );

    /**
     * Destroys this exception
     */
    virtual ~WDHNoSuchDataSet() throw();

protected:

private:
};

#endif  // WDHNOSUCHDATASET_H
