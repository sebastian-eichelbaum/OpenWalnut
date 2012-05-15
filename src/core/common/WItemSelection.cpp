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
#include <vector>

#include "WLogger.h"

#include "exceptions/WOutOfBounds.h"
#include "exceptions/WNameNotUnique.h"
#include "WItemSelector.h"

#include "WItemSelection.h"

WItemSelection::WItemSelection() :
                WSharedSequenceContainer< std::vector< boost::shared_ptr< WItemSelectionItem > > >()
{
    // initialize members
}

WItemSelection::~WItemSelection()
{
    // cleanup
}

WItemSelector WItemSelection::getSelectorAll()
{
    WItemSelector::IndexList l;
    ReadTicket r = getReadTicket();

    for( size_t i = 0; i < r->get().size(); ++i )
    {
        l.push_back( i );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorNone()
{
    WItemSelector::IndexList l;
    ReadTicket r = getReadTicket();
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorFirst()
{
    WItemSelector::IndexList l;
    ReadTicket r = getReadTicket();

    if( r->get().size() >= 1 )
    {
        l.push_back( 0 );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelectorLast()
{
    WItemSelector::IndexList l;
    ReadTicket r = getReadTicket();

    if( r->get().size() >= 1 )
    {
        l.push_back( r->get().size() - 1 );
    }
    return WItemSelector( shared_from_this(), l );
}

WItemSelector WItemSelection::getSelector( size_t item )
{
    WItemSelector::IndexList l;
    ReadTicket r = getReadTicket();

    if( r->get().size() <= item )
    {
        throw WOutOfBounds( std::string( "The specified item does not exist." ) );
    }
    l.push_back( item );
    return WItemSelector( shared_from_this(), l );
}

void WItemSelection::addItem( std::string name, std::string description, const char** icon )
{
    push_back( boost::shared_ptr< WItemSelectionItem >( new WItemSelectionItem( name, description, icon ) ) );
}

void WItemSelection::addItem( boost::shared_ptr< WItemSelectionItem > item )
{
    push_back( item );
}

