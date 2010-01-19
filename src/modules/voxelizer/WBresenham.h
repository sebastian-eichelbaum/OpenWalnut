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

#ifndef WBRESENHAM_H
#define WBRESENHAM_H

#include <boost/shared_ptr.hpp>

#include "../../dataHandler/WGridRegular3D.h"
#include "../../math/WLine.h"
#include "WRasterAlgorithm.h"

/**
 * Implementes basic bresenham algorithm for rasterization.
 */
class WBresenham : public WRasterAlgorithm
{
public:
    /**
     * Initializes new raster algo.
     *
     * \param grid The grid which defines the voxels which should be marked.
     */
    explicit WBresenham( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Finishes this raster algo.
     */
    virtual ~WBresenham();

    /**
     * Rasterize the given line into the grid of dataset.
     * The value of the voxel which will be hit changes its value.
     *
     * \param line Polyline which is about to be rastered.
     */
    virtual void raster( const wmath::WLine& line );

protected:
    /**
     * Rasters a line segement of a polyline.
     *
     * \param start Start point of the line segement.
     * \param stop End point of the line segement.
     */
    void rasterSegment( const wmath::WValue< int >& start, const wmath::WValue< int >& stop );

    /**
     * Marks a given voxel as hit.
     *
     * \param voxel The voxel coordinates
     * \param intensity Marks the voxel with this intensity.
     */
    virtual void markVoxel( const wmath::WValue< int >& voxel, const double intensity = 1.0 );

private:
};

#endif  // WBRESENHAM_H
