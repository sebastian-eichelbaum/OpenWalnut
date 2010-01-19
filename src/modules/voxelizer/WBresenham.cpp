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

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../math/WLine.h"
#include "../../math/WValue.h"
#include "WBresenham.h"

WBresenham::WBresenham( boost::shared_ptr< WGridRegular3D > grid )
    : WRasterAlgorithm( grid )
{
}

WBresenham::~WBresenham()
{
}

void WBresenham::raster( const wmath::WLine& line )
{
    assert( line.size() > 1 );
    for( size_t i = 1; i < line.size(); ++i )
    {
        wmath::WValue< int > start = m_grid->getVoxelCoord( line[i-1] );
        wmath::WValue< int > stop = m_grid->getVoxelCoord( line[i] );
        rasterSegment( start, stop );
    }
}

void WBresenham::rasterSegment( const wmath::WValue< int >& start, const wmath::WValue< int >& stop )
{
    int x1 = start[0];
    int y1 = start[1];
    int z1 = start[2];
    int x2 = stop[0];
    int y2 = stop[1];
    int z2 = stop[2];
    int i;
    int dx, dy, dz;
    int l, m, n;
    int x_inc, y_inc, z_inc;
    int err_1, err_2;
    int dx2, dy2, dz2;
    wmath::WValue< int > voxel( 3 );

    voxel[0] = x1;
    voxel[1] = y1;
    voxel[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = ( dx < 0 ) ? -1 : 1;
    l = std::abs( dx );
    y_inc = ( dy < 0 ) ? -1 : 1;
    m = std::abs( dy );
    z_inc = ( dz < 0 ) ? -1 : 1;
    n = std::abs( dz );
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;

    if( ( l >= m ) && ( l >= n ) )
    {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for( i = 0; i < l; i++ )
        {
            markVoxel( voxel );
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
            markVoxel( voxel );
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
            markVoxel( voxel );
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
    markVoxel( voxel );
}

void WBresenham::markVoxel( const wmath::WValue< int >& voxel, const double intensity )
{
    assert( voxel.size() == 3 );
    size_t idx = voxel[0] + voxel[1] * m_grid->getNbCoordsX() + voxel[2] * m_grid->getNbCoordsX() * m_grid->getNbCoordsY();
    m_values[ idx ] = intensity;
}
