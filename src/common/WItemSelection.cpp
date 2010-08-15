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

#include "WLogger.h"

#include "exceptions/WOutOfBounds.h"
#include "WItemSelector.h"

#include "WItemSelection.h"

WItemSelection::WItemSelection()
{
    // initialize members
}

WItemSelection::~WItemSelection()
{
    // cleanup
}

void WItemSelection::addItem( std::string name, std::string description, const char** icon )
{
    ItemListType::WriteTicket w = m_items.getWriteTicket(); // this ensures that invalidation is only triggered when no one else is currently
    // reading the selection while we write it is not allowed
    invalidateSelectors();

    Item* i = new Item;   // NOLINT  <-- initialize the struct this way is far more comfortable
    i->name = name;
    i->description = description;
    i->icon = icon;
    w->get().push_back( i );
}

void WItemSelection::removeItem( std::string name )
{
    ItemListType::WriteTicket w = m_items.getWriteTicket(); // this ensures that invalidation is only triggered when no one else is currently

    // TODO(ebaum): implement me
}

WItemSelector WItemSelection::getSelectorAll()
{
    WItemSelector::IndexList l;
    ItemListType::ReadTicket r = m_items.getReadTicket();

    for ( size_t i = 0; i < r->get().size(); ++i )
    {
        l.push_back( i );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorNone()
{
    WItemSelector::IndexList l;
    ItemListType::ReadTicket r = m_items.getReadTicket();
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorFirst()
{
    WItemSelector::IndexList l;
    ItemListType::ReadTicket r = m_items.getReadTicket();

    if ( r->get().size() >= 1 )
    {
        l.push_back( 0 );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelector( size_t item )
{
    WItemSelector::IndexList l;
    ItemListType::ReadTicket r = m_items.getReadTicket();

    if ( r->get().size() <= item )
    {
        throw WOutOfBounds( "The specified item does not exist." );
    }
    l.push_back( item );
    return WItemSelector( shared_from_this(), l );
}

void WItemSelection::invalidateSelectors()
{
    signal_invalidate();
}

boost::signals2::connection WItemSelection::subscribeInvalidationSignal( boost::function< void( void ) > invalidationCallback )
{
    return signal_invalidate.connect( invalidationCallback );
}

size_t WItemSelection::size() const
{
    return m_items.size();
}

WItemSelection::Item& WItemSelection::at( size_t index ) const
{
    return *m_items.at( index );
}

