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

#include <string>

#include <boost/signals2/signal.hpp>
#include <boost/signals2/connection.hpp>

#include "WModuleInputConnector.h"
#include "WModuleConnectorSignals.h"

#include "WModuleOutputConnector.h"

WModuleOutputConnector::WModuleOutputConnector( boost::shared_ptr< WModule > module, std::string name, std::string description ):
    WModuleConnector( module, name, description )
{
    // initialize members
}

WModuleOutputConnector::~WModuleOutputConnector()
{
    // cleanup
}

bool WModuleOutputConnector::connectable( boost::shared_ptr<WModuleConnector> con )
{
    // output connectors are just allowed to get connected with input connectors
    if( dynamic_cast<WModuleInputConnector*>( con.get() ) )  // NOLINT - since we really need them here
    {
        return true;
    }

    return false;
}

bool WModuleOutputConnector::lazyConnectable( boost::shared_ptr<WModuleConnector> con )
{
    // output connectors are just allowed to get connected with input connectors
    if( dynamic_cast<WModuleInputConnector*>( con.get() ) )  // NOLINT - since we really need them here
    {
        return true;
    }

    return false;
}

boost::signals2::connection WModuleOutputConnector::subscribeSignal( MODULE_CONNECTOR_SIGNAL signal,
                                                                     t_GenericSignalHandlerType notifier )
{
    // connect DataChanged signal
    switch( signal )
    {
        case DATA_CHANGED:
            return signal_DataChanged.connect( notifier );
        default:    // we do not know this signal: maybe the base class knows it
            return WModuleConnector::subscribeSignal( signal, notifier );
    }
}

void WModuleOutputConnector::propagateDataChange()
{
    signal_DataChanged( boost::shared_ptr<WModuleConnector>(), shared_from_this() );
}

bool WModuleOutputConnector::isInputConnector() const
{
    return false;
}

bool WModuleOutputConnector::isOutputConnector() const
{
    return true;
}
