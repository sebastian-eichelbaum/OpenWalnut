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

#include <boost/lexical_cast.hpp>

#include "WStringUtils.h"
#include "WItemSelection.h"

#include "WItemSelector.h"

WItemSelector::WItemSelector( boost::shared_ptr< WItemSelection > selection, IndexList selected ):
    m_selection( selection ),
    m_selected( selected )
{
    // initialize members
}

WItemSelector::~WItemSelector()
{
    // cleanup
}

WItemSelector WItemSelector::newSelector( IndexList selected ) const
{
    return WItemSelector( m_selection, selected );
}

WItemSelector WItemSelector::newSelector( const std::string asString ) const
{
    std::vector<std::string> tokens;
    tokens = string_utils::tokenize( asString, ";" );

    IndexList l;
    for ( size_t i = 0; i < tokens.size(); ++i )
    {
        l.push_back( boost::lexical_cast< size_t >( tokens[i] ) );
    }

    return newSelector( l );
}

std::ostream& WItemSelector::operator<<( std::ostream& out ) const
{
    for ( WItemSelector::IndexList::const_iterator iter = m_selected.begin(); iter != m_selected.end(); ++iter )
    {
        out << ( *iter );
        if ( ( iter + 1 ) != m_selected.end() )
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
    return ( ( m_selection == other.m_selection ) && ( m_selected == other.m_selected ) );
}

size_t WItemSelector::sizeAll() const
{
    return m_selection->size();
}

size_t WItemSelector::size() const
{
    return m_selected.size();
}

WItemSelection::Item WItemSelector::atAll( size_t index ) const
{
    return m_selection->at( index );
}

WItemSelection::Item WItemSelector::at( size_t index ) const
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

