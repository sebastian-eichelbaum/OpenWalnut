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

#ifndef WMORITHREAD_H
#define WMORITHREAD_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "../../common/WThreadedRunner.h"
#include "../../common/WProgress.h"
#include "../../dataHandler/WDataSetSingle.h"
#include "../../dataHandler/WDataSetFiberVector.h"
#include "../../dataHandler/WDataSetFibers.h"
#include "../../common/math/WVector3D.h"

/**
 * A class that extracts fibers from a subset of an eigenvectorfield in its own thread.
 */
class WMoriThread : public WThreadedRunner
{
public:

    /**
     * A class that encapsulates the data needed to construct a WDataSetFibers.
     */
    class WFiberAccumulator
    {
        public:

        /**
         * Constructor.
         */
        WFiberAccumulator();

        /**
         * Destructor.
         */
        ~WFiberAccumulator();

        /**
         * Add a fiber to the dataset.
         *
         * \param in The fiber to add, stored as a vector of Positions.
         * 
         * This function is threadsafe.
         */
        void add( std::vector< wmath::WVector3D > const& in );

        /**
         * Return the dataset that has been accumulated to this point
         * and start a new dataset.
         * 
         * \return A shared_ptr pointing to the WDataSetFibers that has been accumulated.
         *
         * The returned shared_ptr is the sole owner of the WDataSetFibers.
         */
        boost::shared_ptr< WDataSetFibers > buildDataSet();

    protected:
    private:

        /**
         * A mutex needed to guarantee thread-safety.
         */
        boost::mutex m_fiberMutex;

        /**
         * One of the vectors needed to construct a WDataSetFibers.
         * Stores the points in a vector of floats.
         */
        boost::shared_ptr< std::vector< float > > m_points;

        /**
         * One of the vectors needed to construct a WDataSetFibers.
         * Stores the starting indices (refering to the points vector) of the fibers.
         */
        boost::shared_ptr< std::vector< size_t > > m_fiberIndices;

        /**
         * One of the vectors needed to construct a WDataSetFibers.
         * Stores the length of the fibers.
         */
        boost::shared_ptr< std::vector< size_t > > m_fiberLengths;

        /**
         * One of the vectors needed to construct a WDataSetFibers.
         * Stores information about what fiber a point in the points vector refers to.
         */
        boost::shared_ptr< std::vector< size_t > > m_pointToFiber;
    };

    /**
     * Constructor.
     * \param grid The regular 3D grid of the input dataset.
     * \param eigenVectors A vector storing the eigenvector that belongs to the eigenvalue of largest magnitude for every grid position.
     * \param fa A vector that stores the fractional anisotropy for every grid position.
     * \param minFA The minimum fractional anisotropy threshold needed by the tracking algorithm. 
     * \param minPoints The minimum length of a fiber.
     * \param minCos The minimum cosine of the angle of bends between adjacent fiber segments.
     * \param progr A progress object.
     * \param index Determines what part of the grid this thread should place seed points in.
     * \param fiberAccu A WFiberAccumulator.
     */
    WMoriThread( boost::shared_ptr< WGridRegular3D > grid,
                 boost::shared_ptr< std::vector< wmath::WVector3D > > eigenVectors,
                 boost::shared_ptr< std::vector< double > > fa,
                 double const minFA,
                 unsigned int const minPoints,
                 double const minCos,
                 boost::shared_ptr< WProgress > progr,
                 uint32_t const index,
                 boost::shared_ptr< WFiberAccumulator > fiberAccu );

    /**
     * Destructor.
     */
    virtual ~WMoriThread();

protected:
private:

    /**
     * The main function of this thread.
     */
    virtual void threadMain();

    /**
     * Track a fiber starting at a seed point in a voxel given by coords.
     *
     * \param coords The coordinates of the starting voxel.
     * \param points Stores the points of the tracked fiber. 
     */
    void trackFiber( wmath::WValue< size_t > const& coords, std::vector< wmath::WVector3D >& points ) const;

    /**
     * Test the fractional anisotropy of the voxel at coords against the threshold.
     *
     * \param coords The coordinates of the voxel.
     * \return true, iff the fractional anisotropy at the voxel is larger then the threshold. 
     */
    bool testFA( wmath::WValue< size_t >const & coords ) const;

