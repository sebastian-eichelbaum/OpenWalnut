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

#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/math/WLine.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "WBresenhamDBL.h"

WBresenhamDBL::WBresenhamDBL( boost::shared_ptr< WGridRegular3D > grid, bool antialiased )
    : WBresenham( grid, antialiased )
{
}

WBresenhamDBL::~WBresenhamDBL()
{
}

void WBresenhamDBL::rasterSegment( const WPosition& start, const WPosition& end )
{
    int i;
    WVector3i gridStartPos = m_grid->getVoxelCoord( start );
    WVector3i gridEndPos = m_grid->getVoxelCoord( end );
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
    WVector3i voxel = gridStartPos;
    WPosition gridOffset( 0, 0, 0 );
    gridOffset[0] = start[0] - gridStartPos[0];
    gridOffset[1] = start[1] - gridStartPos[1];
    gridOffset[2] = start[2] - gridStartPos[2];

    if( ( l >= m ) && ( l >= n ) )
    {
        err_1 = dy2 - l + gridOffset[1];
        err_2 = dz2 - l + gridOffset[2];
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
        err_1 = dx2 - m + gridOffset[0];
        err_2 = dz2 - m + gridOffset[2];
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
        err_1 = dy2 - n + gridOffset[1];
        err_2 = dx2 - n + gridOffset[0];
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
