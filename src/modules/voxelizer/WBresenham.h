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

#include <cmath>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/math/WLine.h"
#include "core/common/math/linearAlgebra/WLinearAlgebra.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "WRasterAlgorithm.h"

/**
 * Implements basic Bresenham algorithm for rasterization.
 */
class WBresenham : public WRasterAlgorithm
{
friend class WBresenhamTest;
public:
    /**
     * Initializes new raster algo.
     *
     * \param grid The grid which defines the voxels which should be marked.
     * \param antialiased If true then all voxels of a line are supported with
     * anti-aliasing voxels around
     */
    WBresenham( boost::shared_ptr< WGridRegular3D > grid, bool antialiased = true );

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
    virtual void raster( const WLine& line );

protected:
    /**
     * Scans a line segment for voxels which are hit.
     *
     * \warning Not every voxel which is hit by the segment will be marked
     * but at least so many voxels so that the segment is represented by those
     * voxels.
     * \warning Every line starting in voxel A and ending in voxel B is
     * rastered the same way as the line from the middlepoint of A to the
     * middlepoint of B.
     *
     * \note This algorithm is fast since using only integer operations. This
     * is the real Bresenham
     *
     * \param start Start point of the line segment
     * \param end End point of the line segment
     */
    virtual void rasterSegment( const WPosition& start, const WPosition& end );

    /**
     * Marks the given voxel as a hit. If anti-aliasing is enabled also some
     * supporting voxels nearby are marked. The value for marking the voxel
     * depends on the distance from its center point to the real line.
     *
     * \param voxel The voxel to mark
     * \param axis Along which axis the traversal takes place. Since when
     * walking in e.g. X-direction there are not supporting voxels in the
     * same direction.
     * \param start Start point of the line segment (used to computed the
     * distance)
     * \param end End point of the line segment (used to computed the
     * distance)
     */
    virtual void markVoxel( const WVector3i& voxel, const int axis, const WPosition& start, const WPosition& end );

    /**
     * Returns the value to mark the hit voxels with, depending on their
     * distance to the line.
     *
     * \param distance Distance of the voxel to the line.
     *
     * \return Value which is used for marking a voxel.
     */
    virtual double filter( const double distance ) const;

    /**
     * Computes the distances for a voxel to the real line segment and also
     * for its supporting voxels.
     *
     * \param voxelNum The voxel number
     * \param start Start point of the line segment
     * \param end End point of the line segment
     *
     * \return A vector of distances where first distance is the distance of
     * the voxel itself, then x+1, x-1 supporting voxel, then y+1 and y-1
     * and at last z+1 and z-1.
     */
    std::vector< double > computeDistances( const size_t voxelNum, const WPosition& start, const WPosition& end ) const;

    /**
     * Compose the new value for a voxel out of a new computed value and the already existing marking.
     *
     * \param newValue Newly computed value
     * \param existingValue The mark already existing for the voxel (aka previous mark or markSoFar)
     *
     * \return The new mark for that voxel
     */
    double composeValue( double newValue, double existingValue ) const;

    bool m_antialiased; //!< If true also some supporting voxels are marked

private:
};

#endif  // WBRESENHAM_H
