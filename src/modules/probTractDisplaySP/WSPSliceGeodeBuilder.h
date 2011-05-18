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

#ifndef WSPSLICEGEODEBUILDER_H
#define WSPSLICEGEODEBUILDER_H

#include <list>
#include <utility>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "core/common/WBoundingBox.h"
#include "core/common/WProperties.h"
#include "core/common/WPropertyTypes.h"
#include "core/dataHandler/WDataSetFibers.h"
#include "core/dataHandler/WDataSetScalar.h"

/**
 * This Builder is used to generate views of probabilistic tractograms ala Schmahmann and Pandya. After creating a builder instance
 * you must calculate the intersections again and again whenever a slice has moved. A typical usage of this builder is:
   <dfn>
     // first setup
     builder = boost::shared_ptr< WSPSliceGeodeBuilder >( new WSPSliceGeodeBuilder( probTracts, detTracts, m_sliceGroup ) );

     // whenever something changes, discard old slices, compute intersections again and then generate new slices
     groupNode->clear();
     builder->determineIntersectingDeterministicTracts();
     groupNode->insert( builder->generateXSlice( m_delta->get() ) );
     groupNode->insert( builder->generateYSlice( m_delta->get() ) );
     groupNode->insert( builder->generateZSlice( m_delta->get() ) );
   </dfn>
 */
class WSPSliceGeodeBuilder
{
private:
    /**
     * List of scalar datasets e.g. probabilistic tractograms.
     */
    typedef std::list< boost::shared_ptr< const WDataSetScalar > > ProbTractList;

    /**
     * Just a shorthand for a pair of osg::Geodes.
     */
    typedef std::pair< osg::ref_ptr< osg::Geode >, osg::ref_ptr< osg::Geode > > GeodePair;

public:

    /**
     * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya. After this initialization,
     * the user needs still to call the \ref determineIntersectingDeterministicTracts() member function in order to get
     * right results.
     *
     * \param probTracts The list of probabilistic tractograms which should be taken into account.
     * \param detTracts The deterministic fibers which are used to show the probabilistic tracts.
     * \param sliceGroup Slice positions
     * \param colorMap For each connected probabilistic trac its color
     */
    WSPSliceGeodeBuilder( ProbTractList probTracts, boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup sliceGroup,
            std::vector< WPropGroup > colorMap );

    /**
     * For each slice compute the deterministic trac indices which are intersecting with it.
     */
    void determineIntersectingDeterministicTracts();

    /**
     * Creates a pair of geodes for the given slice which depicts a limited envirnonment of its intersecting fibers and a geode
     * for those projected onto that slice.
     *
     * \param sliceNum 0 denotes xSlice, 1 ySlice and finally 2 means zSlice.
     * \param maxDistance when the fibers should be cut off, best value is probably the 2*sampling distance of the fibers.
     * \param probThreshold probabilities below this threshold do not contribute to the coloring
     *
     * \return A pair geodes with the cutted intersecting fibers and projected onto the slice.
     */
    GeodePair generateSlices( const unsigned char sliceNum, const double maxDistance = 1.0, const double probThreshold = 0.1 ) const;

protected:

private:
    /**
     * Ensures that every grid of each probabilistic tractogram is of type WGridRegular3D and that they are all the same.
     *
     * \todo Check that each grid is the same as soon as operator== for WGridRegular3D is available
     * \throw WTypeMismatched when a grid was found which is not castable to WGridRegular3D
     */
    void checkAndExtractGrids();

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
     * Computes the bouding boxes for the slices.
     *
     * \note Whenever the xPos, yPos or zPos of the slice change those have to be recomputed!
     */
    void computeSliceBB();

    /**
     * Very simple color mapping which is used temporarily, later we want to substitute this with a color chooser as properties.
     *
     * \param probTractNum Index of the probabilisitc tractrogram to get the color for.
     *
     * \return The color for the given prob tract index.
     *
     * \todo Replace this with properties approach
     */
    WColor colorMap( size_t probTractNum ) const;

    /**
     * Color each vertex accordingly to all given probabilistic tractograms. For each vertex all probTracts are considered in the following manner:
     *  - first check if the tract contribute, and if so, set alpha value to probability
     *  - the resulting color is then computed by summing all contributing colors but divide them by the number of contributing colors, so each
     *    prob tract contributes the same amount but with different alpha value.
     *
     * \param vertices The vertices to compute the colors for
     * \param probThreshold probabilities below this threshold do not contribute to the coloring
     *
     * \return An array of colors for the given vertices.
     */
    osg::ref_ptr< osg::Vec4Array > colorVertices( osg::ref_ptr< const osg::Vec3Array > vertices, const double probThreshold ) const;

    /**
     * Reference to the deterministic tracts.
     */
    boost::shared_ptr< const WDataSetFibers > m_detTracts;

    /**
     * List of probabilisitc tractograms.
     */
    ProbTractList m_probTracts;

    /**
     * Lists of deterministic tract indices which are intersecting each slice.
     */
    std::vector< std::list< size_t > > m_intersectionList;

    /**
     * For each deterministic tract its precomputed axis aligned bounding box.
     */
    std::vector< WBoundingBox > m_tractBB;

    /**
     * Axis aligned bounding box for each slice.
     */
    std::vector< WBoundingBox > m_sliceBB;

    /**
     * The grid of the first tractogram. It is assumed that all given probablilisitc tractograms operate on the same grid.
     */
    boost::shared_ptr< const WGridRegular3D > m_grid;

    std::vector< boost::shared_ptr< const WPVInt > >  m_slicePos; //!< Hold the current position of each slice given from the properties

    /**
     * Reference to the color properites.
     */
    std::vector< WPropGroup > m_colorMap;
};

#endif  // WSPSLICEGEODEBUILDER_H
