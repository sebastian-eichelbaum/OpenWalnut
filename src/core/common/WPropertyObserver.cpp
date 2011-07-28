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

#include <map>
#include <set>
#include <string>

#include "WPropertyObserver.h"

WPropertyObserver::WPropertyObserver():
    WCondition(),
    m_subscriptions(),
    m_updated( false ),
    m_properties(),
    m_propNames(),
    m_updateConditionConnection()
{
    // initialize members
}

WPropertyObserver::~WPropertyObserver()
{
    // cleanup
}

bool WPropertyObserver::updated() const
{
    return m_updated;
}

WPropertyObserver::PropertyNameMap WPropertyObserver::handled()
{
    LastUpdated::WriteTicket l = m_lastUpdated.getWriteTicket();

    // reset everything
    m_updated = false;

    // return a copy of the list
    PropertyNameMap ret( l->get() );
    l->get().clear();

    return ret;
}

void WPropertyObserver::observe( boost::shared_ptr< WProperties > properties, std::set< std::string > names )
{
    // something to do?
    if( m_properties == properties )
    {
        return;
    }

    // remove old subscriptions to the old properties
    m_updateConditionConnection.disconnect();
    cancelSubscriptions();
    m_updated = false;
    LastUpdated::WriteTicket l = m_lastUpdated.getWriteTicket();
    l->get().clear();

    m_propNames = names;

    // set new properties and subscribe to all signals
    m_properties = properties;
    // we need to get a call if properties get added or removed
    m_updateConditionConnection = m_properties->getUpdateCondition()->subscribeSignal(
        boost::bind( &WPropertyObserver::updateSubscriptions, this )
    );

    // and subscribe again to the new group's properties
    // NOTE: it may be possible that the updateSubscriptions method was called already by the above callback. But thats not that evil.
    updateSubscriptions();
}

void WPropertyObserver::cancelSubscriptions()
{
    // NOTE: locking is handled by WSharedAssociativeContainer

    // unfortunately, scoped_connections can't be used for the container as it requires copy construction which is not allowed by
    // scoped_connections. So we need to iterate by hand and disconnect

    Subscriptions::WriteTicket subs = m_subscriptions.getWriteTicket();
    for( Subscriptions::Iterator i = subs->get().begin(); i != subs->get().end(); ++i )
    {
        ( *i ).second.disconnect();
    }
    subs->get().clear();
}

void WPropertyObserver::updateSubscriptions()
{
    // lock m_subscriptions
    Subscriptions::WriteTicket subs = m_subscriptions.getWriteTicket();

    // iterate the properties
    WProperties::PropertySharedContainerType::ReadTicket props = m_properties->getReadTicket();
    for( WProperties::PropertyConstIterator i = props->get().begin(); i != props->get().end(); ++i )
    {
        // should the property be handled? (empty container ensures handling of all props)
        if( !m_propNames.size() || ( m_propNames.find( ( *i )->getName() ) != m_propNames.end() ) )
        {
            // subscribe to each update signal of the properties
            subs->get().insert( std::make_pair( *i,
                ( *i )->getUpdateCondition()->subscribeSignal( boost::bind( boost::mem_fn( &WPropertyObserver::propertyUpdated ), this, *i ) )
            ) );
        }
    }
}

void WPropertyObserver::propertyUpdated( boost::shared_ptr< WPropertyBase > property )
{
    // lock m_lastUpdated
    LastUpdated::WriteTicket l = m_lastUpdated.getWriteTicket();
    m_updated = true;
    l->get().insert( std::make_pair( property->getName(), property ) );
    notify();
}

boost::shared_ptr< WPropertyObserver > WPropertyObserver::create()
{
    return boost::shared_ptr< WPropertyObserver >( new WPropertyObserver() );
}

