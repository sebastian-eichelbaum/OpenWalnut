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
#include <iostream>
#include <vector>

#include "WCenterlineParameterization.h"

WCenterlineParameterization::WCenterlineParameterization( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WFiber > centerline ):
    WRasterParameterization( grid ),
    m_paramValues( grid->size(), 0.0 ),
    m_paramFinalValues( grid->size(), 0.0 ),
    m_paramSetValues( grid->size(), false ),
    m_centerline( centerline ),
    m_currentStartParameter( 0.0 ),
    m_currentEndParameter( 0.0 )
{
    // initialize members
}

WCenterlineParameterization::~WCenterlineParameterization()
{
    // cleanup
}

boost::shared_ptr< WDataSetScalar > WCenterlineParameterization::getDataSet()
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0,
                                                                                1,
                                                                                boost::shared_ptr< std::vector< double > >(
                                                                                    new std::vector< double >( m_paramFinalValues ) ),
                                                                                W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetScalar >( new WDataSetScalar( valueSet, m_grid ) );
}

namespace wcp // WCenterlineParameterization
{
    size_t index( int x, int y, int z, boost::shared_ptr< WGridRegular3D > grid )
    {
        // check validity of voxel
        x = x < 0 ? 0 : x;
        y = y < 0 ? 0 : y;
        z = z < 0 ? 0 : z;
        x = x >= static_cast< int >( grid->getNbCoordsX() ) ? static_cast< int >( grid->getNbCoordsX() ) - 1 : x;
        y = y >= static_cast< int >( grid->getNbCoordsY() ) ? static_cast< int >( grid->getNbCoordsY() ) - 1 : y;
        z = z >= static_cast< int >( grid->getNbCoordsZ() ) ? static_cast< int >( grid->getNbCoordsZ() ) - 1 : z;

        // calculate the index inside the grid
        size_t nbX  = grid->getNbCoordsX();
        size_t nbXY = grid->getNbCoordsX() * grid->getNbCoordsY();
        return x + y * nbX + z * nbXY;
    }

    /**
     * Neighborhood position
     */
    typedef struct
    {
        size_t indices[27]; //!< The indices of the neighbors.
    }
    Neighbourhood;

    Neighbourhood neighbourhood( int x, int y, int z, boost::shared_ptr< WGridRegular3D > grid )
    {
        Neighbourhood n;
        n.indices[0]  = index( x,   y,   z,   grid );
        n.indices[1]  = index( x,   y,   z+1, grid );
        n.indices[2]  = index( x,   y,   z-1, grid );
        n.indices[3]  = index( x,   y+1, z,   grid );
        n.indices[4]  = index( x,   y+1, z+1, grid );
        n.indices[5]  = index( x,   y+1, z-1, grid );
        n.indices[6]  = index( x,   y-1, z,   grid );
        n.indices[7]  = index( x,   y-1, z+1, grid );
        n.indices[8]  = index( x,   y-1, z-1, grid );

        n.indices[9]  = index( x+1, y,   z,   grid );
        n.indices[10] = index( x+1, y,   z+1, grid );
        n.indices[11] = index( x+1, y,   z-1, grid );
        n.indices[12] = index( x+1, y+1, z,   grid );
        n.indices[13] = index( x+1, y+1, z+1, grid );
        n.indices[14] = index( x+1, y+1, z-1, grid );
        n.indices[15] = index( x+1, y-1, z,   grid );
        n.indices[16] = index( x+1, y-1, z+1, grid );
        n.indices[17] = index( x+1, y-1, z-1, grid );

        n.indices[18] = index( x-1, y,   z,   grid );
        n.indices[19] = index( x-1, y,   z+1, grid );
        n.indices[20] = index( x-1, y,   z-1, grid );
        n.indices[21] = index( x-1, y+1, z,   grid );
        n.indices[22] = index( x-1, y+1, z+1, grid );
        n.indices[23] = index( x-1, y+1, z-1, grid );
        n.indices[24] = index( x-1, y-1, z,   grid );
        n.indices[25] = index( x-1, y-1, z+1, grid );
        n.indices[26] = index( x-1, y-1, z-1, grid );

        return n;
    }
}

void WCenterlineParameterization::parameterizeVoxel( const WValue< int >& voxel, size_t /*voxelIdx*/, const int /*axis*/,
                                                      const double /*value*/,
                                                      const WPosition& /*start*/,
                                                      const WPosition& /*end*/ )
{
    // update a 27 neighbourhood
    wcp::Neighbourhood n = wcp::neighbourhood( voxel[0], voxel[1], voxel[2], m_grid );

    // now update the neighbourhood
    for( unsigned int i = 0; i < 27; ++i )
    {
        if( m_paramSetValues[ n.indices[i] ] )
        {
            m_paramValues[ n.indices[i] ] = 0.5 * ( m_paramValues[ n.indices[i] ] + m_currentStartParameter );
            //m_paramValues[ n.indices[i] ] = std::max( m_paramValues[ n.indices[i] ], m_currentStartParameter );
        }
        else
        {
            m_paramValues[ n.indices[i] ] = m_currentStartParameter;
        }

        m_paramSetValues[ n.indices[i] ] = true;
    }
}

void WCenterlineParameterization::newLine( const WLine& line )
{
    // do nothing here
    WRasterParameterization::newLine( line );
}

void WCenterlineParameterization::newSegment( const WPosition& start, const WPosition& end )
{
    double curLength = 0.0;                     // the accumulated length along the centerline
    double bestDistStart = length( m_centerline->at( 0 ) - start );       // the currently best distance to the start point
    double bestDistEnd   = length( m_centerline->at( 0 ) - end   );       // the currently best distance to the end point

    // the currently best parameter of the centerline for this line segment
    m_currentStartParameter = 0.0;
    m_currentEndParameter = 0.0;

    // for this line segment, find the best matching vertex in the centerline
    for( size_t i = 1; i < m_centerline->size(); ++i )
    {
        curLength += length( m_centerline->at( i ) - m_centerline->at( i - 1 ) );

        // compare current distance
        double curDist = length( m_centerline->at( i ) - start );
        if( bestDistStart >= curDist )
        {
            bestDistStart = curDist;
            m_currentStartParameter = curLength;
        }
        curDist = length( m_centerline->at( i ) - end );
        if( bestDistEnd >= curDist )
        {
            bestDistEnd = curDist;
            m_currentEndParameter = curLength;
        }
    }

    m_currentStartParameter = m_currentStartParameter;

    WRasterParameterization::newSegment( start, end );
}

void WCenterlineParameterization::finished()
{
    // do some selective dilatation on the final grid

    for( size_t x = 0; x < m_grid->getNbCoordsX(); ++x )
    {
        for( size_t y = 0; y < m_grid->getNbCoordsY(); ++y )
        {
            for( size_t z = 0; z < m_grid->getNbCoordsZ(); ++z )
            {
                size_t idx = wcp::index( x, y, z, m_grid );

                // copy
                m_paramFinalValues[ idx ] = m_paramValues[ idx ];

                // has been set during rasterization?
                if( !m_paramSetValues[ idx ] )
                {
                    m_paramSetValues[ idx ] = true;

                    // find maximum in neighbourhood
                    wcp::Neighbourhood n = wcp::neighbourhood( x, y, z, m_grid );

                    double maxVal = m_paramValues[ n.indices[ 0 ] ];
                    for( unsigned int i = 1; i < 27; ++i )
                    {
                        maxVal = std::max( m_paramValues[ n.indices[i] ], maxVal );
                    }

                    m_paramFinalValues[ idx ] = maxVal;
                }
            }
        }
    }
}

