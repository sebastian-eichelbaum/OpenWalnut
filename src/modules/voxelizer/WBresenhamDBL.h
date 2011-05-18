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

#ifndef WBRESENHAMDBL_H
#define WBRESENHAMDBL_H

#include <boost/shared_ptr.hpp>

#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/math/WLine.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "WBresenham.h"

/**
 * This is a modified version the Bresenham algorithm.
 */
class WBresenhamDBL : public WBresenham
{
friend class WBresenhamDBLTest;
public:
    /**
     * Initializes new raster algo.
     *
     * \param grid The grid which defines the voxels which should be marked.
     * \param antialiased If true then all voxels of a line are supported with
     * anti-aliasing voxels around
     */
    WBresenhamDBL( boost::shared_ptr< WGridRegular3D > grid, bool antialiased = true );

    /**
     * Finishes this raster algo.
     */
    virtual ~WBresenhamDBL();

protected:
    /**
     * Scans a line segment for voxels which are hit.
     *
     * \warning Not every voxel which is hit by the segment will be marked
     * but at least so many voxels so that the segment is represented by those
     * voxels.
     *
     * \note This algorithm may be slow since using double operations.
     * \param start Start point of the line segment
     * \param stop End point of the line segment
     */
    void rasterSegment( const WPosition& start, const WPosition& stop );

private:
};

#endif  // WBRESENHAMDBL_H
