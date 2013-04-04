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

#ifndef WRASTERPARAMETERIZATION_H
#define WRASTERPARAMETERIZATION_H

#include "core/common/math/WLine.h"
#include "core/common/math/linearAlgebra/WPosition.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/dataHandler/WDataSetScalar.h"

/**
 * This class is the base for all specific parameterization algorithms. Derive from it to write your own class, which is able to create a new
 * dataset basing on the parameterization of several lines. This might be useful for datasets which contain the current fiber direction at a
 * voxel, datasets integrating the length of fibers along a fiber or parameterizing the volume using the centerline.
 */
class WRasterParameterization
{
public:
    /**
     * Default constructor. It needs a grid as the parameterization volume HAS to be in the same grid.
     *
     * \param grid the grid to use for parameterization.
     */
    explicit WRasterParameterization( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Destructor.
     */
    virtual ~WRasterParameterization();

    /**
     * This method allows this parameterization to update. It gets called for every voxel which is rasterized by the WRasterAlgorithm.
     *
     * \param voxel the voxel to parameterize
     * \param voxelIdx the voxel index in the common grid calculated using "voxel" (this is for convenience)
     * \param axis  Along which axis the traversal takes place. Since when walking in e.g. X-direction there are not supporting voxels in the
     * \param value the new voxel value
     * \param start Start point of the line segment (used to computed the distance)
     * \param end   End point of the line segment (used to computed the distance)
     */
    virtual void parameterizeVoxel( const WVector3i& voxel, size_t voxelIdx, const int axis, const double value,
                                    const WPosition& start,
                                    const WPosition& end ) = 0;

    /**
     * Gets the dataset representing the parameterization.
     *
     * \return the dataset.
     */
    virtual boost::shared_ptr< WDataSetScalar > getDataSet() = 0;

    /**
     * Gets called for each new line getting rasterized.
     *
     * \param line the new line.
     */
    virtual void newLine( const WLine& line );

    /**
     * Gets called for each new line segment getting rasterized, as one segment can have multiple voxels.
     *
     * \param start start point of the new line segment
     * \param end end point of the new line segment
     */
    virtual void newSegment( const WPosition& start, const WPosition& end );

    /**
     * This method gets called whenever all lines got rasterized.
     */
    virtual void finished();

protected:
    /**
     * The grid, which needs to be used for the created dataset and to which the parameterizeVoxel method is relating to.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

private:
};

#endif  // WRASTERPARAMETERIZATION_H

