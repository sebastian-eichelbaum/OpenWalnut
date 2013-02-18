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

#ifndef WTHREADEDTRACKINGFUNCTION_TEST_H
#define WTHREADEDTRACKINGFUNCTION_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../common/WLogger.h"
#include "../WThreadedTrackingFunction.h"

/**
 * \class WTrackingUtilityTest
 *
 * Test the utility functions for tracking.
 */
class WTrackingUtilityTest : public CxxTest::TestSuite
{
    //! an abbreviation
    typedef WTrackingUtilityTest This;

public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Test if positions with a distance less then TRACKING_EPS from a boundary
     * are determined correctly by onBoundary().
     */
    void testBoundary()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 0.0, 1.0, 0.0 ) );
        // test the test
        TS_ASSERT( ds );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );

        WVector3d v( 1.0, 0.0, 0.0 );
        v += x * -0.5;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 0 ] += 0.5 * TRACKING_EPS;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 2 ] -= 2.0 * TRACKING_EPS;
        v[ 1 ] += 1.6 * TRACKING_EPS;
        TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 0.5;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 0 ] -= 0.6 * TRACKING_EPS;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 2 ] += 1.5 * TRACKING_EPS;
        v[ 1 ] += 2.6 * TRACKING_EPS;
        TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + y * 1.77 + z * 0.65;
        TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + y * 1.0 + z * 0.65;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 3.5 + y * 1.77 + z * 0.65;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 1.5 + y * 1.77 + z * 0.65;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 3.9 + y * 5.0 + z * 0.65;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 2.3 + y * 7.73 + z * 3.75;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 3.4 + y * 1.77 + z * 6.75;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 3.5 + y * 5.0 + z * 0.65;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 1 ] -= 0.7 * TRACKING_EPS;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, v ) );
        v[ 0 ] += 3.5 * TRACKING_EPS;
        v[ 1 ] += 0.7 * TRACKING_EPS;
        TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, v ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 1.2 + y * 7.9 + z * 5.3;
        TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, v ) );
    }

    /**
     * Check if getDistanceToBoundary() returns the correct distance.
     */
    void testGetDistanceToBoundary()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );

        WVector3d v( 1.0, 0.0, 0.0 );

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 0.0, 1.0, 0.0 ) );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );

        TS_ASSERT_DELTA( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, x ), 0.5, TRACKING_EPS );
        TS_ASSERT_DELTA( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, y ), 1.0, TRACKING_EPS );
        TS_ASSERT_DELTA( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, z ), 0.75, TRACKING_EPS );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 0.5;
        TS_ASSERT_THROWS( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, x ), WException );

        v -= x * 2.0 * TRACKING_EPS;
        TS_ASSERT_DELTA( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, x ), 2.0 * TRACKING_EPS, TRACKING_EPS );

        v = WVector3d( 2.9741, 3.527, 1.992 );
        WVector3d dir( 3, 4, -2.003 );
        dir = normalize( dir );
        double t = wtracking::WTrackingUtility::getDistanceToBoundary( g, v, dir );
        WVector3d res = v + dir * t;
        TS_ASSERT( wtracking::WTrackingUtility::onBoundary( g, res ) );

        v = WVector3d( 1.0, 0.0, 0.0 ) + x * 3.75 + y * 6.65 + z * 5.59;
        TS_ASSERT_DELTA( wtracking::WTrackingUtility::getDistanceToBoundary( g, v, y ), 0.35, TRACKING_EPS );
    }

    /**
     * Test if followToNextVoxel() returns a valid (inside the grid) position
     * in the next voxel (and not on any boundary).
     */
    void testFollowToNextVoxel()
    {
        {
            WVector3d x( 0.707, 0.707, 0.0 );
            WVector3d y( -0.707, 0.707, 0.0 );
            WVector3d z( 0.0, 0.0, 1.0 );
            x = normalize( x );
            y = normalize( y );

            boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 0.0, 1.0, 0.0 ) );
            boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );

            wtracking::WTrackingUtility::JobType j;
            // TODO(wiebel): somehow changing the order of the last multiplication does not find the desired operator*
            j.first = WVector3d( 1.0, 0.0, 0.0 ) + ( x + y + z ) * ( wlimits::FLT_EPS + 0.7 ); // the starting point
            j.second = x; // initial direction
            TS_ASSERT( g->encloses( j.first ) );

            WVector3d v = j.first;

            TS_ASSERT( wtracking::WTrackingUtility::followToNextVoxel( ds, j, boost::bind( &This::simpleDirFunc, this, _1, _2 ) ) );
            TS_ASSERT( !wtracking::WTrackingUtility::onBoundary( g, j.first ) );
            TS_ASSERT( g->encloses( j.first ) );
            v += x * 0.8;
            TS_ASSERT_DELTA( length( j.first - v ), 0.0, 2.0 * TRACKING_EPS );
        }
    }

