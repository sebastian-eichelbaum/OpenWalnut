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

#include <iostream>
#include <list>
#include <string>
#include <sstream>

#include "boost/exception.hpp"
#include "boost/signals2/signal.hpp"
#include "boost/signals2/connection.hpp"

#include "WModule.h"
#include "exceptions/WModuleConnectionFailed.h"
#include "exceptions/WModuleDisconnectFailed.h"
#include "exceptions/WModuleSignalSubscriptionFailed.h"
#include "exceptions/WModuleConnectorsIncompatible.h"
#include "WModuleConnectorSignals.h"

#include "WModuleConnector.h"

WModuleConnector::WModuleConnector( boost::shared_ptr<WModule> module, std::string name, std::string description ):
    boost::enable_shared_from_this<WModuleConnector>()
{
    // initialize members
    m_Module = module;

    m_Name = name;
    m_Description = description;

    // connect standard signals
    // NOTE: these signals are NOT emitted by the connector this one is connected to, since a module can't send a "connection
    // closed" message if the connection is closed.
    signal_ConnectionEstablished.connect( getSignalHandler( CONNECTION_ESTABLISHED ) );
    signal_ConnectionClosed.connect( getSignalHandler( CONNECTION_CLOSED ) );
}

WModuleConnector::~WModuleConnector()
{
    disconnectAll();

    // cleanup
    signal_ConnectionEstablished.disconnect_all_slots();
    signal_ConnectionClosed.disconnect_all_slots();
}

void WModuleConnector::connect( boost::shared_ptr<WModuleConnector> con )
{
    // are both partners compatible to each other?
    if ( !( con->connectable( shared_from_this() ) && connectable( con ) ) )
    {
        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectorsIncompatible( s.str() );
    }

    boost::unique_lock<boost::shared_mutex> lock;
    try
    {
        // add to list
        lock = boost::unique_lock<boost::shared_mutex>( m_ConnectionListLock );
        m_Connected.insert( con );
        lock.unlock();

        // add to list of partner
        lock = boost::unique_lock<boost::shared_mutex>( con->m_ConnectionListLock );
        con->m_Connected.insert( shared_from_this() );
        lock.unlock();
    }
    catch ( std::exception& e )
    {
        lock.unlock();

        // undo changes
        m_Connected.erase( con );
        con->m_Connected.erase( con );
    
        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectionFailed( s.str() );
    }
    catch ( boost::exception& e )
    {
        lock.unlock();

        // undo changes
        m_Connected.erase( con );
        con->m_Connected.erase( con );

        std::ostringstream s;
        s << "Connection between " << getCanonicalName() << " and " << con->getCanonicalName() << " failed.";
        throw WModuleConnectionFailed( s.str() );
    }

    // let them connect their signals
    connectSignals( con );
    con->connectSignals( shared_from_this() );

    // signal "connection established"
    signal_ConnectionEstablished( shared_from_this(), con );
    // signal to my partner, of course with the parameters the other way round
    con->signal_ConnectionEstablished( con, shared_from_this() );
}

void WModuleConnector::connectSignals( boost::shared_ptr<WModuleConnector> /*con*/ )
{
    // Add extra signal- connections here that are COMMON to ALL connectors.
    // NOTE: connection established and connection closed are not signals to connect, since you can not send an connection closed
    // signal to somebody with whom you are not connected anymore ;-).
}

void WModuleConnector::disconnectSignals( boost::shared_ptr<WModuleConnector> /*con*/ )
{
    // The base module does not subscribe to any signal -> no disconnection needed here
}

boost::signals2::connection WModuleConnector::subscribeSignal( MODULE_CONNECTOR_SIGNAL signal, t_GenericSignalHandlerType notifier)
{
    switch (signal)
    {
        case CONNECTION_ESTABLISHED:
            return signal_ConnectionEstablished.connect( notifier );
        case CONNECTION_CLOSED:
            return signal_ConnectionClosed.connect( notifier );
        default:
            throw new WModuleSignalSubscriptionFailed( "Could not subscribe to unknown signal. You need to implement this signal type\
                    explicitly." );
            break;
    }
}

const t_GenericSignalHandlerType WModuleConnector::getSignalHandler( MODULE_CONNECTOR_SIGNAL signal )
{
    // the module instance knows that
    return m_Module->getSignalHandler( signal );
}

void WModuleConnector::disconnect( boost::shared_ptr<WModuleConnector> con, bool removeFromOwnList )
{
    boost::unique_lock<boost::shared_mutex> lock;
    try
    {
        // disconnect all signals
        con->disconnectSignals( shared_from_this() );
        disconnectSignals( con );
       
        // remove from list
        if ( removeFromOwnList )
        {
            lock = boost::unique_lock<boost::shared_mutex>( m_ConnectionListLock );
            // since we use shared pointers, erasing the item should be enough
            m_Connected.erase( con );
            lock.unlock();
        }

        // remove me from his list
        lock = boost::unique_lock<boost::shared_mutex>( con->m_ConnectionListLock );
        con->m_Connected.erase( shared_from_this() );
        lock.unlock();

        // signal closed connection
        signal_ConnectionClosed( shared_from_this(), con );
        con->signal_ConnectionClosed( con, shared_from_this() );
    }
    catch ( std::exception& e )
    {
        lock.unlock();

        std::ostringstream s;
        s << "Unable to disconnect " << getCanonicalName() << " from " << con->getCanonicalName() << ".";
        throw new WModuleDisconnectFailed( s.str() );
    }
    catch ( boost::exception& e )
    {
        lock.unlock();

        std::ostringstream s;
        s << "Unable to disconnect " << getCanonicalName() << " from " << con->getCanonicalName() << ".";
        throw new WModuleDisconnectFailed( s.str() );
    }
}

void WModuleConnector::disconnectAll()
{
    // remove from list
    boost::unique_lock<boost::shared_mutex> lock( m_ConnectionListLock );

    // each connector needs to be notified and disconnected properly
    for( std::set<boost::shared_ptr<WModuleConnector> >::iterator listIter = m_Connected.begin(); listIter != m_Connected.end();
         ++listIter )
    {
        disconnect( *listIter, false );
    }

    m_Connected.clear();
    lock.unlock();
}

const std::string WModuleConnector::getDescription() const
{
    return m_Description;
}

const std::string WModuleConnector::getName() const
{
    return m_Name;
}

const std::string WModuleConnector::getCanonicalName() const
{
    std::ostringstream s;
    s << m_Module->getName() << ":" << getName();

    return s.str();
}

void WModuleConnector::setDescription( std::string desc )
{
    m_Description = desc;
}

void WModuleConnector::setName( std::string name )
{
    m_Name = name;
}

