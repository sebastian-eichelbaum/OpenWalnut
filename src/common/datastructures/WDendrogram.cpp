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
#include <iterator>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

#include "../WAssert.h"
#include "../WLogger.h"
#include "../WStringUtils.h"
#include "WDendrogram.h"

WDendrogram::WDendrogram( size_t n )
    : m_heights( n - 1, 0.0 )
{
    m_parents.reserve( 2 * n - 1 );
    m_parents.resize( n, 0 );
}

size_t WDendrogram::merge( size_t i, size_t j, double height )
{
#ifdef DEBUG
    std::stringstream ss;
    ss << "Bug: n=" << m_heights.size() << " many leafs can lead maximal to 2n-1 many nodes in a tree but this was violated now!" << std::endl;
    WAssert( m_parents.size() < 2 * m_heights.size() + 1, ss.str() );
#endif

    m_parents.push_back( m_parents.size() ); // the root s always self referencing

#ifdef DEBUG
    m_heights.at( m_parents.size() - 2 - m_heights.size() ) = height;
    m_parents.at( i ) = m_parents.size() - 1;
    m_parents.at( j ) = m_parents.size() - 1;
#else
    m_heights[ m_parents.size() - 2 - m_heights.size() ] = height;
    m_parents[ i ] = m_parents.back();
    m_parents[ j ] = m_parents.back();
#endif

    return m_parents.size() - 1;
}

std::string WDendrogram::toTXTString() const
{
    std::stringstream ss;

    std::map< size_t, std::set< size_t > > childsOfInnerNodes;
    std::map< size_t, std::set< size_t > > preds;
    std::vector< size_t > level( 2 * m_heights.size() + 1, 0 );

    // first write out all fibers
    for( size_t i = 0; i < m_heights.size() + 1; ++i )
    {
        ss << "(0, (" << i << ",))" << std::endl;
        childsOfInnerNodes[ m_parents[ i ] ].insert( i );
        preds[ m_parents[ i ] ] = childsOfInnerNodes[ m_parents[ i ] ];
    }
    for( size_t i = m_heights.size() + 1; i < 2 * m_heights.size() + 1; ++i )
    {
        WAssert( preds[ i ].size() == 2, "There are more or less than 2 predecessors for an inner node" );
        size_t left = *( preds[ i ].begin() );
        size_t right = *( preds[ i ].rbegin() );
        level[ i ] = std::max( level[ left ], level[ right ] ) + 1;
        preds[ m_parents[ i ] ].insert( i );
        std::set< size_t > join;
        std::set_union( childsOfInnerNodes[ m_parents[ i ] ].begin(), childsOfInnerNodes[ m_parents[ i ] ].end(),
                childsOfInnerNodes[ i ].begin(), childsOfInnerNodes[ i ].end(), std::inserter( join, join.begin() ) );
        childsOfInnerNodes[ m_parents[ i ] ] = join;
        ss << "(" << level[i] << ", (";
        size_t numElements = childsOfInnerNodes[i].size();
        for( std::set< size_t >::const_iterator cit = childsOfInnerNodes[i].begin(); cit != childsOfInnerNodes[i].end(); ++cit )
        {
            if( numElements == 1 )
            {
                ss << *cit << "), ";
            }
            else
            {
                ss << *cit << ", ";
            }
            numElements -= 1;
        }
        using string_utils::operator<<;
        ss << "(" << left << ", " << right << "), " << m_heights[ i - m_heights.size() - 1 ] << ")" << std::endl;
    }

    // TODO(math): the needs to be made with a writer instead
    wlog::debug( "WDendrogram" ) << "start writing the txt file to pansen.txt";
    std::ofstream file( "/home/math/pansen.txt" );
    file << ss.str();
    file.close();
    wlog::debug( "WDendrogram" ) << "written the txt file to pansen.txt";

    return ss.str();
}
