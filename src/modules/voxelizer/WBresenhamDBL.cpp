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
#include "../../math/WPosition.h"
#include "WBresenhamDBL.h"

WBresenhamDBL::WBresenhamDBL( boost::shared_ptr< WGridRegular3D > grid )
    : WRasterAlgorithm( grid )
{
}

WBresenhamDBL::~WBresenhamDBL()
{
}

void WBresenhamDBL::raster( const wmath::WLine& line )
{
    assert( line.size() > 1 );
    for( size_t i = 1; i < line.size(); ++i )
    {
        rasterSegment( line[i-1], line[i] );
    }
}

void WBresenhamDBL::rasterSegment( const wmath::WPosition& start, const wmath::WPosition& stop )
{
    double x1 = start[0];
    double y1 = start[1];
    double z1 = start[2];
    double x2 = stop[0];
    double y2 = stop[1];
    double z2 = stop[2];
    int i;
    double dx, dy, dz;
    double l, m, n;
    int x_inc, y_inc, z_inc;
    double err_1, err_2;
    double dx2, dy2, dz2;
    wmath::WPosition voxel = start;

    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = ( dx < 0 ) ? -1 : 1;
    l = std::abs( dx );
    y_inc = ( dy < 0 ) ? -1 : 1;
    m = std::abs( dy );
    z_inc = ( dz < 0 ) ? -1 : 1;
    n = std::abs( dz );
    dx2 = 2 * l;
    dy2 = 2 * m;
    dz2 = 2 * n;

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
