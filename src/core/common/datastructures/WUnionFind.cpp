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

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../exceptions/WOutOfBounds.h"
#include "WUnionFind.h"

WUnionFind::WUnionFind( size_t size )
    : m_component( size )
{
    for( size_t i = 0; i < size; ++i )
    {
        m_component[i] = i;
    }
}

size_t WUnionFind::find( size_t x )
{
    if( x >= m_component.size() )
    {
        throw WOutOfBounds( std::string( "Element index in UnionFind greater than overall elements!" ) );
    }
    if( m_component[x] == x ) // we are the root
    {
        return x;
    }

    m_component[x] = find( m_component[x] ); // path compression otherwise
    return m_component[x];
}

void WUnionFind::merge( size_t i, size_t j )
{
    if( i >= m_component.size() || j >= m_component.size() )
    {
        throw WOutOfBounds( std::string( "Element index in UnionFind greater than overall elements!" ) );
    }
    if( i == j )
    {
        return;
    }

    size_t ci = find( i );
    size_t cj = find( j );

    if( ci == cj )
    {
        return;
    }

    m_component[ ci ] = cj;
}

boost::shared_ptr< std::set< size_t > > WUnionFind::getMaxSet()
{
    std::map< size_t, std::set< size_t > > sets;
    size_t maxSetSizeSoFar = 0;
    size_t maxSetElement = 0;
    for( size_t i = 0; i < m_component.size(); ++i )
    {
        size_t cE = find( i ); // canonical Element
        sets[ cE ].insert( i );
        if( sets[ cE ].size() > maxSetSizeSoFar )
        {
            maxSetSizeSoFar = sets[ cE ].size();
            maxSetElement = cE;
        }
    }
    return boost::shared_ptr< std::set< size_t > >( new std::set< size_t >( sets[ maxSetElement ] ) );
}