    /**
     * Return the eigenvector at the voxel at coords.
     *
     * \param coords The coordinates of the voxel.
     * \return The eigenvector at the voxel.
     */
    wmath::WVector3D getEigenAt( wmath::WValue< size_t > const& coords ) const;

    /**
     * Recursively track the fiber through the next voxel.
     *
     * \param pos The starting position on the boundary of the voxel.
     * \param eigen The direction in which to track.
     * \param dir Determines whether to add the generated points to the end or at the beginning of the points vector.
     * \param coords The coordinates of the current voxel.
     * \param points Stores the points of the currently tracked fiber.
     * \param iter The maximum recursion depth. 
     */
    void trackFiberRec( wmath::WVector3D const& pos,
                        wmath::WVector3D const& eigen,
                        int dir,
                        wmath::WValue< size_t > const& coords,
                        std::vector< wmath::WVector3D >& points,
                        size_t const iter ) const;

    /**
     * Clamp a vector to the voxel at position coords.
     * 
     * \param v The vector to clamp.
     * \param coords The coordinates of the voxel.
     *
     * The clamped vector will be either inside or on the boundary of the voxel.
     */
    void clamp( wmath::WVector3D& v, wmath::WValue< size_t > const& coords ) const;

    /**
     * Find a new position on the boundary of the current voxel by following the direction
     * of the eigenvector starting from position pos.
     *
     * \param pos The starting position.
     * \param coords The coordinates of the current voxel.
     * \param eigen The eigenvector.
     */
    wmath::WVector3D getNextPosition( wmath::WVector3D const& pos, wmath::WValue< size_t > const& coords, wmath::WVector3D const& eigen ) const;

    /**
     * Find a new voxel that contains a given position and is adjacent to the current voxel.
     *
     * \param pos The position.
     * \param oldCoords The coordinates of the current voxel.
     * \param newCoords The coordinates of the new voxel.
     * \return true, iff the new voxel is inside the grid.
     */
    bool getNextVoxel( wmath::WVector3D const& pos, wmath::WValue< size_t > const& oldCoords,
           wmath::WValue< size_t >& newCoords ) const;

    /**
     * Calculate the one-dimensional distance to the border of the current voxel.
     *
     * \param pos The current position.
     * \param coord The coordinate of the current voxel.
     * \param eigen The direction.
     * 
     * \return The distance to the boundary of the voxel (in one dimension/axis/component).
     */
    double distanceToBorder( double const& pos, size_t const& coord, double const& eigen ) const;

    /**
     * A shared pointer to the grid. 
     */
    boost::shared_ptr< WGridRegular3D > m_grid;

    /**
     * A shared pointer to the vector of eigenvectors.
     */
    boost::shared_ptr< std::vector< wmath::WVector3D > > m_eigenVectors;

    /**
     * A shared poiter to the vector of fractional anisotropy values.
     */
    boost::shared_ptr< std::vector< double > > m_FA;

    /**
     * The fractional anisotropy threshold.
     */
    double const m_minFA;

    /**
     * Minimum of points per fiber.
     */
    unsigned int const m_minPoints;

    /**
     * The minimum angle between two adjacent segments of a fiber.
     */
    double const m_minCos;

    /**
     * A shared pointer to a WProgress object.
     */
    boost::shared_ptr< WProgress > m_progress;

    /**
     * Determines what part of the grid the seedpoints should be placed in.
     */
    uint32_t const m_index;

    /**
     * A WFiberAccumulator.
     */
    boost::shared_ptr< WFiberAccumulator > m_fiberAccu;

    /**
     * A constant needed for comparations.
     */
    double const m_eps;

    /**
     * The extent of the grid in each axis' direction.
     */
    wmath::WValue< size_t > m_gridSize;

    /**
     * The grid's transformation matrix.
     */
    wmath::WMatrix< double > m_gridTransform;

    /**
     * The inverse of the grid's tranformation matrix.
     */
    wmath::WMatrix< double > m_invGridTransform;

    /**
     * The translation vector of the grid.
     */
    wmath::WVector3D m_gridTranslation;
};

#endif  // WMORITHREAD_H