private:
    /**
     * A simple direction calculation function.
     *
     * \return A direction.
     */
    WVector3d simpleDirFunc( wtracking::WTrackingUtility::DataSetPtr, wtracking::WTrackingUtility::JobType const& )
    {
        WVector3d v( 1.0, 1.0, 0.0 );
        v = normalize( v );
        return v;
    }

    /**
     * Build a test dataset.
     *
     * \param data The vector for every voxel.
     *
     * \return the test dataset
     */
    boost::shared_ptr< WDataSetSingle > buildTestData( WVector3d data )
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );

        x = normalize( x );
        y = normalize( y );

        y *= 2.0;
        z *= 1.5;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];
        mat( 0, 3 ) = 1.0;

        WGridTransformOrtho t( mat );
        boost::shared_ptr< WGrid > g( new WGridRegular3D( 5, 5, 5, t ) );

        data = normalize( data );

        boost::shared_ptr< std::vector< double > > v = boost::shared_ptr< std::vector< double > >( new  std::vector< double > ( 5 * 5 * 5 * 3 ) );
        for( std::size_t k = 0; k < 5 * 5 * 5; ++k )
        {
            v->at( 3 * k + 0 ) = data[ 0 ];
            v->at( 3 * k + 1 ) = data[ 1 ];
            v->at( 3 * k + 2 ) = data[ 2 ];
        }

        boost::shared_ptr< WValueSetBase > vs( new WValueSet< double >( 1, 3, v, W_DT_DOUBLE ) );
        return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( vs, g ) );
    }
};

/**
 * \class WThreadedTrackingFunctionTest
 *
 * Test the WThreadedTrackingFunction class.
 */
class WThreadedTrackingFunctionTest : public CxxTest::TestSuite
{
    //! a handy abbreviation
    typedef WThreadedTrackingFunctionTest This;

public:
    /**
     * Test if everything gets initialized correctly.
     */
    void testIndexInitialization()
    {
        std::vector< int > v0;
        std::vector< int > v1;
        std::size_t numSeeds = 1;
        std::size_t seedsPerPosition = 1;

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 5 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );

        {
            wtracking::WThreadedTrackingFunction::IndexType i;
            i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition );

