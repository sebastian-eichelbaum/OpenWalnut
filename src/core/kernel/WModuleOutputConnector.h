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

#ifndef WMODULEOUTPUTCONNECTOR_H
#define WMODULEOUTPUTCONNECTOR_H

#include <string>

#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>

class WModule;
#include "../common/WPrototyped.h"
#include "../common/WTransferable.h"
#include "WModuleConnector.h"
#include "WModuleConnectorSignals.h"

class WCondition;

/**
 * Class implementing output connection functionality between modules.
 */
class  WModuleOutputConnector: public WModuleConnector
{
public:
    // **************************************************************************************************************************
    // Methods
    // **************************************************************************************************************************

    /**
     * Constructor.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleOutputConnector( boost::shared_ptr< WModule > module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleOutputConnector();

    /**
     * Connects (subscribes) a specified notify function with a signal this module instance is offering.
     *
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return the connection object. Disconnect manually if not needed anymore.
     */
     boost::signals2::connection subscribeSignal( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier );

    /**
     * Checks whether the specified connector is an input connector.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con );

    /**
     * Checks whether the specified connector is connectable to this one, but ignores compatibility the type to be transferred.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool lazyConnectable( boost::shared_ptr<WModuleConnector> con );

    /**
     * Returns the prototype of the WTransferable used in this connector.
     *
     * \return the prototype of the transfered type.
     */
    virtual boost::shared_ptr< WPrototyped > getTransferPrototype() = 0;

    /**
     * Gives back the currently set data as WTransferable.
     *
     * \return the data. If no data has been set: a NULL pointer is returned.
     */
    virtual const boost::shared_ptr< WTransferable > getRawData() const = 0;

    /**
     * Returns true if this instance is an WModuleInputConnector.
     *
     * \return true if castable to WModuleInputConnector.
     */
    virtual bool isInputConnector() const;

    /**
     * Returns true if this instance is an WModuleOutputConnector.
     *
     * \return true if castable to WModuleOutputConnector.
     */
    virtual bool isOutputConnector() const;

protected:
    // If you want to add additional signals an output connector should subscribe FROM an input connector, overwrite
    // connectSignals
    // virtual void connectSignals( boost::shared_ptr<WModuleConnector> con );

    /**
     * Propagates the signal "DATA_CHANGED" to all connected items.
     */
    virtual void propagateDataChange();

private:
    /**
     * Signal fired whenever new data should be propagated. Represented by DATA_CHANGED enum- element.
     */
    t_GenericSignalType signal_DataChanged;
};

#endif  // WMODULEOUTPUTCONNECTOR_H

