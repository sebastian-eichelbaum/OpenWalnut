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

#ifndef WTHREADEDTRACKINGFUNCTION_H
#define WTHREADEDTRACKINGFUNCTION_H

#include <vector>
#include <utility>

#include <boost/array.hpp>

#include "../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../common/WSharedObject.h"
#include "../common/WThreadedJobs.h"

#include "WDataSetSingle.h"

class WThreadedTrackingFunctionTest; //! forward declaration


/**
 * For tracking related functionality.
 */
namespace wtracking // note that this is not final
{
    // an epsilon value for various floating point comparisons
#define TRACKING_EPS 0.0000001

    /**
     * \class WTrackingUtility
     *
     * A class that provides untility functions and typedefs for tracking algorithms.
     */
    class WTrackingUtility
    {
    public:
        //! define a job type for tracking algorithms
        typedef std::pair< WVector3d, WVector3d > JobType;

        //! the dataset type
        typedef WDataSetSingle DataSetType;

        //! a pointer to a dataset
        typedef boost::shared_ptr< DataSetType const > DataSetPtr;

        //! a function that calculates a direction to continue tracking
        typedef boost::function< WVector3d ( DataSetPtr, JobType const& ) > DirFunc;

        //! a pointer to a regular 3d grid
        // other grid types are not supported at the moment
        typedef boost::shared_ptr< WGridRegular3D > Grid3DPtr;

        /**
         * A function that follows a direction until leaving the current voxel.
         *
         * \param dataset A pointer to the input dataset.
         * \param job A pair of vectors, the position and the direction of the last integration.
         * \param dirFunc A function that computes the next direction.
         *
         * \return true, iff the calculated point is a valid position inside the grid
         */
        static bool followToNextVoxel( DataSetPtr dataset, JobType& job, DirFunc const& dirFunc );

        // one could add a runge-kutta-integrator too

        /**
         * Check if a point is on the boundary of the given grid, where boundary
         * means a distance less then TRACKING_EPS from any plane between
         * voxels. This does not check if the position is actually inside the grid.
         *
         * \param grid The grid.
         * \param pos The position to test.
         *
         * \return true, iff the position is on any voxel boundary
         */
        static bool onBoundary( Grid3DPtr grid, WVector3d const& pos );

        /**
         * Calculate the distance from a given position to the nearest voxel boundary
         * on the ray from the position along the given direction.
         *
         * \param grid The grid.
         * \param pos The starting position of the ray.
         * \param dir The normalized direction of the ray.
         *
         * \return The distance to the next voxel boundary.
         *
         * \note pos + getDistanceToBoundary( grid, pos, dir ) * dir will be a position on a voxel boundary
         */
        static double getDistanceToBoundary( Grid3DPtr grid, WVector3d const& pos, WVector3d const& dir );
    };

    //////////////////////////////////////////////////////////////////////////////////////////

    /**
     * \class WThreadedTrackingFunction
     *
     * Implements a generalized multithreaded tracking algorithm. A function that calculates the direction
     * and a function that calculates a new position have to be provided.
     *
     * Output values can be retrieved via two visitor functions that get called per fiber tracked and
     * per point calculated respectively.
     *
     * There are a certain number n of seeds per direction, this meens n*n*n seeds per voxel. For every
     * seed, m fibers get integrated. These two parameters are the seedPositions and seedsPerVoxel parameters
     * of the constructor, respectively.
     *
     * A 'cubic' region of the grid can be chosen for seeding. The v0 and v1 parameters of the constructor
     * are the starting/target voxel coords. Example:
     *
     * v0: 1, 1, 1
     * v1: 4, 5, 3
     *
     * In this case, only voxels between coords 1 to 3 in the x-direction, the voxels 1 to 4 in y- and the voxels 1 to 2
     * in z-direction are used for seeding.
     *
     * Note that voxels at the first (0) and last (grid->getNbCoords*()) position in any direction are
     * invalid seeding voxels as they are partially outside of the grid.
     */
    class WThreadedTrackingFunction : public WThreadedJobs< WTrackingUtility::DataSetType, WTrackingUtility::JobType >
    {
    //! make the test a friend
    friend class ::WThreadedTrackingFunctionTest;

    //! the job type
    typedef WTrackingUtility::JobType JobType;

    //! the dataset type
    typedef WTrackingUtility::DataSetType DataSetType;

    //! a pointer to a dataset
    typedef WTrackingUtility::DataSetPtr DataSetPtr;

