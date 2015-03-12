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

#ifndef WMODULEDISCONNECTEVENT_H
#define WMODULEDISCONNECTEVENT_H

#include <boost/shared_ptr.hpp>

#include <QtCore/QEvent>

#include "core/kernel/WModuleConnector.h"

/**
 * Event signalling a module connection was closed.
 */
class WModuleDisconnectEvent: public QEvent
{
public:
    /**
     * Creates a new event denoting the established connection between both connectors.
     *
     * \param in the input
     * \param out the output
     */
    WModuleDisconnectEvent( boost::shared_ptr< WModuleConnector > in, boost::shared_ptr< WModuleConnector > out );

    /**
     * Destructor.
     */
    virtual ~WModuleDisconnectEvent();

    /**
     * Gets the input connector involved in this connection event.
     *
     * \return the connector.
     */
    boost::shared_ptr< WModuleConnector > getInput() const;

    /**
     * Gets the output connector involved in this connection event.
     *
     * \return the connector.
     */
    boost::shared_ptr< WModuleConnector > getOutput() const;

protected:
    /**
     * The input.
     */
    boost::shared_ptr< WModuleConnector > m_in;

    /**
     * The output.
     */
    boost::shared_ptr< WModuleConnector > m_out;

private:
};

#endif  // WMODULEDISCONNECTEVENT_H

