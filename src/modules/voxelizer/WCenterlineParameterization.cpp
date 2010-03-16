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

#include <iostream>

#include "WCenterlineParameterization.h"

WCenterlineParameterization::WCenterlineParameterization( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< wmath::WFiber > centerline ):
    WRasterParameterization( grid ),
    m_paramValues( grid->size(), 0.0 ),
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

boost::shared_ptr< WDataSetSingle > WCenterlineParameterization::getDataSet()
{
    boost::shared_ptr< WValueSet< double > > valueSet( new WValueSet< double >( 0, 1, m_paramValues, W_DT_DOUBLE ) );
    return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( valueSet, m_grid ) );
}

namespace
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
}

void WCenterlineParameterization::parameterizeVoxel( const wmath::WValue< int >& voxel, size_t /*voxelIdx*/, const int /*axis*/,
                                                      const double /*value*/,
                                                      const wmath::WPosition& /*start*/,
                                                      const wmath::WPosition& /*end*/ )
{
    // update a 27 neighbourhood
    size_t indices[ 27 ];
    indices[0]  = index( voxel[0],   voxel[1],   voxel[2],   m_grid );
    indices[1]  = index( voxel[0],   voxel[1],   voxel[2]+1, m_grid );
    indices[2]  = index( voxel[0],   voxel[1],   voxel[2]-1, m_grid );
    indices[3]  = index( voxel[0],   voxel[1]+1, voxel[2],   m_grid );
    indices[4]  = index( voxel[0],   voxel[1]+1, voxel[2]+1, m_grid );
    indices[5]  = index( voxel[0],   voxel[1]+1, voxel[2]-1, m_grid );
    indices[6]  = index( voxel[0],   voxel[1]-1, voxel[2],   m_grid );
    indices[7]  = index( voxel[0],   voxel[1]-1, voxel[2]+1, m_grid );
    indices[8]  = index( voxel[0],   voxel[1]-1, voxel[2]-1, m_grid );

    indices[9]  = index( voxel[0]+1, voxel[1],   voxel[2],   m_grid );
    indices[10] = index( voxel[0]+1, voxel[1],   voxel[2]+1, m_grid );
    indices[11] = index( voxel[0]+1, voxel[1],   voxel[2]-1, m_grid );
    indices[12] = index( voxel[0]+1, voxel[1]+1, voxel[2],   m_grid );
    indices[13] = index( voxel[0]+1, voxel[1]+1, voxel[2]+1, m_grid );
    indices[14] = index( voxel[0]+1, voxel[1]+1, voxel[2]-1, m_grid );
    indices[15] = index( voxel[0]+1, voxel[1]-1, voxel[2],   m_grid );
    indices[16] = index( voxel[0]+1, voxel[1]-1, voxel[2]+1, m_grid );
    indices[17] = index( voxel[0]+1, voxel[1]-1, voxel[2]-1, m_grid );

    indices[18] = index( voxel[0]-1, voxel[1],   voxel[2],   m_grid );
    indices[19] = index( voxel[0]-1, voxel[1],   voxel[2]+1, m_grid );
    indices[20] = index( voxel[0]-1, voxel[1],   voxel[2]-1, m_grid );
    indices[21] = index( voxel[0]-1, voxel[1]+1, voxel[2],   m_grid );
    indices[22] = index( voxel[0]-1, voxel[1]+1, voxel[2]+1, m_grid );
    indices[23] = index( voxel[0]-1, voxel[1]+1, voxel[2]-1, m_grid );
    indices[24] = index( voxel[0]-1, voxel[1]-1, voxel[2],   m_grid );
    indices[25] = index( voxel[0]-1, voxel[1]-1, voxel[2]+1, m_grid );
    indices[26] = index( voxel[0]-1, voxel[1]-1, voxel[2]-1, m_grid );

    // now update the neighbourhood
    for ( unsigned int i = 0; i < 27; ++i )
    {
        if ( m_paramSetValues[ indices[i] ] )
        {
            m_paramValues[ indices[i] ] = 0.5 * ( m_paramValues[ indices[i] ] + m_currentStartParameter );
        }
        else
        {
            m_paramValues[ indices[i] ] = m_currentStartParameter;
        }

        m_paramSetValues[ indices[i] ] = true;
    }
}

void WCenterlineParameterization::newLine( const wmath::WLine& line )
{
    // do nothing here
    WRasterParameterization::newLine( line );
}

void WCenterlineParameterization::newSegment( const wmath::WPosition& start, const wmath::WPosition& end )
{
    double curLength = 0.0;                     // the accumulated length along the centerline
    double bestDistStart = ( m_centerline->at( 0 ) - start ).norm();       // the currently best distance to the start point
    double bestDistEnd   = ( m_centerline->at( 0 ) - end   ).norm();       // the currently best distance to the end point

    // the currently best parameter of the centerline for this line segment
    m_currentStartParameter = 0.0;
    m_currentEndParameter = 0.0;

    // for this line segment, find the best matching vertex in the centerline
    for( size_t i = 1; i < m_centerline->size(); ++i )
    {
        curLength += ( m_centerline->at( i ) - m_centerline->at( i - 1 ) ).norm();

        // compare current distance
        double curDist = ( m_centerline->at( i ) - start ).norm();
        if ( bestDistStart >= curDist )
        {
            bestDistStart = curDist;
            m_currentStartParameter = curLength;
        }
        curDist = ( m_centerline->at( i ) - end ).norm();
        if ( bestDistEnd >= curDist )
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
}

