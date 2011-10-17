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
#include <cmath>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/common/math/WLine.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/math/WValue.h"
#include "core/common/WAssert.h"
#include "core/common/WLogger.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "WBresenham.h"

WBresenham::WBresenham( boost::shared_ptr< WGridRegular3D > grid, bool antialiased )
    : WRasterAlgorithm( grid ),
      m_antialiased( antialiased )
{
}

WBresenham::~WBresenham()
{
}

void WBresenham::raster( const WLine& line )
{
    // lock the parameterization list for reading
    boost::shared_lock< boost::shared_mutex > lock =  boost::shared_lock< boost::shared_mutex >( m_parameterizationsLock );

    newLine( line );

    if( line.size() < 1 )
    {
        wlog::debug( "WBresenham" ) << "Tried to raster an empty line!, skipped.";
    }
    for( size_t i = 1; i < line.size(); ++i )
    {
        newSegment( line[i-1], line[i] );
        rasterSegment( line[i-1], line[i] );
    }

    lock.unlock();
}

void WBresenham::rasterSegment( const WPosition& start, const WPosition& end )
{
    int i;
    WValue< int > gridStartPos = m_grid->getVoxelCoord( start );
    WValue< int > gridEndPos = m_grid->getVoxelCoord( end );
    int dx = gridEndPos[0] - gridStartPos[0];
    int dy = gridEndPos[1] - gridStartPos[1];
    int dz = gridEndPos[2] - gridStartPos[2];
    int l = std::abs( dx );
    int m = std::abs( dy );
    int n = std::abs( dz );
    int x_inc = ( dx < 0 ) ? -1 : 1;
    int y_inc = ( dy < 0 ) ? -1 : 1;
    int z_inc = ( dz < 0 ) ? -1 : 1;
    double err_1, err_2;
    int dx2 = l << 1;
    int dy2 = m << 1;
    int dz2 = n << 1;
    WValue< int > voxel = gridStartPos;

    if( ( l >= m ) && ( l >= n ) )
    {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for( i = 0; i < l; i++ )
        {
            markVoxel( voxel, 0, start, end );
            if( err_1 > 0 )
            {
                voxel[1] += y_inc;
                err_1 -= dx2;
            }
            if( err_2 > 0 )
            {
                voxel[2] += z_inc;
                err_2 -= dx2;
            }
            // end of antialiased if-else
            err_1 += dy2;
            err_2 += dz2;
            voxel[0] += x_inc;
        }
    }
    else if( ( m >= l ) && ( m >= n ) )
    {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for( i = 0; i < m; i++ )
        {
            markVoxel( voxel, 1, start, end );
            if( err_1 > 0 )
            {
                voxel[0] += x_inc;
                err_1 -= dy2;
            }
            if( err_2 > 0 )
            {
                voxel[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            voxel[1] += y_inc;
        }
    }
    else
    {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for( i = 0; i < n; i++ )
        {
            markVoxel( voxel, 2, start, end );
            if( err_1 > 0 )
            {
                voxel[1] += y_inc;
                err_1 -= dz2;
            }
            if( err_2 > 0 )
            {
                voxel[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            voxel[2] += z_inc;
        }
    }
    markVoxel( voxel, -1, start, end );
}

std::vector< double > WBresenham::computeDistances( const size_t voxelNum,
                                                    const WPosition& start,
                                                    const WPosition& end ) const
{
    WPosition u = end - start;
    u = normalize( u );

    std::vector< WPosition > x;
    x.reserve( 7 );
    x.push_back( m_grid->getPosition( voxelNum ) - start );
    x.push_back( m_grid->getPosition( voxelNum + 1 ) - start );
    x.push_back( m_grid->getPosition( voxelNum - 1 ) - start );
    x.push_back( m_grid->getPosition( voxelNum + m_grid->getNbCoordsX() ) - start );
    x.push_back( m_grid->getPosition( voxelNum - m_grid->getNbCoordsX() ) - start );
    x.push_back( m_grid->getPosition( voxelNum + m_grid->getNbCoordsX() * m_grid->getNbCoordsY() ) - start );
    x.push_back( m_grid->getPosition( voxelNum - m_grid->getNbCoordsX() * m_grid->getNbCoordsY() ) - start );

    std::vector< double > result;
    result.reserve( 7 );

    // now calculate distance from x to the line given via start and end
    for( size_t i = 0; i < x.size(); ++i )
    {
        WPosition lot = dot( u, x[i] ) * u; // lot == perpendicular
        result.push_back( std::abs( length( x[i] - lot ) ) );
    }
    return result;
}

double WBresenham::composeValue( double newValue, double existingValue ) const
{
    return std::max( newValue, existingValue );
    // return newValue + existingValue;
}

void WBresenham::markVoxel( const WValue< int >& voxel, const int axis, const WPosition& start, const WPosition& end )
{
    size_t nbX  = m_grid->getNbCoordsX();
    size_t nbXY = m_grid->getNbCoordsX() * m_grid->getNbCoordsY();
    int x = voxel[0];
    int y = voxel[1];
    int z = voxel[2];
    size_t idx = x + y * nbX + z * nbXY;
    std::vector< double > distances;

    if( m_antialiased )
    {
        distances = computeDistances( idx, start, end );
        m_values[ idx ] = composeValue( filter( distances[0] ), m_values[ idx ] );
        parameterizeVoxel( voxel, idx, axis, m_values[ idx ], start, end );
    }
    else
    {
        m_values[ idx ] = 1.0;
        parameterizeVoxel( voxel, idx, axis, m_values[ idx ], start, end );
        return;
    }

    // if the voxel is on a "border" of the dataset antialiasing would write over the bounds
    // hence if the voxel is at least "margin" positions away from the border antialiasing
    // takes place, but not at the borders
    // ATM we have just a fixed margin of size 1
    int margin = 1;
    int nbXInt = m_grid->getNbCoordsX();
    int nbYInt = m_grid->getNbCoordsY();
    int nbZInt = m_grid->getNbCoordsZ();
    if( voxel[0] < margin || voxel[0] >= nbXInt - 1 ||
        voxel[1] < margin || voxel[1] >= nbYInt - 1 ||
        voxel[2] < margin || voxel[2] >= nbZInt - 1
      )
    {
        return; // otherwise we would write over the bounds
    }

    if( axis == -1 ) // just mark ONE voxel even in antialising mode!
    {
        return;
    }

    WAssert( distances.size() == 7, "There is an invalid number of precomputed antialiased voxels" );
    switch( axis )
    {
        case 0 :
                m_values[ idx + nbX ] = composeValue( filter( distances[3] ), m_values[ idx + nbX ] );
                m_values[ idx - nbX ] = composeValue( filter( distances[4] ), m_values[ idx - nbX ] );
                m_values[ idx + nbXY ] = composeValue( filter( distances[5] ), m_values[ idx + nbXY ] );
                m_values[ idx - nbXY ] = composeValue( filter( distances[6] ), m_values[ idx - nbXY ] );

                parameterizeVoxel( voxel, idx + nbX, axis, m_values[ idx + nbX ], start, end );
                parameterizeVoxel( voxel, idx - nbX, axis, m_values[ idx - nbX ], start, end );
                parameterizeVoxel( voxel, idx + nbXY, axis, m_values[ idx + nbXY ], start, end );
                parameterizeVoxel( voxel, idx - nbXY, axis, m_values[ idx - nbXY ], start, end );

                break;
        case 1 :
                m_values[ idx + 1 ] = composeValue( filter( distances[1] ), m_values[ idx + 1 ] );
                m_values[ idx - 1 ] = composeValue( filter( distances[2] ), m_values[ idx - 1 ] );
                m_values[ idx + nbXY ] = composeValue( filter( distances[5] ), m_values[ idx + nbXY ] );
                m_values[ idx - nbXY ] = composeValue( filter( distances[6] ), m_values[ idx - nbXY ] );

                parameterizeVoxel( voxel, idx + 1, axis, m_values[ idx + 1 ], start, end );
                parameterizeVoxel( voxel, idx - 1, axis, m_values[ idx - 1 ], start, end );
                parameterizeVoxel( voxel, idx + nbXY, axis, m_values[ idx + nbXY ], start, end );
                parameterizeVoxel( voxel, idx - nbXY, axis, m_values[ idx - nbXY ], start, end );

                break;
        case 2 :
                m_values[ idx + 1 ] = composeValue( filter( distances[1] ), m_values[ idx + 1 ] );
                m_values[ idx - 1 ] = composeValue( filter( distances[2] ), m_values[ idx - 1 ] );
                m_values[ idx + nbX ] = composeValue( filter( distances[3] ), m_values[ idx + nbX ] );
                m_values[ idx - nbX ] = composeValue( filter( distances[4] ), m_values[ idx - nbX ] );
                parameterizeVoxel( voxel, idx + 1, axis, m_values[ idx + 1 ], start, end );
                parameterizeVoxel( voxel, idx - 1, axis, m_values[ idx - 1 ], start, end );
                parameterizeVoxel( voxel, idx + nbX, axis, m_values[ idx + nbX ], start, end );
                parameterizeVoxel( voxel, idx - nbX, axis, m_values[ idx - nbX ], start, end );

                break;
        default : WAssert( 0, "Invalid axis selected for marking a voxel" );
    }
}

double WBresenham::filter( const double distance ) const
{
    WAssert( distance >= 0, "Negative distances are forbidden" );
    if( distance > 1 )
    {
        return 0.0;
    }
    size_t bucket = std::floor( distance * 10 );
    double tableData[] = { 1.0, 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1 };
    std::vector< double > lookUpTable( tableData, tableData + sizeof( tableData ) / sizeof( double ) );
    return lookUpTable.at( bucket );
}
