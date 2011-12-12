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

#ifndef WMODULECONNECTORUNCONNECTED_H
#define WMODULECONNECTORUNCONNECTED_H

#include <string>

#include "WModuleException.h"

#include "../WExportKernel.h"

/**
 * Exception thrown whenever something is requested from an connector which is not connected.
 * \ingroup kernel
 */
class OWKERNEL_EXPORT WModuleConnectorUnconnected: public WModuleException
{
public:
    /**
     * Default constructor.
     * \param msg the exception message.
     */
    explicit WModuleConnectorUnconnected( const std::string& msg =
            "Operation needs a connected Connector, but this one is unconnected." );

    /**
     * Destructor.
     */
    virtual ~WModuleConnectorUnconnected() throw();

protected:
private:
};

#endif  // WMODULECONNECTORUNCONNECTED_H

