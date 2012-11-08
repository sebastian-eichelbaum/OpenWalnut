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
#include <string>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/WAssert.h"
#include "../common/WColor.h"
#include "../common/math/linearAlgebra/WPosition.h"

#include "WDataSetPoints.h"

// prototype instance as singleton
boost::shared_ptr< WPrototyped > WDataSetPoints::m_prototype = boost::shared_ptr< WPrototyped >();

WDataSetPoints::WDataSetPoints( WDataSetPoints::VertexArray vertices,
                                WDataSetPoints::ColorArray colors,
                                WBoundingBox boundingBox ):
    m_vertices( vertices ),
    m_colors( colors ),
    m_bb( boundingBox )
{
    WAssert( vertices->size() % 3 == 0, "Number of floats in the vertex array must be a multiple of 3" );
    if( colors )
    {
        WAssert( colors->size() / 4 == vertices->size() / 3, "Number of floats in the color array must be 4 per vertex" );
    }

    init();
}

WDataSetPoints::WDataSetPoints( WDataSetPoints::VertexArray vertices,
                                WDataSetPoints::ColorArray colors ):
    m_vertices( vertices ),
    m_colors( colors )
{
    WAssert( vertices->size() % 3 == 0, "Number of floats in the vertex array must be a multiple of 3" );
    if( colors )
    {
        WAssert( colors->size() / 4 == vertices->size() / 3, "Number of floats in the color array must be 4 per vertex" );
    }

    init( true );
}

WDataSetPoints::WDataSetPoints()
{
    // dummy construction. Empty point and color list
}

WDataSetPoints::~WDataSetPoints()
{
    // cleanup
}

void WDataSetPoints::init( bool calcBB )
{
    // no colors specified? Use white as default
    if( !m_colors )
    {
        // Store 4 values for each point
        m_colors = ColorArray( new ColorArray::element_type( m_vertices->size() / 3 * 4, 1.0 ) );
    }

    // calculate the bounding box if needed
    if( calcBB && ( m_vertices->size() != 0 ) )
    {
        float minX = m_vertices->operator[]( 0 );
        float minY = m_vertices->operator[]( 1 );
        float minZ = m_vertices->operator[]( 2 );
        float maxX = minX;
        float maxY = minY;
        float maxZ = minZ;

        // go through each point
        for( size_t pointIdx = 1; pointIdx < m_vertices->size(); ++pointIdx )
        {
            minX = std::min( m_vertices->operator[]( pointIdx * 3 + 0 ), minX );
            minY = std::min( m_vertices->operator[]( pointIdx * 3 + 1 ), minY );
            minZ = std::min( m_vertices->operator[]( pointIdx * 3 + 2 ), minZ );
            maxX = std::max( m_vertices->operator[]( pointIdx * 3 + 0 ), maxX );
            maxY = std::max( m_vertices->operator[]( pointIdx * 3 + 1 ), maxY );
            maxZ = std::max( m_vertices->operator[]( pointIdx * 3 + 2 ), maxZ );
        }

        m_bb = WBoundingBox( minX, minY, minZ, maxX, maxY, maxZ );
    }
}

size_t WDataSetPoints::size() const
{
    return m_vertices->size() / 3;
}

bool WDataSetPoints::isTexture() const
{
    return false;
}

const std::string WDataSetPoints::getName() const
{
    return "WDataSetPoints";
}

const std::string WDataSetPoints::getDescription() const
{
    return "Dataset which contains points without any topological relation.";
}

boost::shared_ptr< WPrototyped > WDataSetPoints::getPrototype()
{
    if( !m_prototype )
    {
        m_prototype = boost::shared_ptr< WPrototyped >( new WDataSetPoints() );
    }

    return m_prototype;
}

WDataSetPoints::VertexArray WDataSetPoints::getVertices() const
{
    return m_vertices;
}

WDataSetPoints::ColorArray WDataSetPoints::getColors() const
{
    return m_colors;
}

WBoundingBox WDataSetPoints::getBoundingBox() const
{
    return m_bb;
}

WPosition WDataSetPoints::operator[]( const size_t pointIdx ) const
{
    if( pointIdx * 3 > m_vertices->size() - 3 )
    {
        throw WOutOfBounds( "The specified index is invalid." );
    }

    return WPosition( m_vertices->operator[]( pointIdx * 3 + 0 ),
                      m_vertices->operator[]( pointIdx * 3 + 1 ),
                      m_vertices->operator[]( pointIdx * 3 + 2 ) );
}

WColor WDataSetPoints::getColor( const size_t pointIdx ) const
{
    if( pointIdx * 4 > m_colors->size() - 4 )
    {
        throw WOutOfBounds( "The specified index is invalid." );
    }

    return WColor( m_colors->operator[]( pointIdx * 4 + 0 ),
                   m_colors->operator[]( pointIdx * 4 + 1 ),
                   m_colors->operator[]( pointIdx * 4 + 2 ),
                   m_colors->operator[]( pointIdx * 4 + 3 ) );
}
