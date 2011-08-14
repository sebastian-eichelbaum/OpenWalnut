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

#include <list>
#include <utility>
#include <vector>

#include <boost/array.hpp>
#include <boost/unordered_map.hpp>

#include <osg/Geometry>

#include "core/common/WBoundingBox.h"
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
     * \param localPath The path where the shaders reside
     */
    WBoundaryLines( boost::shared_ptr< const WDataSetScalar > texture,
                    boost::shared_ptr< const WProperties > properties,
                    boost::array< osg::ref_ptr< WGEManagedGroupNode >, 3 > *slices,
                    boost::filesystem::path localPath );

    /**
     * Starts rendering and finally insert result in output.
     *
     * \param output Where to put/insert the results.
     * \param sliceNum If given -1 all slice will perform an update, otherwise only the slice with the given slice number.
     */
    virtual void run( osg::ref_ptr< WGEManagedGroupNode > output, const char sliceNum = -1 );

protected:
private:
    /**
     * Map for edgeID -> ( neighbourEdgeID1, neighbourEdgeID2 ).
     */
    typedef boost::unordered_map< size_t, std::pair< int , int > > EdgeNeighbourMap;

    /**
     * Two pointIDs describing an edge.
     */
    typedef std::pair< std::size_t, std::size_t > Edge;

    //todo(math): Edge is wrong here, since that are not point but EdgeIDs
    /**
     * Vector of segments where two edgeIDs describing an segment.
     */
    typedef std::vector< Edge > SegmentVector;

    /**
     * List of segments where two edgeIDs describing an segment.
     */
    typedef std::list< Edge > SegmentList;

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
     * In order to compute iso lines of the gray and white matter for a specific slice, we need a grid to operate on. This
     * grid represents a WGridRegular2DIn3D but the last dimension/numCoordsZ() is set to 1.
     *
     * \param sliceNum For which slice the grid should be generated.
     * \param resolution The size of the cells in that grid.
     *
     * \return The 2D grid for the given slice.
     */
    boost::shared_ptr< WGridRegular3D > generateSliceGrid( const unsigned char sliceNum, const double resolution ) const;

    /**
     * Extracts the sequence of edges representing a line strip, where the first element in the hash map is a member of.
     *
     * \param l The hash map, containing edge numbers as keys and their left and right neighbour as the value pair.
     *
     * \return List of edge number of the line strip.
     */
    std::list< size_t > extractLineStripEdges( EdgeNeighbourMap *l ) const;

    /**
     * Constructs a hash map where each grid edgeID ( the key ), has a pair of neighbour edgeIDs ( value ).
     *
     * \param segments List of segments given as pair of edgeIDs.
     *
     * \return The edge neighbour map.
     */
    boost::shared_ptr< EdgeNeighbourMap > generateEdgeNeighbourMap( const SegmentList& segments ) const;

    /**
     * Construct a iso lines geometry for the given isovalue.
     *
     * \param isoValue The iso value
     * \param map Containing edgeIDs as keys and
     * \param edgeIDToPointIDs
     * \param interpolates
     * \param sliceGrid
     *
     * \return The osg geometry.
     */
    osg::ref_ptr< osg::Geometry > traverseEdgeHashMap( double isoValue, boost::shared_ptr< WBoundaryLines::EdgeNeighbourMap > map,
            const std::vector< Edge >& edgeIDToPointIDs, const std::vector< double > &interpolates,
            boost::shared_ptr< const WGridRegular3D > sliceGrid ) const;

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

    /**
     * Defines the size of quads to be processed for the iso curve computation.
     */
    WPropDouble m_resolution;
};

#endif  // WBOUNDARYLINES_H
