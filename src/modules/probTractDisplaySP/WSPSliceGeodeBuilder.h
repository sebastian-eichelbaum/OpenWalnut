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
#include <vector>

#include <boost/shared_ptr.hpp>

#include <osg/Geode>

#include "../../common/WBoundingBox.h"
#include "../../common/WProperties.h"
#include "../../common/WPropertyTypes.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../dataHandler/WDataSetScalar.h"

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

public:
    /**
     * Creates a geode builder to build axial, coronal and sagittal slices ala Schahmann y Pandya. After this initialization,
     * the user needs still to call the \ref determineIntersectingDeterministicTracts() member function in order to get
     * right results.
     *
     * \param probTracts The list of probabilistic tractograms which should be taken into account.
     * \param detTracts The deterministic fibers which are used to show the probabilistic tracts.
     * \param sliceGroup Slice positions, and slice flags (if to show or not)
     */
    WSPSliceGeodeBuilder( ProbTractList probTracts, boost::shared_ptr< const WDataSetFibers > detTracts, WPropGroup sliceGroup );

    /**
     * Generates those special slice geodes for the sagittal slice. \ref generateSlice
     *
     * \param maxDistance specifies when the fibers should be cut off! Its a kind of environment around the slice.
     *
     * \return Geode representing a sagittal view ala Schahmann y Pandya.
     */
    osg::ref_ptr< osg::Geode > generateXSlice( double maxDistance = 1.0 ) const;

    /**
     * Generates those special slice geodes for the coronal slice. \ref generateSlice
     *
     * \param maxDistance specifies when the fibers should be cut off! Its a kind of environment around the slice.
     *
     * \return Geode representing a coronal view ala Schahmann y Pandya.
     */
    osg::ref_ptr< osg::Geode > generateYSlice( double maxDistance = 1.0 ) const;

    /**
     * Generates those special slice geodes for the axial slice. \ref generateSlice
     *
     * \param maxDistance specifies when the fibers should be cut off! Its a kind of environment around the slice.
     *
     * \return Geode representing an axial view ala Schahmann y Pandya.
     */
    osg::ref_ptr< osg::Geode > generateZSlice( double maxDistance = 1.0 ) const;

    /**
     * For each slice compute the deterministic trac indices which are intersecting with it.
     */
    void determineIntersectingDeterministicTracts();

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
     * \param component Which slice 0 == xSlice, 1 == ySlice and 2 == zSlice
     * \param vertices Verices of the tract which needs to be transformed/projected.
     * \param slicePos The position of the slice.
     *
     * \note After calling this funtion the given vertices are lost!
     */
    void projectTractOnSlice( unsigned char component, osg::ref_ptr< osg::Vec3Array > vertices, int slicePos ) const;

    /**
     * Computes the bouding boxes for the slices.
     *
     * \note Whenever the xPos, yPos or zPos of the slice change those have to be recomputed!
     */
    void computeSliceBB();

    /**
     * Creates a geode for the given slice which depicts a limited envirnonment of its intersecting fibers but projected onto that slice.
     *
     * \param intersections Indices for that slice where its deterministic tract do intersect with the BB of the slice.
     * \param slicePos Current position of the slice ( either x value, y value or z value )
     * \param component 0 denotes xSlice, 1 ySlice and finally 2 means zSlice.
     * \param maxDistance when the fibers should be cut off, best value is probably the 2*sampling distance of the fibers.
     *
     * \return A geode where the cutted intersecting fibers are projected onto the slice.
     */
    osg::ref_ptr< osg::Geode > generateSlice( std::list< size_t > intersections, int slicePos, unsigned char component,
            double maxDistance = 1.0 ) const;

    /**
     * List of deterministic tract indices which are intersecting the xSlice.
     */
    std::list< size_t > m_xIntersections;

    /**
     * List of deterministic tract indices which are intersecting the ySlice.
     */
    std::list< size_t > m_yIntersections;

    /**
     * List of deterministic tract indices which are intersecting the zSlice.
     */
    std::list< size_t > m_zIntersections;

    /**
     * For each deterministic tract its precomputed axis aligned bounding box.
     */
    std::vector< WBoundingBox > m_tractBB;

    /**
     * Axis aligned bounding box for the current xSlice.
     */
    WBoundingBox m_xSliceBB;

    /**
     * Axis aligned bounding box for the current ySlice.
     */
    WBoundingBox m_ySliceBB;

    /**
     * Axis aligned bounding box for the current zSlice.
     */
    WBoundingBox m_zSliceBB;

    /**
     * Reference to the deterministic tracts.
     */
    boost::shared_ptr< const WDataSetFibers > m_detTracts;

    /**
     * List of probabilisitc tractograms.
     */
    ProbTractList m_probTracts;

    /**
     * The grid of the first tractogram. It is assumed that all given probablilisitc tractograms operate on the same grid.
     */
    boost::shared_ptr< const WGridRegular3D > m_grid;

    boost::shared_ptr< const WPVInt >  m_xPos; //!< x position of the slice
    boost::shared_ptr< const WPVInt >  m_yPos; //!< y position of the slice
    boost::shared_ptr< const WPVInt >  m_zPos; //!< z position of the slice
    boost::shared_ptr< const WPVBool > m_showonX; //!< indicates whether the vector should be shown on slice X
    boost::shared_ptr< const WPVBool > m_showonY; //!< indicates whether the vector should be shown on slice Y
    boost::shared_ptr< const WPVBool > m_showonZ; //!< indicates whether the vector should be shown on slice Z
};

#endif  // WSPSLICEGEODEBUILDER_H
