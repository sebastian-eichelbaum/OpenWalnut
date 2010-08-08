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

#ifndef WGESIGNALSUBSCRIPTIONFAILED_H
#define WGESIGNALSUBSCRIPTIONFAILED_H

#include <string>

#include "WGEException.h"
#include "../WExportWGE.h"

/**
 * Exception thrown if a notifier could not be subscribed to a signal.
 * \ingroup ge
 */
class WGE_EXPORT WGESignalSubscriptionFailed: public WGEException
{
public:

    /**
     * Default constructor.
     * \param msg the exception message.
     */
    explicit WGESignalSubscriptionFailed( const std::string& msg = "Could not subscribe to unknown signal." );

    /**
     * Destructor.
     */
    virtual ~WGESignalSubscriptionFailed() throw();

protected:

private:
};

#endif  // WGESIGNALSUBSCRIPTIONFAILED_H

