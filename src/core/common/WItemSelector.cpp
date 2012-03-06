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

#include "WStringUtils.h"
#include "WItemSelection.h"

#include "WItemSelector.h"

WItemSelector::WItemSelector( boost::shared_ptr< WItemSelection > selection, IndexList selected ):
    m_selection( selection ),
    m_selected( selected ),
    m_invalidateSignalConnection(),
    m_valid( true )
{
    // initialize members
    m_invalidateSignalConnection = m_selection->getChangeCondition()->subscribeSignal( boost::bind( &WItemSelector::invalidate, this ) );
}

WItemSelector::WItemSelector( const WItemSelector& other ):
    m_selection( other.m_selection ),
    m_selected( other.m_selected ),
    m_valid( other.m_valid )
{
    m_invalidateSignalConnection = m_selection->getChangeCondition()->subscribeSignal( boost::bind( &WItemSelector::invalidate, this ) );
}

WItemSelector& WItemSelector::operator=( const WItemSelector & other )
{
    if( this != &other ) // protect against invalid self-assignment
    {
        m_selection = other.m_selection;
        m_selected = other.m_selected;
        m_valid = other.m_valid;

        m_invalidateSignalConnection = m_selection->getChangeCondition()->subscribeSignal( boost::bind( &WItemSelector::invalidate, this ) );
    }

    // by convention, always return *this
    return *this;
}

WItemSelector::~WItemSelector()
{
    // cleanup
    m_invalidateSignalConnection.disconnect();
}

WItemSelector WItemSelector::newSelector( IndexList selected ) const
{
    return createSelector( selected );
}

WItemSelector WItemSelector::newSelector( size_t selected ) const
{
    IndexList n = m_selected;
    n.push_back( selected );
    return createSelector( n );
}

WItemSelector WItemSelector::newSelector( const std::string asString ) const
{
    std::vector<std::string> tokens;
    tokens = string_utils::tokenize( asString, ";" );

    IndexList l;
    for( size_t i = 0; i < tokens.size(); ++i )
    {
        l.push_back( string_utils::fromString< size_t >( tokens[i] ) );
    }

    return createSelector( l );
}

WItemSelector WItemSelector::newSelector() const
{
    WItemSelector s( *this );
    s.m_valid = true;
    // iterate selected items to remove items with invalid index
    for( IndexList::iterator i = s.m_selected.begin(); i != s.m_selected.end(); ++i )
    {
        if( ( *i ) >= m_selection->size() )
        {
            s.m_selected.erase( i );
        }
    }
    return s;
}

std::ostream& WItemSelector::operator<<( std::ostream& out ) const
{
    for( WItemSelector::IndexList::const_iterator iter = m_selected.begin(); iter != m_selected.end(); ++iter )
    {
        out << ( *iter );
        if( ( iter + 1 ) != m_selected.end() )
        {
            out << ";";
        }
    }
    return out;
}

std::ostream& operator<<( std::ostream& out, const WItemSelector& other )
{
    return other.operator<<( out );
}

bool WItemSelector::operator==( const WItemSelector& other ) const
{
    return ( ( m_selection == other.m_selection ) && ( m_selected == other.m_selected ) && ( m_valid == other.m_valid ) );
}

size_t WItemSelector::sizeAll() const
{
    return m_selection->size();
}

size_t WItemSelector::size() const
{
    return m_selected.size();
}

const boost::shared_ptr< WItemSelectionItem > WItemSelector::atAll( size_t index ) const
{
    return m_selection->at( index );
}

const boost::shared_ptr< WItemSelectionItem > WItemSelector::at( size_t index ) const
{
    return m_selection->at( getItemIndexOfSelected( index ) );
}

size_t WItemSelector::getItemIndexOfSelected( size_t index ) const
{
    return m_selected.at( index );
}

bool WItemSelector::empty() const
{
    return ( size() == 0 );
}

void WItemSelector::invalidate()
{
    m_valid = false;
}

bool WItemSelector::isValid() const
{
    return m_valid;
}

WItemSelector WItemSelector::createSelector( const IndexList& selected ) const
{
    WItemSelector s = WItemSelector( m_selection, selected );
    return s;
}

void WItemSelector::lock()
{
    // NOTE: it is not needed to check whether lock() has been called earlier. The old lock gets freed in the moment m_lock gets overwritten as
    // ReadTickets are reference counted.
    m_lock = m_selection->getReadTicket();
}

void WItemSelector::unlock()
{
    m_lock.reset();
}

WItemSelector::operator unsigned int() const
{
    return getItemIndexOfSelected( 0 );
}

WItemSelector::IndexList WItemSelector::getIndexList() const
{
    return m_selected;
}

