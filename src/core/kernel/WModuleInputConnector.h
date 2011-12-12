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

#ifndef WMODULEINPUTCONNECTOR_H
#define WMODULEINPUTCONNECTOR_H

#include <string>

#include <boost/thread/locks.hpp>

#include "WModule.h"
#include "WModuleConnector.h"
#include "WModuleConnectorSignals.h"

#include "../common/WCondition.h"

#include "WExportKernel.h"

/**
 * Class implementing input connection functionality between modules.
 */
class OWKERNEL_EXPORT WModuleInputConnector: public WModuleConnector
{
public:
    /**
     * Constructor.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleInputConnector( boost::shared_ptr< WModule > module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleInputConnector();

    /**
     * Checks whether the specified connector is an output connector.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con );

    /**
     * Gets the condition variable that gets fired whenever new data has been sent.
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getDataChangedCondition();

    /**
     * Connects (subscribes) a specified notify function with a signal this module instance is offering.
     *
     * \exception WModuleSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return the connection. Disconnect it manually if not needed anymore!
     */
    boost::signals2::connection subscribeSignal( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier );

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

    /**
     * Denotes whether the connected output was updated. This does NOT denote an actual change in the current data!
     *
     * \return true if there has been an update.
     */
    virtual bool updated();

    /**
     * Resets the updated-flag. Call this from your module to reset the value of updated().
     *
     * \return the update flag before reset. Useful to get the flag and reset it in one call.
     */
    virtual bool handledUpdate();

protected:
    /**
     * Connect additional signals.
     *
     * \param con the connector that requests connection.
     *
     */
    virtual void connectSignals( boost::shared_ptr<WModuleConnector> con );

    /**
     * Disconnect all signals subscribed by this connector from "con".
     *
     * \param con the connector that gets disconnected.
     */
    virtual void disconnectSignals( boost::shared_ptr<WModuleConnector> con );

    /**
     * Gets called when the data on this input connector changed.
     *
     * \param input the input connector receiving the change.
     * \param output the output connector sending the change notification.
     */
    virtual void notifyDataChange( boost::shared_ptr<WModuleConnector> input, boost::shared_ptr<WModuleConnector> output );

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there );

    /**
     * Sets the update flag (use updated() to query it)to true. This is normally called by the notifyDataChange callback.
     */
    virtual void setUpdated();

private:
    /**
     * Signal for "DATA_CHANGED" Events. We use a separate signal here (instead of using the signal send by the connected output)
     * since the output can not determine the receiver when signalling. So we use an own signal handler and signal to "forward"
     * the message and complete the information with a this-pointer.
     */
    t_GenericSignalType signal_DataChanged;

    /**
     * Condition fired whenever data changes.
     */
    boost::shared_ptr< WCondition > m_dataChangedCondition;

    /**
     * Connection for Data Changed signal of the connected output connector.
     */
    boost::signals2::connection m_DataChangedConnection;

    /**
     * This lock protects the m_updated flag.
     */
    boost::shared_mutex m_updatedLock;

    /**
     * A flag denoting that an update was received. It does not denote a real change in the value!
     */
    bool m_updated;
};

#endif  // WMODULEINPUTCONNECTOR_H

