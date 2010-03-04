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

#include "../../dataHandler/WGridRegular3D.h"
#include "../../common/math/WLine.h"
#include "../../common/math/WPosition.h"
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
     * antialiasing voxels around
     */
    WBresenhamDBL( boost::shared_ptr< WGridRegular3D > grid, bool antialiased = true );

    /**
     * Finishes this raster algo.
     */
    virtual ~WBresenhamDBL();

protected:
    /**
     * Scans a line segement for voxels which are hit.
     *
     * \warning Not every voxel which is hitten by the segement will be marked
     * but at least so many voxels so that the segement is represented by those
     * voxels.
     *
     * \note This algorithm may be slow since using double operations.
     * \param start Start point of the line segement
     * \param stop End point of the line segement
     */
    void rasterSegment( const wmath::WPosition& start, const wmath::WPosition& stop );

private:
};

#endif  // WBRESENHAMDBL_H
