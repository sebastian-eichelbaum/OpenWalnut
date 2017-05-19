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

#ifndef WMODULECONNECTOR_H
#define WMODULECONNECTOR_H

#include <set>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2/connection.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>

#include "WModule.h"
#include "WModuleCombinerTypes.h"
#include "WModuleConnectorSignals.h"



class WModuleInputConnector;
class WModuleOutputConnector;

/**
 * Base class for modelling connections between kernel modules. It contains several pure virtual member functions and can
 * therefore not instantiated directly.
 */
class  WModuleConnector: public boost::enable_shared_from_this<WModuleConnector>
{
    friend class WModuleConnectorTest; //!< Access for test class.
    friend class WModuleProjectFileCombiner; //!< Access for creating a module graph automatically.

public:
    /**
     * Shared pointer to this class.
     */
    typedef boost::shared_ptr< WModuleConnector > SPtr;

    /**
     * Const shared pointer to this class.
     */
    typedef boost::shared_ptr< const WModuleConnector > ConstSPtr;

    /**
     * Constructor.
     *
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */
    WModuleConnector( boost::shared_ptr< WModule > module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleConnector();

    /**
     * Returns the module which owns this connector.
     *
     * \return the module owning the connector.
     */
    boost::shared_ptr< WModule > getModule() const;

    /**
     * Disconnects this connector if connected. If it is not connected: nothing happens.
     *
     * \param con the connector to disconnect.
     * \param removeFromOwnList if true the specified connection is also removed from the own connection list. If false it won't.
     */
    virtual void disconnect( boost::shared_ptr<WModuleConnector> con, bool removeFromOwnList = true );

    /**
     * Disconnects ALL connected connectors.
     */
    virtual void disconnectAll();

    /**
     * Connects this Module Connector with another one. During connection process, just the connectibility flag from
     * WModuleConnector::connectable is used to determine whether the connection is possible or not.
     *
     * \param con the connector to connect.
     * \param force force connection even if incompatible. Used primarily for project file loader. If true, only a \ref lazyConnectable is used.
     *
     * \exception WModuleConnectionFailed if connection can not be established.
     *
     * \return true if successful
     */
    virtual void connect( boost::shared_ptr<WModuleConnector> con, bool force = false );

    /**
     * Checks whether this connector is connected to the given one. If there is the strange case where one connector is connected
     * with the other one but not vice versa it will throw an exception.
     *
     * \param con the connector to check connection with.
     *
     * \return true if connected
     *
     * \throw WModuleConnectionInvalid thrown if one connector thinks it is connected but the other one not.
     */
    bool isConnectedTo( boost::shared_ptr<WModuleConnector> con );

    /**
     * Gets the count of connections currently established.
     *
     * \return the number of connections.
     */
    unsigned int isConnected();

    /**
     * Connects a specified notify function with a signal this module instance is offering.
     *
     * \exception WSignalSubscriptionFailed thrown if the signal can't be connected.
     *
     * \param signal the signal to connect to.
     * \param notifier the notifier function to bind.
     *
     * \return connection descriptor.
     */
    virtual boost::signals2::connection subscribeSignal( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier );

    /**
     * Gives information about this connection.
     *
     * \return The connection's description.
     */
    const std::string getDescription() const;

    /**
     * Sets the connector's description. This is not thread-safe! Do not use it outside the WModule thread.
     *
     * \param desc the new description.
     */
    void setDescription( std::string desc );

    /**
     * Gives name of connection.
     *
     * \return The name of this connection
     */
    const std::string getName() const;

    /**
     * Gives canonical name of connection. The canonical name is a descriptor including module name. The description is
     * ModuleName:ConnectorName.
     *
     * \return The name of this connection
     */
    const std::string getCanonicalName() const;

    /**
     * Sets the connector's name. This is not thread-safe! Do not use it outside the WModule thread.
     *
     * \param name the new name.
     */
    void setName( std::string name );

    /**
     * Checks whether the specified connector is connectable to this one.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool connectable( boost::shared_ptr<WModuleConnector> con ) = 0;

    /**
     * Checks whether the specified connector is connectable to this one, but ignores compatibility the type to be transferred. If you implement your
     * own connectors, please override and check for compatibility with your matching connectors, but ignore the transfer type.
     *
     * \param con the connector to check against.
     *
     * \return true if compatible.
     */
    virtual bool lazyConnectable( boost::shared_ptr<WModuleConnector> con ) = 0;

    /**
     * Returns a list of possible disconnections for this connector. Please be aware that the connections might change during the life-time of
     * the returned DisconnectCombiner instances.
     *
     * \return the possible disconnections.
     */
    WCombinerTypes::WOneToOneCombiners getPossibleDisconnections();

    /**
     * Tries to convert this instance to an input connector.
     *
     * \return this as  input connector
     */
    boost::shared_ptr< WModuleInputConnector > toInputConnector();

    /**
     * Tries to convert this instance to an output connector.
     *
     * \return this as output connector
     */
    boost::shared_ptr< WModuleOutputConnector > toOutputConnector();

    /**
     * Returns true if this instance is an WModuleInputConnector.
     *
     * \return true if castable to WModuleInputConnector.
     */
    virtual bool isInputConnector() const = 0;

    /**
     * Returns true if this instance is an WModuleOutputConnector.
     *
     * \return true if castable to WModuleOutputConnector.
     */
    virtual bool isOutputConnector() const = 0;

    /**
     * Gets the condition variable that gets fired whenever new data has been sent.
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getDataChangedCondition();

protected:
    /**
     * List of connectors connected to this connector.
     */
    std::set<boost::shared_ptr<WModuleConnector> > m_connected;

    /**
     * Lock for avoiding concurrent write to m_Connected (multiple reader, single writer lock). The read lock can be acquired using
     * the boost::shared_lock<boost::shared_mutex> lock( m_ConnectionListLock );.
     */
    boost::shared_mutex m_connectionListLock;

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
     * Gives the signal handler function responsible for a given signal. Modules defining own signal handlers should overwrite
     * this function. This function is protected since boost::functions are callable, which is what is not wanted here. Just
     * signals should call them.
     *
     * \param signal the signal to get the handler for.
     *
     * \return the signal handler for "signal".
     */
    virtual const t_GenericSignalHandlerType getSignalHandler( MODULE_CONNECTOR_SIGNAL signal );

    /**
     * The Module this connector belongs to
     */
    boost::weak_ptr< WModule > m_module;

    /**
     * The name of the module owning this connector.
     */
    std::string m_moduleName;

    /**
     * Gets called whenever a connector gets connected to the specified input.
     *
     * \param here the connector of THIS module that got connected to "there"
     * \param there the connector that has been connected with the connector "here" of this module.
     */
    virtual void notifyConnectionEstablished( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there );

    /**
     * Gets called whenever a connection between a remote and local connector gets closed.
     *
     * \param here the connector of THIS module getting disconnected.
     * \param there the connector of the other module getting disconnected.
     */
    virtual void notifyConnectionClosed( boost::shared_ptr<WModuleConnector> here, boost::shared_ptr<WModuleConnector> there );

    /**
     * Signal emitted whenever connection has been established.
     */
    t_GenericSignalType signal_ConnectionEstablished;

    /**
     * Signal emitted whenever connection has been closed.
     */
    t_GenericSignalType signal_ConnectionClosed;

    /**
     * Condition fired whenever data changes. Proper calling is made in subclasses.
     */
    boost::shared_ptr< WCondition > m_dataChangedCondition;

private:
    /**
     * The connections name.
     */
    std::string m_name;

    /**
     * The connections description.
     */
    std::string m_description;
};

#endif  // WMODULECONNECTOR_H
