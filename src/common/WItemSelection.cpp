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

WItemSelection::WItemSelection():
    m_modifyable( true )
{
    // initialize members
}

WItemSelection::~WItemSelection()
{
    // cleanup
}

void WItemSelection::addItem( std::string name, std::string description, const char** icon )
{
    if ( m_modifyable )
    {
        Item* i = new Item;   // NOLINT  <-- initialize the struct this way is far more comfortable
        i->name = name;
        i->description = description;
        i->icon = icon;
        m_items.push_back( i );
    }
    else
    {
        wlog::warn( "WItemSelection " ) << "You can not modify the selection list after a selector has been created.";
    }
}

WItemSelector WItemSelection::getSelectorAll()
{
    m_modifyable = false;
    WItemSelector::IndexList l;
    for ( size_t i = 0; i < m_items.size(); ++i )
    {
        l.push_back( i );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorNone()
{
    m_modifyable = false;
    WItemSelector::IndexList l;
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorFirst()
{
    m_modifyable = false;
    WItemSelector::IndexList l;
    if ( m_items.size() >= 1 )
    {
        l.push_back( 0 );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelector( size_t item )
{
    m_modifyable = false;
    WItemSelector::IndexList l;
    if ( m_items.size() <= item )
    {
        throw WOutOfBounds( "The specified item does not exist." );
    }
    l.push_back( item );
    return WItemSelector( shared_from_this(), l );
}

size_t WItemSelection::size() const
{
    return m_items.size();
}

WItemSelection::Item& WItemSelection::at( size_t index ) const
{
    return *m_items.at( index );
}