    //! the grid type
    typedef WGridRegular3D GridType;

    //! a pointer to the grid
    typedef boost::shared_ptr< GridType > GridPtr;

    //! the direction calculation function
    typedef WTrackingUtility::DirFunc DirFunc;

    //! the path integration function
    typedef boost::function< bool ( DataSetPtr, JobType&, DirFunc const& ) > NextPositionFunc;

    //! a visitor function for fibers
    typedef boost::function< void ( std::vector< WVector3d > const& ) > FiberVisitorFunc;

    //! a visitor function type for points
    typedef boost::function< void ( WVector3d const& ) > PointVisitorFunc;

    //! the base class, a threaded job function
    typedef WThreadedJobs< DataSetType, JobType > Base;

    //! this type
    typedef WThreadedTrackingFunction This;

    public:
        /**
         * Constructor.
         *
         * \param dataset A pointer to a dataset.
         * \param dirFunc A direction calculation function.
         * \param nextFunc A position integration function.
         * \param fiberVst A visitor for fibers.
         * \param pointVst A visitor for points.
         * \param seedPositions The number of seed positions in every direction per voxel.
         * \param seedsPerPos The number of fibers startet from every seed position.
         * \param v0 A vector of starting voxel indices for every direction.
         * \param v1 A vector of target voxel indices for every direction.
         */
        WThreadedTrackingFunction( DataSetPtr dataset, DirFunc dirFunc, NextPositionFunc nextFunc,
                FiberVisitorFunc fiberVst, PointVisitorFunc pointVst,
                std::size_t seedPositions = 1, std::size_t seedsPerPos = 1,
                std::vector< int > v0 = std::vector< int >(),
                std::vector< int > v1 = std::vector< int >() );

        /**
         * Destructor.
         */
        virtual ~WThreadedTrackingFunction();

        /**
         * The job generator.
         *
         * \param job The next job (output).
         *
         * \return false, iff there are no more jobs.
         */
        virtual bool getJob( JobType& job ); // NOLINT

        /**
         * The calculation per job.
         *
         * \param input The input dataset.
         * \param job The job.
         */
        virtual void compute( DataSetPtr input, JobType const& job );

    private:
        /**
         * \class IndexType
         *
         * An index for seed positions.
         */
        class IndexType
        {
        friend class ::WThreadedTrackingFunctionTest;
        public:
            /**
             * Construct an invalid index.
             */
            IndexType();

            /**
             * Construct an index.
             *
             * \param grid The grid.
             * \param v0 A vector of starting voxel indices for every direction.
             * \param v1 A vector of target voxel indices for every direction.
             * \param seedPositions The number of seed positions in every direction per voxel.
             * \param seedsPerPosition The number of fibers startet from every seed position.
             */
            IndexType( GridPtr grid, std::vector< int > const& v0,
                    std::vector< int > const& v1, std::size_t seedPositions,
                    std::size_t seedsPerPosition );

            /**
             * Increase the index by one, effectively generating the next seed position.
             *
             * \return *this
             */
            IndexType& operator++ ();

            /**
             * Check if there aren't any more seed positions.
             *
             * \return true, iff there aren't any more seed positions.
             */
            bool done();

            /**
             * Create a job from this index.
             *
             * \return The job that is the current position.
             */
            JobType job();

        private:
            //! a pointer to the grid
            GridPtr m_grid;

            //! true, iff there are no more seeds
            bool m_done;

            //! the position in the seed space
            boost::array< std::size_t, 4 > m_pos;

            //! the minimum position in the seed space
            boost::array< std::size_t, 4 > m_min;

            //! the maximum position in the seed space
            boost::array< std::size_t, 4 > m_max;

            //! the relative (to the size of a voxel) distance between seeds
            double m_offset;
            };

            //! a pointer to the grid
            GridPtr m_grid;

            //! a function that returns the next direction
            DirFunc m_directionFunc;

            //! a function that calculates the next position
            NextPositionFunc m_nextPosFunc;

            //! the fiber visitor
            FiberVisitorFunc m_fiberVisitor;

            //! the point visitor
            PointVisitorFunc m_pointVisitor;

            //! the maximum number of points per forward/backward integration of a fiber
            std::size_t m_maxPoints;

            //! the current index/seed position
            WSharedObject< IndexType > m_currentIndex;
        };

} /* namespace wtracking */

#endif  // WTHREADEDTRACKINGFUNCTION_H
