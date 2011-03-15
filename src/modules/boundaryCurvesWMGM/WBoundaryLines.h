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

#ifndef WBOUNDARYLINES_H
#define WBOUNDARYLINES_H

#include <utility>

#include <boost/array.hpp>

#include "../../common/WBoundingBox.h"
#include "WBoundaryBuilder.h"

// forward declarations
class WGridRegular3D;
class WDataSetScalar;

/**
 * Computes boundary lines with an iso lines approach instead of the fragment shader.
 */
class WBoundaryLines : public WBoundaryBuilder
{
public:
    /**
     * Constructs an builder instance.
     *
     * \param texture The underlying scalar dataset to compute those boundaries for
     * \param properties Properties, like slice positions, thresholds etc.
     * \param slices Slice geodes which are controlled (hide/unhide) by the module.
     */
    WBoundaryLines( boost::shared_ptr< const WDataSetScalar > texture,
                    boost::shared_ptr< const WProperties > properties,
                    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices );

    /**
     * Starts rendering and finally insert result in output.
     *
     * \param output Where to put/insert the results.
     */
    virtual void run( osg::ref_ptr< WGEManagedGroupNode > output );

protected:
private:
    /**
     * Creates for each slice number the corresponding geodes for a bounding box as well
     * for the boundary curves.
     *
     * \param sliceNum For which slice the geode should be generated.
     *
     * \return A WGEGroupNode containing the bb geode as well as the boundary curve geode.
     */
    osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const;

    /**
     * Computes the origin as well as the base vectors for the given slice number.
     *
     * \param sliceNum For which slice the base should be computed for.
     * \param origin Return parameter of the slice origin
     * \param a Return parameter of the first base vector
     * \param b Return parameter of the second base vector
     *
     * \return The other two slice numbers.
     */
    std::pair< unsigned char, unsigned char > computeSliceBase( const unsigned char sliceNum,
        boost::shared_ptr< WVector3D > origin, boost::shared_ptr< WVector3D > a, boost::shared_ptr< WVector3D > b ) const;

    /**
     * Generates the bounding boxes for all three slices.
     */
    void computeSliceBB();

    /**
     * Boundingboxes for each slice.
     * 0 : xSlice, 1 : ySlice, 2 : zSlice
     */
    boost::array< WBoundingBox, 3 > m_sliceBB;

    /**
     * Shortcut for the WRegularGrid3D given via the m_texture dataset.
     */
    boost::shared_ptr< WGridRegular3D > m_grid;
};

#endif  // WBOUNDARYLINES_H
