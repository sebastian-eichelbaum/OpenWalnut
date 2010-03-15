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

#ifndef WDIRECTIONPARAMETERIZATION_H
#define WDIRECTIONPARAMETERIZATION_H

#include "WRasterParameterization.h"

/**
 * Stores the direction if a line in a separate dataset for each voxel.
 */
class WDirectionParameterization: public WRasterParameterization
{
public:

    /**
     * Default constructor.
     *
     * \param grid the grid used for the new dataset.
     */
    explicit WDirectionParameterization( boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Destructor.
     */
    virtual ~WDirectionParameterization();

    /**
     * This method allows this parameterization to update. It gets called for every voxel which is rasterized by the WRasterAlgorithm.
     *
     * \param voxel the voxel to parameterize
     * \param voxelIdx the voxel index in the common grid calculated using "voxel" (this is for convenience)
     * \param axis  Along which axis the traversal takes place. Since when walking in e.g. X-direction there are not supporting voxels in the
     * \param value the new voxel value
     * \param start Start point of the line segement (used to computed the distance)
     * \param end   End point of the line segement (used to computed the distance)
     */
    virtual void parameterizeVoxel( const wmath::WValue< int >& voxel, size_t voxelIdx, const int axis, const double value,
                                    const wmath::WPosition& start,
                                    const wmath::WPosition& end );

    /**
     * Gets the dataset representing the parameterization.
     *
     * \return the dataset.
     */
    virtual boost::shared_ptr< WDataSetSingle > getDataSet();

protected:

    /**
     * Stores the direction of the fiber at each voxel.
     */
    std::vector< double > m_dirValues;

private:
};

#endif  // WDIRECTIONPARAMETERIZATION_H

