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

#ifndef WQTNETWORKINPUTPORT_H
#define WQTNETWORKINPUTPORT_H

#include "core/kernel/WModuleInputConnector.h"
#include "WQtNetworkPort.h"

// forward declaration
class WQtNetworkArrow;

/**
 * This class represents the ports a module have. Two ports from different
 * modules can be connected by the corresponding ports.
 */
class WQtNetworkInputPort : public WQtNetworkPort
{
public:
    /**
     * Constructor
     *
     * \param connector the related WModuleInputConnector
     */
    explicit WQtNetworkInputPort( boost::shared_ptr<WModuleInputConnector> connector );

    /**
     * Destructor
     */
    ~WQtNetworkInputPort();

    /**
     * This customize the return value of type()
     */
    enum
    {
        Type = UserType + 11
    };

    /**
     * Reimplementation from QGraphicsItem
     * \return the type of the item as int
     */
    int type() const;

    /**
     * Returns the WModuleInputConnecter that belongs to this object.
     * \return a WModuleInputConnector
     */
    boost::shared_ptr<WModuleInputConnector> getConnector();

private:
    boost::shared_ptr<WModuleInputConnector> m_connector; //!< the related WModuleInputConnector
};
#endif  // WQTNETWORKINPUTPORT_H
