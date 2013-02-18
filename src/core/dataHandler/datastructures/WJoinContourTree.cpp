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
#include <set>
#include <string>
#include <vector>

#include "../../common/WStringUtils.h"
#include "../../common/WTransferable.h"
#include "../../common/datastructures/WUnionFind.h"
#include "../../common/exceptions/WNotImplemented.h"
#include "../WValueSet.h"
#include "WJoinContourTree.h"

WJoinContourTree::WJoinContourTree()
    : WTransferable()
{
}

WJoinContourTree::WJoinContourTree( boost::shared_ptr< WDataSetSingle > dataset )
    : WTransferable(),
      m_elementIndices( dataset->getValueSet()->size() ),
      m_joinTree( dataset->getValueSet()->size() ),
      m_lowestVoxel( dataset->getValueSet()->size() )
{
    if( dataset->getValueSet()->order() != 0 || dataset->getValueSet()->dimension() != 1 )
    {
        throw WNotImplemented( std::string( "ATM there is only support for scalar fields" ) );
    }
    m_valueSet = boost::dynamic_pointer_cast< WValueSet< double > >( dataset->getValueSet() );
    if( !m_valueSet )
    {
        throw WNotImplemented( std::string( "ATM there is only support for scalar fields with doubles as scalars" ) );
    }
    m_grid = boost::dynamic_pointer_cast< WGridRegular3D >( dataset->getGrid() );
    if( !m_grid )
    {
        throw WNotImplemented( std::string( "Only WGridRegular3D is supported, despite that its not a simplicial mesh!" ) );
    }
    for( size_t i = 0; i < m_elementIndices.size(); ++i )
    {
        m_joinTree[i] = m_elementIndices[i] = i;
    }
}

void WJoinContourTree::sortIndexArray()
{
    IndirectCompare comp( m_valueSet );
    std::sort( m_elementIndices.begin(), m_elementIndices.end(), comp );
}

void WJoinContourTree::buildJoinTree()
{
    sortIndexArray();

    WUnionFind uf( m_joinTree.size() );

    for( size_t i = 0; i < m_joinTree.size(); ++i )
    {
        m_lowestVoxel[ m_elementIndices[i] ] = m_elementIndices[i];
        std::vector< size_t > neighbours = m_grid->getNeighbours( m_elementIndices[i] );
        std::vector< size_t >::const_iterator n = neighbours.begin();
        for( ; n != neighbours.end(); ++n )
        {
            if( uf.find( m_elementIndices[i] ) == uf.find( *n ) || m_valueSet->getScalar( *n ) <= m_valueSet->getScalar( m_elementIndices[i] ) )
            {
                continue;
            }
            else
            {
                uf.merge( m_elementIndices[i], *n );
                m_joinTree[ m_lowestVoxel[ uf.find( *n ) ] ] = m_elementIndices[i];
                m_lowestVoxel[ uf.find( *n ) ] = m_elementIndices[i];
            }
        }
    }
}

boost::shared_ptr< std::set< size_t > > WJoinContourTree::getVolumeVoxelsEnclosedByIsoSurface( const double isoValue ) const
{
    boost::shared_ptr< std::vector< size_t > > result( new std::vector< size_t >( m_elementIndices ) );
    WUnionFind uf( m_elementIndices.size() );

    // using string_utils::operator<<;
    // std::cout << "m_element: " << m_elementIndices << std::endl;

    //std::stringstream ss;
    // assume the m_elementIndices array is still sorted descending on its iso values in the valueset
    for( size_t i = 0; i < m_elementIndices.size() && m_valueSet->getScalar( m_elementIndices[i] ) >= isoValue; ++i )
    {
        // std::cout << "processing element: " << i << std::endl;
        // std::cout << "having index: " << m_elementIndices[i] << std::endl;
        // using string_utils::operator<<;
        // std::cout << "xyz: " << m_grid->getNeighbours( m_elementIndices[i] ) << std::endl;
        // std::cout << "having isovalue: " << m_valueSet->getScalar( m_elementIndices[i] ) << std::endl;
        // ss << " m_elementIndices[i]:isovalue=" << m_valueSet->getScalar( m_elementIndices[i] ) << ", ";
        size_t target = m_joinTree[ m_elementIndices[i] ];
        // std::cout << "having edge to: " << target << std::endl;
        if( m_valueSet->getScalar( target ) >= isoValue )
        {
            uf.merge( target, m_elementIndices[i] );
        }
    }
    //std::cout << ss.str() << std::endl;
    return uf.getMaxSet();
}

WJoinContourTree::IndirectCompare::IndirectCompare( boost::shared_ptr< WValueSet< double > > valueSet )
    : m_valueSet( valueSet )
{
}

bool WJoinContourTree::IndirectCompare::operator()( size_t i, size_t j )
{
    return ( m_valueSet->getScalar( i ) > m_valueSet->getScalar( j ) );
}

// make the class beeing a WTransferrable:
// ---------------------------------------
boost::shared_ptr< WPrototyped > WJoinContourTree::m_prototype = boost::shared_ptr< WPrototyped >();

boost::shared_ptr< WPrototyped > WJoinContourTree::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WJoinContourTree() );
    }
    return m_prototype;
}
