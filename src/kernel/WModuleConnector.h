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

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/bind.hpp>

#include "WModule.h"

/**
 * Base class for modelling connections between kernel modules. It contains several pure virtual member functions and can
 * therefore not instantiated directly.
 */
class WModuleConnector: public boost::enable_shared_from_this<WModuleConnector>
{
public:

    /** 
     * Constructor.
     * 
     * \param module the module which is owner of this connector.
     * \param name The name of this connector.
     * \param description Short description of this connector.
     */    
    WModuleConnector( boost::shared_ptr<WModule> module, std::string name="", std::string description="" );

    /**
     * Destructor.
     */
    virtual ~WModuleConnector();

    /** 
     * Disconnects this connector if connected. If it is not connected: nothing happens.
     *
     * \param con the connector to disconnect.
     */
    virtual void disconnect( boost::shared_ptr<WModuleConnector> con );

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
     * Sets the connector's name. This is not thread-safe! Do not use it outside the WModule thread.
     * 
     * \param name the new name.
     */
    void setName( std::string name );

protected:

    /** 
     * Connects this Module Connector with another one. During connection process, just the connectibility flag from
     * WModuleConnector::connectable is used to determine whether the connection is possible or not.
     * 
     * \param con the connector to connect.
     * 
     * \return true if successful
     */
    virtual bool connect( boost::shared_ptr<WModuleConnector> con );

    /** 
     * Disconnects ALL connected connectors.
     */
    virtual void disconnectAll();

    /** 
     * List of connectors connected to this connector.
     */
    std::set<boost::shared_ptr<WModuleConnector> > m_Connected;

    /** 
     * Lock for avoiding concurrent write to m_Connected (multiple reader, single writer lock). The read lock can be acquired using
     * the boost::shared_lock<boost::shared_mutex> lock( m_ConnectionListLock );.
     */
    boost::shared_mutex m_ConnectionListLock;

    // Define the signals common to all connectors here.

    /** 
     * Generic signal type used in the most signals involving a sender and receiver.
     * 
     * \param recv The connector receiving the signal.
     * \param sender The counterpart (sender).
     */
    typedef boost::signals2::signal<void ( boost::shared_ptr<WModuleConnector>,
                                           boost::shared_ptr<WModuleConnector> )>  t_GenericSignalType;

private:

    /** 
     * The connections name.
     */
    std::string m_Name;

    /** 
     * The connections description.
     */
    std::string m_Description;

    /** 
     * The Module this connector belongs to
     */
    boost::shared_ptr<WModule> m_Module;

    /** 
     * Signal emitted whenever connection has been established.
     */
    t_GenericSignalType signal_ConnectionEstablished;

    /** 
     * Signal emitted whenever connection has been closed.
     */
    t_GenericSignalType signal_ConnectionClosed;

};

#endif  // WMODULECONNECTOR_H

