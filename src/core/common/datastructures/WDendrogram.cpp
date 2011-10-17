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

#include "../exceptions/WOutOfBounds.h"
#include "../WAssert.h"
#include "../WLogger.h"
#include "../WStringUtils.h"
#include "WDendrogram.h"

// init _static_ member variable and provide a linker reference to it
boost::shared_ptr< WPrototyped > WDendrogram::m_prototype = boost::shared_ptr< WPrototyped >();

boost::shared_ptr< WPrototyped > WDendrogram::getPrototype()
{
    if( !m_prototype )
    {
         m_prototype = boost::shared_ptr< WPrototyped >( new WDendrogram() );
    }
    return m_prototype;
}

WDendrogram::WDendrogram()
    : WTransferable(),
      m_parents( 0 ),
      m_heights( 0 )
{
}

WDendrogram::WDendrogram( size_t n )
    : WTransferable()
{
    reset( n );
}

void WDendrogram::reset( size_t n )
{
    WAssert( n > 0, "A Dendrogram for an empty set of objects was created which makes no sence, if your really need it implement it!" );
    m_heights.resize( n - 1, 0.0 );
    m_parents.reserve( 2 * n - 1 );
    m_parents.resize( n, 0 );
}

void WDendrogram::checkAndThrowExceptionIfUsedUninitialized( std::string caller ) const
{
    if( m_parents.empty() )
    {
        throw WOutOfBounds( "Accessed an empty WDendrogam via a call to a memeber function: " + caller + " which needs access to elements." );
    }
}

size_t WDendrogram::merge( size_t i, size_t j, double height )
{
    checkAndThrowExceptionIfUsedUninitialized( "merge(...)" );

#ifdef DEBUG
    std::stringstream errMsg;
    errMsg << "Bug: n=" << m_heights.size() << " many leafs can lead maximal to 2n-1 many nodes in a tree but this was violated now!" << std::endl;
    WAssert( m_parents.size() != 2 * m_heights.size() + 1, errMsg.str() );
#endif

    m_parents.push_back( m_parents.size() ); // the root s always self referencing

    m_heights[ m_parents.size() - 2 - m_heights.size() ] = height;
    m_parents[i] = m_parents.back();
    m_parents[j] = m_parents.back();

    return m_parents.size() - 1;
}

std::string WDendrogram::toString() const
{
    checkAndThrowExceptionIfUsedUninitialized( "toString()" );

    std::stringstream result;
    std::map< size_t, std::vector< size_t > > childsOfInnerNodes;
    std::map< size_t, std::vector< size_t > > preds;
    std::vector< size_t > level( 2 * m_heights.size() + 1, 0 );

    // For very insane debugging only:
    // wlog::debug( "WDendrogram" ) << "nodes size: " << m_parents.size() << " and expeceted: " << 2 * m_heights.size() + 1;
    // wlog::debug( "WDendrogram" ) << "Parents-ARRAY:";
    // wlog::debug( "WDendrogram" ) << m_parents;
    // wlog::debug( "WDendrogram" ) << "Heights-ARRAY:";
    // wlog::debug( "WDendrogram" ) << m_heights;

    // first write out all fibers
    for( size_t i = 0; i < m_heights.size() + 1; ++i )
    {
        result << "(0, (" << i << ",))" << std::endl;
        childsOfInnerNodes[ m_parents[i] ].push_back( i );
        preds[ m_parents[i] ].push_back( i );
    }
    for( size_t i = m_heights.size() + 1; i < 2 * m_heights.size() + 1; ++i )
    {
        // using string_utils::operator<<;
        // wlog::debug( "WDendrogram" ) << "i: " << i;
        // wlog::debug( "WDendrogram" ) << "pred[i]: " << preds[i];
        // wlog::debug( "WDendrogram" ) << "childs[i]: " << childsOfInnerNodes[i];
        // wlog::debug( "WDendrogram" ) << "parentchilds: " << childsOfInnerNodes[ m_parents[ i ] ];

        size_t left = *( preds[ i ].begin() );
        size_t right = *( preds[ i ].rbegin() );
        level[i] = std::max( level[left], level[right] ) + 1;
        if( i != m_parents[i] )
        {
            preds[ m_parents[ i ] ].push_back( i );
            childsOfInnerNodes[ m_parents[i] ].reserve( childsOfInnerNodes[ m_parents[i] ].size() + childsOfInnerNodes[i].size() );
            childsOfInnerNodes[ m_parents[i] ].insert(  childsOfInnerNodes[ m_parents[i] ].end(), childsOfInnerNodes[i].begin(),
                    childsOfInnerNodes[i].end() );
        }
        result << "(" << level[i] << ", (";
        size_t numElements = childsOfInnerNodes[i].size();
        for( std::vector< size_t >::const_iterator cit = childsOfInnerNodes[i].begin(); cit != childsOfInnerNodes[i].end(); ++cit )
        {
            if( numElements == 1 )
            {
                result << *cit << "), ";
            }
            else
            {
                result << *cit << ", ";
            }
            numElements -= 1;
        }
        // wlog::debug( "WDendrogram" ) << "i: " << i;
        // wlog::debug( "WDendrogram" ) << "pred[i]: " << preds[i];
        // wlog::debug( "WDendrogram" ) << "childs[i]: " << childsOfInnerNodes[i];
        // wlog::debug( "WDendrogram" ) << "parentchilds: " << childsOfInnerNodes[ m_parents[ i ] ];
        result << "(" << left << ", " << right << "), " << m_heights[ i - m_heights.size() - 1 ] << ")" << std::endl;
    }

    return result.str();
}