            // new test if everything was initialized correctly
            TS_ASSERT_EQUALS( i.m_done, false );
            TS_ASSERT_EQUALS( i.m_offset, 1.0 );
            TS_ASSERT_EQUALS( i.m_pos[ 0 ], 1 );
            TS_ASSERT_EQUALS( i.m_pos[ 1 ], 1 );
            TS_ASSERT_EQUALS( i.m_pos[ 2 ], 1 );
            TS_ASSERT_EQUALS( i.m_pos[ 3 ], 0 );
            TS_ASSERT_EQUALS( i.m_min[ 0 ], 1 );
            TS_ASSERT_EQUALS( i.m_min[ 1 ], 1 );
            TS_ASSERT_EQUALS( i.m_min[ 2 ], 1 );
            TS_ASSERT_EQUALS( i.m_min[ 3 ], 0 );
            TS_ASSERT_EQUALS( i.m_max[ 0 ], 4 );
            TS_ASSERT_EQUALS( i.m_max[ 1 ], 4 );
            TS_ASSERT_EQUALS( i.m_max[ 2 ], 4 );
            TS_ASSERT_EQUALS( i.m_max[ 3 ], 1 );
        }
        {
            v0.resize( 3 );
            v1.resize( 3 );
            v0[ 0 ] = 1;
            v0[ 1 ] = 2;
            v0[ 2 ] = 1;
            v1[ 0 ] = 3;
            v1[ 1 ] = 3;
            v1[ 2 ] = 4;
            numSeeds = 4;
            seedsPerPosition = 3;

            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            // new test if everything was initialized correctly
            TS_ASSERT_EQUALS( i.m_done, false );
            TS_ASSERT_EQUALS( i.m_offset, 0.25 );
            TS_ASSERT_EQUALS( i.m_pos[ 0 ], 4 );
            TS_ASSERT_EQUALS( i.m_pos[ 1 ], 8 );
            TS_ASSERT_EQUALS( i.m_pos[ 2 ], 4 );
            TS_ASSERT_EQUALS( i.m_pos[ 3 ], 0 );
            TS_ASSERT_EQUALS( i.m_min[ 0 ], 4 );
            TS_ASSERT_EQUALS( i.m_min[ 1 ], 8 );
            TS_ASSERT_EQUALS( i.m_min[ 2 ], 4 );
            TS_ASSERT_EQUALS( i.m_min[ 3 ], 0 );
            TS_ASSERT_EQUALS( i.m_max[ 0 ], 12 );
            TS_ASSERT_EQUALS( i.m_max[ 1 ], 12 );
            TS_ASSERT_EQUALS( i.m_max[ 2 ], 16 );
            TS_ASSERT_EQUALS( i.m_max[ 3 ], 3 );
        }
        {
            // now lets do something wrong
            wtracking::WThreadedTrackingFunction::IndexType i;
            v0[ 0 ] = -1;
            TS_ASSERT_THROWS( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ), WException );
            v0[ 0 ] = 0;
            v1[ 1 ] = 5;
            TS_ASSERT_THROWS( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ), WException );
        }
    }

    /**
     * Test if indices (seed positions) iteration works.
     */
    void testIndexIncrement()
    {
        std::vector< int > v0;
        std::vector< int > v1;
        std::size_t numSeeds = 1;
        std::size_t seedsPerPosition = 1;

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 5 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );

        {
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            for( int j = 0; j < 27; ++j )
            {
                TS_ASSERT( !i.done() );
                ++i;
            }
            TS_ASSERT( i.done() );
        }
        {
            numSeeds = 4;
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            for( int j = 0; j < 27 * 64; ++j )
            {
                TS_ASSERT( !i.done() );
                ++i;
            }
            TS_ASSERT( i.done() );
        }
        {
            seedsPerPosition = 3;
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            for( int j = 0; j < 27 * 64 * 3; ++j )
            {
                TS_ASSERT( !i.done() );
                ++i;
            }
            TS_ASSERT( i.done() );
        }
        {
            v0.resize( 3, 1 );
            v1.resize( 3 );
            v1[ 0 ] = 4;
            v1[ 1 ] = 3;
            v1[ 2 ] = 4;
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            for( int j = 0; j < 18 * 64 * 3; ++j )
            {
                TS_ASSERT( !i.done() );
                ++i;
            }
            TS_ASSERT( i.done() );
        }
    }

    /**
     * Test if the right jobs get created from seeds.
     */
    void testIndexToJob()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );
        y *= 2.0;
        z *= 1.5;

        std::vector< int > v0;
        std::vector< int > v1;
        std::size_t numSeeds = 2;
        std::size_t seedsPerPosition = 1;

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 5 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );

        {
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            wtracking::WThreadedTrackingFunction::JobType job = i.job();
            WVector3d v = g->getOrigin() + 0.75 * x + 0.75 * y + 0.75 * z;

            std::cout << g->getOrigin() << std::endl;

            TS_ASSERT_DELTA( v[ 0 ], job.first[ 0 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 1 ], job.first[ 1 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 2 ], job.first[ 2 ], TRACKING_EPS );

            v += 0.5 * z;
            ++i;
            job = i.job();
            TS_ASSERT_DELTA( v[ 0 ], job.first[ 0 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 1 ], job.first[ 1 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 2 ], job.first[ 2 ], TRACKING_EPS );

            for( int k = 0; k < 6; ++k )
            {
                ++i;
            }
            v += 0.5 * y;
            job = i.job();
            TS_ASSERT_DELTA( v[ 0 ], job.first[ 0 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 1 ], job.first[ 1 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 2 ], job.first[ 2 ], TRACKING_EPS );

            for( int k = 0; k < 36; ++k )
            {
                ++i;
            }
            v += 0.5 * x;
            job = i.job();
            TS_ASSERT_DELTA( v[ 0 ], job.first[ 0 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 1 ], job.first[ 1 ], TRACKING_EPS );
            TS_ASSERT_DELTA( v[ 2 ], job.first[ 2 ], TRACKING_EPS );

            TS_ASSERT_EQUALS( job.second[ 0 ], 0.0 );
            TS_ASSERT_EQUALS( job.second[ 1 ], 0.0 );
            TS_ASSERT_EQUALS( job.second[ 2 ], 0.0 );
        }
        {
            numSeeds = 4;
            seedsPerPosition = 11;
            wtracking::WThreadedTrackingFunction::IndexType i;
            TS_ASSERT_THROWS_NOTHING( i = wtracking::WThreadedTrackingFunction::IndexType( g, v0, v1, numSeeds, seedsPerPosition ) );

            wtracking::WThreadedTrackingFunction::JobType job;
            WVector3d v = g->getOrigin() + 0.625 * x + 0.625 * y + 0.625 * z;
            for( int k = 0; k < 11; ++k )
            {
                job = i.job();
                TS_ASSERT_DELTA( v[ 0 ], job.first[ 0 ], TRACKING_EPS );
                TS_ASSERT_DELTA( v[ 1 ], job.first[ 1 ], TRACKING_EPS );
                TS_ASSERT_DELTA( v[ 2 ], job.first[ 2 ], TRACKING_EPS );
                ++i;
            }
        }
    }

    /**
     * Test if everything gets initialized correctly.
     */
    void testInstantiation()
    {
        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 5 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );

        TS_ASSERT_THROWS_NOTHING(
                wtracking::WThreadedTrackingFunction w( ds,
                                                        boost::bind( &This::dirFunc, this, _1, _2, WVector3d( 1.0, 0.0, 0.0 ) ),
                                                        boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                        boost::bind( &This::fibVis, this, _1 ),
                                                        boost::bind( &This::pntVis, this, _1 ) ) );
    }

    /**
     * Test if the correct amount of jobs gets created.
     */
    void testGetJob()
    {
        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 7 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );

        wtracking::WThreadedTrackingFunction w( ds, boost::bind( &This::dirFunc, this, _1, _2, WVector3d( 1.0, 0.0, 0.0 ) ),
                                                    boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                    boost::bind( &This::fibVis, this, _1 ),
                                                    boost::bind( &This::pntVis, this, _1 ) );
        wtracking::WThreadedTrackingFunction::JobType job;
        for( int i = 0; i < 125; ++i )
        {
            TS_ASSERT( w.getJob( job ) );
        }
        TS_ASSERT( !w.getJob( job ) );
    }

    /**
     * Test if fibers with the right number of points get created.
     */
    void testCompute()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );

        boost::shared_ptr< WDataSetSingle > ds = buildTestData( WVector3d( 1.0, 0.0, 0.0 ), 7 );
        boost::shared_ptr< WGridRegular3D > g = boost::dynamic_pointer_cast< WGridRegular3D >( ds->getGrid() );
        TS_ASSERT( g );
        {
            wtracking::WThreadedTrackingFunction w( ds, boost::bind( &This::dirFunc, this, _1, _2, x ),
                                                        boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                        boost::bind( &This::fibVis, this, _1 ),
                                                        boost::bind( &This::pntVis, this, _1 ) );
            wtracking::WThreadedTrackingFunction::JobType job;
            m_points.getWriteTicket()->get() = 0;
            w.getJob( job );
            w.compute( ds, job );
            TS_ASSERT_EQUALS( m_points.getReadTicket()->get(), 7 );
        }
        {
            wtracking::WThreadedTrackingFunction w( ds, boost::bind( &This::dirFunc, this, _1, _2, y ),
                                                        boost::bind( &wtracking::WTrackingUtility::followToNextVoxel, _1, _2, _3 ),
                                                        boost::bind( &This::fibVis, this, _1 ),
                                                        boost::bind( &This::pntVis, this, _1 ) );
            wtracking::WThreadedTrackingFunction::JobType job;
            m_points.getWriteTicket()->get() = 0;
            w.getJob( job );
            w.compute( ds, job );
            TS_ASSERT_EQUALS( m_points.getReadTicket()->get(), 7 );
        }
    }

