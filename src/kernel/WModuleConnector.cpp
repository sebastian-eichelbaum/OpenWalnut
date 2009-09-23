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

#include "WModule.h"

#include "WModuleConnector.h"

WModuleConnector::WModuleConnector( boost::shared_ptr<WModule> module, std::string name, std::string description )
{
    // initialize members
    m_Module = module;

    m_Name = name;
    m_Description = description;

    // connect standard signals
    // NOTE: these signals are NOT emitted by the connector this one is connected to, since a module can't send a "connection
    // closed" message if the connection is closed.
    signal_ConnectionEstablished.connect( boost::bind( &WModule::notifyConnectionEstablished, m_Module.get(), _1, _2 ) );
    signal_ConnectionClosed.connect( boost::bind( &WModule::notifyConnectionClosed, m_Module.get(), _1, _2 ) );

}

WModuleConnector::~WModuleConnector()
{
    disconnectAll();

    // cleanup
    signal_ConnectionEstablished.disconnect( boost::bind( &WModule::notifyConnectionEstablished, m_Module.get(), _1, _2 ) );
    signal_ConnectionClosed.disconnect( boost::bind( &WModule::notifyConnectionClosed, m_Module.get(), _1, _2 ) );
}

bool WModuleConnector::connect( boost::shared_ptr<WModuleConnector> con )
{
    // add to list
    boost::unique_lock<boost::shared_mutex> lock( m_ConnectionListLock );
    m_Connected.insert( con );
    lock.unlock();

    // add to list of partner
    boost::unique_lock<boost::shared_mutex> lockRemote( con->m_ConnectionListLock );
    con->m_Connected.insert( shared_from_this() );
    lockRemote.unlock();
    
    // signalling
    signal_ConnectionEstablished( shared_from_this(), con );
    // signal to my partner, of course with the parameters the other way round
    con->signal_ConnectionEstablished( con, shared_from_this() );

    return true;
}

void WModuleConnector::disconnect( boost::shared_ptr<WModuleConnector> con )
{
    // remove from list
    boost::unique_lock<boost::shared_mutex> lock( m_ConnectionListLock );

    // since we use shared pointers, erasing the item should be enough
    m_Connected.erase( con );
    lock.unlock();
}

void WModuleConnector::disconnectAll()
{
    // remove from list
    boost::unique_lock<boost::shared_mutex> lock( m_ConnectionListLock );
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

void WModuleConnector::setDescription( std::string desc )
{
    m_Description = desc;
}

void WModuleConnector::setName( std::string name )
{
    m_Name = name;
}

