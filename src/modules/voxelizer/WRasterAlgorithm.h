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

#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WGridRegular3D.h"
#include "../../common/math/WLine.h"
#include "WRasterAlgorithm.h"

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
     * furter instructions.
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
    virtual void raster( const wmath::WLine& line ) = 0;

    /**
     * Computes a dataset out of our voxel values and the previously given
     * grid. Note this may take some time.
     *
     * \return Dataset where all voxels which are hit by the rastered lines are
     * non zero.
     */
    boost::shared_ptr< WDataSetSingle > generateDataSet() const;

    /**
     * Computes a dataset out of our voxel values and the previously given grid. It contains a vector at each position representing the direction
     * of the fiber at this point.
     *
     * \return Dataset where all voxels which are hit by the rastered lines contain non-zero vectors.
     */
    boost::shared_ptr< WDataSetSingle > generateVectorDataSet() const;

protected:

    /**
     * The grid is used for the following purposes:
     *  - First we need it when creating the final dataset
     *  - Second we need it to determine how many voxels are there at
     *    construction time
     *  - Thrid we need it when computing the value number out of a position
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * Stores the value of each voxel. If and only if a voxel is _not_ hit by
     * a line segment its value is 0.0.
     */
    std::vector< double > m_values;

    /**
     * Stores the direction of the fiber at each voxel.
     */
    std::vector< double > m_dirValues;

private:
};
#endif  // WRASTERALGORITHM_H
