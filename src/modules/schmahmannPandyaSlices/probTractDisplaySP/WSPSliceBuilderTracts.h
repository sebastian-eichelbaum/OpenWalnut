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

#ifndef WSPSLICEBUILDERTRACTS_H
#define WSPSLICEBUILDERTRACTS_H

#include <list>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "core/common/WBoundingBox.h"
#include "core/common/WProperties.h"
#include "core/common/WPropertyTypes.h"
#include "WSPSliceBuilder.h"

class WDataSetFibers;

/**
 * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya. After creating a builder instance
 * you must calculate the intersections (with preprocess) again and again whenever a slice has moved.
 */
class WSPSliceBuilderTracts : public WSPSliceBuilder
{
public:
    /**
     * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya. After this initialization,
     * the user needs still to call the \ref preprocess() member function in order to get right results.
     *
     * \param probTracts The list of probabilistic tractograms which should be taken into account.
     * \param sliceGroup Slice positions
     * \param colorMap For each connected probabilistic trac its color
     * \param detTracts The deterministic fibers which are used to show the probabilistic tracts.
     * \param tractGroup The properties for visualization for the deterministic tract method
     */
    WSPSliceBuilderTracts( ProbTractList probTracts, WPropGroup sliceGroup, std::vector< WPropGroup > colorMap,
            boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup tractGroup );

    /**
     * Implements the preprocessing interface \ref WSPSliceBuilder.
     * For each slice compute the deterministic trac indices which are intersecting with it.
     */
    void preprocess();

    /**
     * Implements the generateSlice interface \ref WSPSliceBuilder.
     * Creates some geodes for the given slice which depicts a limited envirnonment of its intersecting fibers and a geode
     * for those projected onto that slice.
     *
     * \param sliceNum 0 denotes xSlice, 1 ySlice and finally 2 means zSlice.
     *
     * \return A group node with the cutted intersecting fibers and projected onto the slice.
     */
    osg::ref_ptr< WGEGroupNode > generateSlice( const unsigned char sliceNum ) const;

protected:
private:
    /**
     * Projects a given line strip onto the given slice simply by setting its components to the slice position.
     *
     * \param sliceNum Which slice 0 == xSlice, 1 == ySlice and 2 == zSlice
     * \param vertices Verices of the tract which needs to be transformed/projected.
     * \param slicePos The position of the slice.
     *
     * \note After calling this funtion the given vertices are lost!
     */
    void projectTractOnSlice( const unsigned char sliceNum, osg::ref_ptr< osg::Vec3Array > vertices, const int slicePos ) const;

    /**
     * Color each vertex accordingly to all given probabilistic tractograms. For each vertex all probTracts are considered in the following manner:
     *  - first check if the tract contribute, and if so, set alpha value to probability
     *  - the resulting color is then computed by summing all contributing colors but divide them by the number of contributing colors, so each
     *    prob tract contributes the same amount but with different alpha value.
     *
     * \param vertices The vertices to compute the colors for
     *
     * \return An array of colors for the given vertices.
     */
    osg::ref_ptr< osg::Vec4Array > colorVertices( osg::ref_ptr< const osg::Vec3Array > vertices ) const;


    /**
     * Reference to the deterministic tracts.
     */
    boost::shared_ptr< const WDataSetFibers > m_detTracts;

    /**
     * Lists of deterministic tract indices which are intersecting each slice.
     */
    std::vector< std::list< size_t > > m_intersectionList;

    /**
     * For each deterministic tract its precomputed axis aligned bounding box.
     */
    std::vector< WBoundingBox > m_tractBB;

    /**
     * The distance at which the lines are cut off, and only their parts inside the so defined environment are projected onto the slices.
     */
    boost::shared_ptr< const WPVDouble > m_maxDistance;

    /**
     * Up to this propbability the probabilities of the probabilisitic tractogram do NOT contribute.
     */
    boost::shared_ptr< const WPVDouble > m_probThreshold;

    /**
     * Forwarded flag which denotes if the node corresponding to the intersections stipples should be visible or not.
     */
    WPropBool m_showIntersections;

    /**
     * Forwarded flag which denotes if the node corresponding to the projections stipples should be visible or not.
     */
    WPropBool m_showProjections;
};

#endif  // WSPSLICEBUILDERTRACTS_H