private:
    /**
     * Build a test dataset.
     *
     * \param data The vector for every voxel.
     * \param n The number of grid position in every direction.
     *
     * \return The dataset.
     */
    boost::shared_ptr< WDataSetSingle > buildTestData( WVector3d data, int n )
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );
        y *= 2.0;
        z *= 1.5;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];
        mat( 0, 3 ) = 1.0;

        WGridTransformOrtho t( mat );
        boost::shared_ptr< WGrid > g( new WGridRegular3D( n, n, n, t ) );

        data = normalize( data );

        boost::shared_ptr< std::vector< double > > v = boost::shared_ptr< std::vector< double > >( new  std::vector< double >( n * n * n * 3 ) );
        for( std::size_t k = 0; k < static_cast< std::size_t >( n * n * n ); ++k )
        {
            v->at( 3 * k + 0 ) = data[ 0 ];
            v->at( 3 * k + 1 ) = data[ 1 ];
            v->at( 3 * k + 2 ) = data[ 2 ];
        }

        boost::shared_ptr< WValueSetBase > vs( new WValueSet< double >( 1, 3, v, W_DT_DOUBLE ) );
        return boost::shared_ptr< WDataSetSingle >( new WDataSetSingle( vs, g ) );
    }

    /**
     * The direction computation function. Simply returns the parameter vector.
     *
     * \param j The current job.
     * \param d The direction to return.
     *
     * \return d
     */
    WVector3d dirFunc( wtracking::WThreadedTrackingFunction::DataSetPtr,
                              wtracking::WThreadedTrackingFunction::JobType const& j,
                              WVector3d d )
    {
        if( dot( j.second, d ) > 0.0 )
        {
            return d;
        }
        else
        {
            return d * -1.0;
        }
    }

    /**
     * The fiber visitor. Does nothing.
     */
    void fibVis( std::vector< WVector3d > const& )
    {
    }

    /**
     * The point visitor. Counts the number of points found.
     */
    void pntVis( WVector3d const& )
    {
        ++m_points.getWriteTicket()->get();
    }

    //! the number of points found
    WSharedObject< std::size_t > m_points;
};

#endif  // WTHREADEDTRACKINGFUNCTION_TEST_H
