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

#ifndef WRASTERALGORITHM_H
#define WRASTERALGORITHM_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "core/dataHandler/WDataSetScalar.h"
#include "core/dataHandler/WGridRegular3D.h"
#include "core/common/math/WLine.h"
#include "WRasterParameterization.h"

/**
 * Base class for all rasterization algorithms. The interface will be as
 * follows: You need a WGridRegular3D grid and some geometry. The grid
 * specifies the volume where you raster the geometry. For other geometries
 * than lines please overload the raster() member function.
 */
class WRasterAlgorithm
{
public:
    /**
     * Creates new raster algorithm within the given grid. The grid may later
     * also be used to generate a real DataSet, \see generateDataSet() for
     * further instructions.
     *
     * \param grid The grid specifying the voxels.
     */
    explicit WRasterAlgorithm( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Dispose a this raster algorithm.
     */
    virtual ~WRasterAlgorithm();

    /**
     * Rasterize the given line into the grid of dataset.
     * The value of the voxel which will be hit changes its value.
     *
     * \param line Polyline which is about to be rastered.
     */
    virtual void raster( const WLine& line ) = 0;

    /**
     * Computes a dataset out of our voxel values and the previously given
     * grid. Note this may take some time.
     *
     * \return Dataset where all voxels which are hit by the rastered lines are
     * non zero.
     */
    boost::shared_ptr< WDataSetScalar > generateDataSet() const;

    /**
     * This method allows the user of the raster algorithm to add arbitrary parameterizations. Each parameterization creates a new volume dataset
     * parameterizing the voxelized line somehow.
     *
     * \param algorithm the algorithm
     */
    void addParameterizationAlgorithm( boost::shared_ptr< WRasterParameterization > algorithm );

    /**
     * Called whenever all lines have been rasterized.
     */
    virtual void finished();

protected:
    /**
     * All the parameterization algorithms to apply while rasterizing a line.
     */
    std::vector< boost::shared_ptr< WRasterParameterization > > m_parameterizations;

    /**
     * The mutex used to lock access to m_parameterizations.
     */
    boost::shared_mutex m_parameterizationsLock;

    /**
     * The grid is used for the following purposes:
     *  - First we need it when creating the final dataset
     *  - Second we need it to determine how many voxels are there at
     *    construction time
     *  - Third we need it when computing the value number out of a position
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * Stores the value of each voxel. If and only if a voxel is _not_ hit by
     * a line segment its value is 0.0.
     */
    std::vector< double > m_values;

    /**
     * This method allows all registered parameterization algorithms to update. This basically simply calls all parameterizeVoxel methods in
     * m_parameterizations vector.
     *
     * \param voxel the voxel to parameterize
     * \param voxelIdx the voxel index in the common grid calculated using "voxel" (this is for convenience)
     * \param axis  Along which axis the traversal takes place. Since when walking in e.g. X-direction there are not supporting voxels in the
     * \param value the new voxel value
     * \param start Start point of the line segment (used to computed the distance)
     * \param end   End point of the line segment (used to computed the distance)
     */
    virtual void parameterizeVoxel( const WValue< int >& voxel, size_t voxelIdx, const int axis, const double value,
                                    const WPosition& start,
                                    const WPosition& end );

    /**
     * Distribute a new line getting rasterized to all parameterize algorithms.
     *
     * \param line the new line.
     */
    virtual void newLine( const WLine& line );

    /**
     * Distribute a new segment of a line to all parameterization algorithms.
     * Gets called for each new line segment getting rasterized, as one segment can have multiple voxels.
     *
     * \param start start point of the new line segment
     * \param end end point of the new line segment
     */
    virtual void newSegment( const WPosition& start, const WPosition& end );

private:
};
#endif  // WRASTERALGORITHM_H
