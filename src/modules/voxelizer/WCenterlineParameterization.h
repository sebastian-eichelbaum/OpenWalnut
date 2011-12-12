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

#ifndef WCENTERLINEPARAMETERIZATION_H
#define WCENTERLINEPARAMETERIZATION_H

#include <vector>

#include "core/common/datastructures/WFiber.h"

#include "WRasterParameterization.h"

/**
 * Stores the direction if a line in a separate dataset for each voxel.
 */
class WCenterlineParameterization: public WRasterParameterization
{
public:
    /**
     * Default constructor.
     *
     * \param grid the grid used for the new dataset.
     * \param centerline the centerline of the cluster
     */
    WCenterlineParameterization( boost::shared_ptr< WGridRegular3D > grid, boost::shared_ptr< WFiber > centerline );

    /**
     * Destructor.
     */
    virtual ~WCenterlineParameterization();

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
    virtual void parameterizeVoxel( const WValue< int >& voxel, size_t voxelIdx, const int axis, const double value,
                                    const WPosition& start,
                                    const WPosition& end );

    /**
     * Gets the dataset representing the parameterization.
     *
     * \return the dataset.
     */
    virtual boost::shared_ptr< WDataSetScalar > getDataSet();

    /**
     * Gets called for each new line getting rasterized. Here, it is used to reset the internal length integrator
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
     * Gets called whenever all lines have been rasterized.
     */
    virtual void finished();

protected:
    /**
     * Stores the current length of the centerline fiber at each voxel.
     */
    std::vector< double > m_paramValues;

    /**
     * The values with applied selective dilatation.
     */
    std::vector< double > m_paramFinalValues;

    /**
     * Stores whether the voxel has been set in the past or not.
     */
    std::vector< bool > m_paramSetValues;

    /**
     * The centerline of the cluster
     */
    boost::shared_ptr< WFiber > m_centerline;

    /**
     * The current start parameter for the current segment.
     */
    double m_currentStartParameter;

    /**
     * The current end parameter for the current segment.
     */
    double m_currentEndParameter;

private:
};

#endif  // WCENTERLINEPARAMETERIZATION_H
