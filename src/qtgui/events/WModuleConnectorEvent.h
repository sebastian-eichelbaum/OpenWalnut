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

#ifndef WMODULECONNECTOREVENT_H
#define WMODULECONNECTOREVENT_H

#include <boost/shared_ptr.hpp>

#include <QtCore/QEvent>

#include "core/kernel/WModuleConnector.h"
#include "core/kernel/WModule.h"

/**
 * Event signalling a module connector changed its data.
 */
class WModuleConnectorEvent: public QEvent
{
public:
    /**
     * Creates a new event denoting the updated connector of a module.
     *
     * \param module the module
     * \param connector the output changed
     */
    WModuleConnectorEvent( WModule::SPtr module, WModuleConnector::SPtr connector );

    /**
     * Destructor.
     */
    virtual ~WModuleConnectorEvent();

    /**
     * Gets the connector that has changed.
     *
     * \return the connector.
     */
    WModuleConnector::SPtr getConnector() const;

    /**
     * Gets the module (owner) of the connector.
     *
     * \return the module.
     */
    WModule::SPtr getModule() const;

protected:
    /**
     * The module.
     */
    WModule::SPtr m_module;

    /**
     * The connector.
     */
    WModuleConnector::SPtr m_connector;

private:
};

#endif  // WMODULECONNECTOREVENT_H


