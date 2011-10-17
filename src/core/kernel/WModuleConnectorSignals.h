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

#ifndef WMODULECONNECTORSIGNALS_H
#define WMODULECONNECTORSIGNALS_H

#include <boost/signals2/signal.hpp>
#include <boost/function.hpp>

class WModuleConnector;

/**
 * Enum of all possible signals between WModuleConnector instances.
 * TODO(ebaum): Replace this by an automatic mapping class also able to allow varying function signatures.
 */
typedef enum
{
CONNECTION_ESTABLISHED,                             // a connection has been established
CONNECTION_CLOSED,                                  // a connection has been closed
DATA_CHANGED                                        // data on an output connector has been changed
}
MODULE_CONNECTOR_SIGNAL;

// **************************************************************************************************************************
// Types
// **************************************************************************************************************************

/**
 * Signal transmitting sender and receiver.
 *
 * \param boost::shared_ptr<WModuleConnector> this should be the receiver, depending on specific signal.
 * \param boost::shared_ptr<WModuleConnector> this should be the sender, depending on specific signal.
 *
 */
typedef boost::function<void ( boost::shared_ptr<WModuleConnector>,
                               boost::shared_ptr<WModuleConnector> )> t_GenericSignalHandlerType;

/**
 * Generic signal type used in the most signals involving a sender and receiver.
 *
 * \param recv The connector receiving the signal.
 * \param sender The counterpart (sender).
 */
typedef boost::signals2::signal<void ( boost::shared_ptr<WModuleConnector>,
                                       boost::shared_ptr<WModuleConnector> )>  t_GenericSignalType;

#endif  // WMODULECONNECTORSIGNALS_H

