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

#ifndef WFIBERCLUSTER_TEST_H
#define WFIBERCLUSTER_TEST_H

#include <list>

#include <cxxtest/TestSuite.h>

#include "../../../common/WLimits.h"
#include "../../../common/WLogger.h"
#include "../../test/WDataSetFiberVectorTraits.h"
#include "../WFiberCluster.h"

/**
 * Unit test the WFiberCluster class
 */
class WFiberClusterTest : public CxxTest::TestSuite
{
public:
    /**
     * When merging two clusters the indices are merged and the second cluster
     * becomes empty.
     */
    void testMerge( void )
    {
        WFiberCluster a( 1 );
        WFiberCluster b;
        size_t mydata[] = { 16, 2, 77, 29 }; // NOLINT
        std::list< size_t > data( mydata, mydata + sizeof( mydata ) / sizeof( size_t ) );
        b.m_memberIndices = data;
        a.merge( b );
        TS_ASSERT( b.empty() );
        size_t mxdata[] = { 1, 16, 2, 77, 29 }; // NOLINT
        std::list< size_t > xdata( mxdata, mxdata + sizeof( mxdata ) / sizeof( size_t ) );
        WFiberCluster expected;
        expected.m_memberIndices = xdata;
        TS_ASSERT_EQUALS( expected, a );
    }

    /**
     * Generates a dataset for some unit tests.
     \verbatim
       Fiber B had initially 3 points: [0;2],[2;2],[4;2] after resampling there are 5 points:
                                       [0;2],[1;2],[2;2],[3;2],[4;2]
       Fiber A had initially 7 points: [0;0],[1;1],[2;1],[3;1],[4;1],[5;1],[6;1] after resampling there are 5 points:
                                       [0;0],[1.5;1],[3;1],[4.5;1],[6;1]

           |
        2 -B----B-----B------B-----B
           |
           |
           |   ,.m------m---------m-------m (center line)
           |  /
           | /
        1 -m     aA----a--A--a---A-a----Aa-----A
           |    /
           |   /
           |  /
           | /
           |/
           A-----+-----+-----+-----+-----+-----+---
           0     1     2     3     4     5     6

     \endverbatim
     */
    void testCenterLine( void )
    {
        WFiber expected;
        expected.push_back( WPosition( 0,                                                                                               1, 0 ) );
        expected.push_back( WPosition( ( ( 9.0 - 3 * std::sqrt( 2.0 ) ) / 4.0 - 1.0 ) / 2.0 + 1,                                      1.5, 0 ) );
        expected.push_back( WPosition( ( ( 9.0 - 3 * std::sqrt( 2.0 ) ) / 4.0 + ( 5 + std::sqrt( 2.0 ) ) / 4.0 - 2.0 ) / 2.0 + 2,     1.5, 0 ) );
        expected.push_back( WPosition( ( ( 9.0 - 3 * std::sqrt( 2.0 ) ) / 4.0 + 2 * ( 5 + std::sqrt( 2.0 ) ) / 4.0 - 3.0 ) / 2.0 + 3, 1.5, 0 ) );
        expected.push_back( WPosition( 5,                                                                                             1.5, 0 ) );
        m_cluster->generateCenterLine();
        assert_equals_delta( *m_cluster->getCenterLine(), expected, wlimits::FLT_EPS );
        std::cout << *m_cluster->getDataSetReference() << std::endl;
    }

    /**
     * If there are two fibers which are nearly parallel changing ones direction
     * will have huge effect
     */
    void testUnifyDirectionOnTwoNearlyParallelFibers( void )
    {
        WDataSetFiberVector expected( *m_cluster->getDataSetReference() ); // make a copy

        boost::shared_ptr< WDataSetFiberVector > fibs( new WDataSetFiberVector( expected ) );
        fibs->at( 1 ).reverseOrder(); // simulate wrong direction of the second fiber

        m_cluster->unifyDirection( fibs );
        using string_utils::operator<<;
        TS_ASSERT_EQUALS( *fibs, expected );
    }

    /**
     * Check determination of the direction of a pair of fibers which are in the following shape (all points having the same Y and Z coordinate).
     \verbatim
       AS--------------------------->AE    BS------------------------->BE
     \endverbatim
     */
    void testUnifyDirectionOnTwoConsecutiveFibers( void )
    {
        WFiber a;
        a.push_back( WPosition(  0, 0, 0 ) );
        a.push_back( WPosition(  1, 0, 0 ) );
        a.push_back( WPosition(  2, 0, 0 ) );
        a.push_back( WPosition(  3, 0, 0 ) );
        a.push_back( WPosition(  4, 0, 0 ) );
        a.push_back( WPosition(  5, 0, 0 ) );
        a.push_back( WPosition(  6, 0, 0 ) );
        WFiber b;
        b.push_back( WPosition( 10, 0, 0 ) );
        b.push_back( WPosition( 11, 0, 0 ) );
        b.push_back( WPosition( 12, 0, 0 ) );
        b.push_back( WPosition( 13, 0, 0 ) );
        b.push_back( WPosition( 14, 0, 0 ) );
        b.push_back( WPosition( 15, 0, 0 ) );
        b.push_back( WPosition( 16, 0, 0 ) );
        WDataSetFiberVector expected;
        expected.push_back( a );
        expected.push_back( b );

        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector( expected ) );
        ds->at( 1 ).reverseOrder();
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // second tract should flip over
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // no tract should flip over
    }

    /**
     * Check determination of the direction of a pair of fibers which are in the following shape.
     *
      \verbatim
       AS.                   _BS
          `.              ,-'
            `.           /
              \         /
               \       |
               `.     .'
                 |   |
                 |   |
                 |   |
                 |   |
                 |   |
                 |   |
                AE   BE
      \endverbatim
     */
    void testUnifyDirectionOnTwoCSTShapedFibers( void )
    {
        WFiber a;
        a.push_back( WPosition(  0,  0, 0 ) );
        a.push_back( WPosition(  5,  1, 0 ) );
        a.push_back( WPosition(  8,  3, 0 ) );
        a.push_back( WPosition( 11,  7, 0 ) );
        a.push_back( WPosition( 11, 10, 0 ) );
        a.push_back( WPosition( 11, 12, 0 ) );
        WFiber b;
        b.push_back( WPosition( 23,  0, 0 ) );
        b.push_back( WPosition( 19,  2, 0 ) );
        b.push_back( WPosition( 17,  5, 0 ) );
        b.push_back( WPosition( 15,  7, 0 ) );
        b.push_back( WPosition( 16, 10, 0 ) );
        b.push_back( WPosition( 16, 12, 0 ) );
        WDataSetFiberVector expected;
        expected.push_back( a );
        expected.push_back( b );

        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector( expected ) );
        ds->at( 1 ).reverseOrder();
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // second tract should flip over
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // no tract should flip over
    }

    /**
     * Check determination of the direction of a pair of fibers which are in the following shape.
     *
      \verbatim
             _.-----AS
           ,'
          /      ,-'BS
         |     ,'
         |    /               AE
        |    /           BE    |
        |   |             |    |
        |   |             |    |
         \   \           ,|    |
          \   `.       ,Y'  _,'
           `\_  `''''''  _.'
             '`--------''
      \endverbatim
     */
    void testUnifyDirectionOnTwoCircularShapedFibersInSameCircle( void )
    {
        WFiber a;
        a.push_back( WPosition( 14,  0, 0 ) );
        a.push_back( WPosition(  5,  1, 0 ) );
        a.push_back( WPosition(  2,  4, 0 ) );
        a.push_back( WPosition(  3,  9, 0 ) );
        a.push_back( WPosition( 11, 11, 0 ) );
        a.push_back( WPosition( 19, 10, 0 ) );
        a.push_back( WPosition( 24,  8, 0 ) );
        a.push_back( WPosition( 23,  4, 0 ) );
        WFiber b;
        b.push_back( WPosition( 13,  2, 0 ) );
        b.push_back( WPosition(  7,  4, 0 ) );
        b.push_back( WPosition(  6,  8, 0 ) );
        b.push_back( WPosition( 10, 10, 0 ) );
        b.push_back( WPosition( 17,  9, 0 ) );
        b.push_back( WPosition( 19,  7, 0 ) );
        b.push_back( WPosition( 19,  5, 0 ) );
        WDataSetFiberVector expected;
        expected.push_back( a );
        expected.push_back( b );

        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector( expected ) );
        ds->at( 1 ).reverseOrder();
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // second tract should flip over
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // no tract should flip over
    }

    /**
     * Check determination of the direction of a pair of fibers which are in the following shape.
     *
      \verbatim
            ,,---._
          ,'       \
         /          \
         |           |
         \          ,'
          \        ,'
           `-AS  AE
            BS    BE
          ,'       `.
         /           \
        |             |
        |             |
        |             |
         \           /
          `-._   _,-'
              `''
      \endverbatim
     */
    void testUnifyDirectionOnTwoCircularShapedFibersInDifferentCircle( void )
    {
        WFiber a;
        a.push_back( WPosition(  6,  6, 0 ) );
        a.push_back( WPosition(  3,  5, 0 ) );
        a.push_back( WPosition(  2,  3, 0 ) );
        a.push_back( WPosition(  4,  1, 0 ) );
        a.push_back( WPosition(  7,  0, 0 ) );
        a.push_back( WPosition( 11,  0, 0 ) );
        a.push_back( WPosition( 14,  2, 0 ) );
        a.push_back( WPosition( 13,  4, 0 ) );
        a.push_back( WPosition( 11,  6, 0 ) );
        WFiber b;
        b.push_back( WPosition(  6,  7, 0 ) );
        b.push_back( WPosition(  3,  9, 0 ) );
        b.push_back( WPosition(  1, 11, 0 ) );
        b.push_back( WPosition(  3, 14, 0 ) );
        b.push_back( WPosition(  8, 15, 0 ) );
        b.push_back( WPosition( 13, 14, 0 ) );
        b.push_back( WPosition( 15, 12, 0 ) );
        b.push_back( WPosition( 14,  9, 0 ) );
        b.push_back( WPosition( 11,  7, 0 ) );
        WDataSetFiberVector expected;
        expected.push_back( a );
        expected.push_back( b );

        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector( expected ) );
        ds->at( 1 ).reverseOrder();
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // second tract should flip over
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // no tract should flip over
    }

    /**
     * Check determination of the direction of a pair of fibers which are in the following shape.
     *
      \verbatim
              ,,-'-AE   BS''`-._
            ,'                   `.
           /                       \
          /                         `.
         /                           `.
         /                            |
        |                              |
        |                              |
        |                               |
       |                                |
       |                                |
       |                                |
       AS                               BE
      \endverbatim
     */
    void testUnifyDirectionOnTwoInverseCSTShapedFibers( void )
    {
        WFiber a;
        a.push_back( WPosition(  1, 12, 0 ) );
        a.push_back( WPosition(  1,  9, 0 ) );
        a.push_back( WPosition(  2,  5, 0 ) );
        a.push_back( WPosition(  5,  1, 0 ) );
        a.push_back( WPosition(  9,  0, 0 ) );
        a.push_back( WPosition( 14,  0, 0 ) );
        WFiber b;
        b.push_back( WPosition( 19,  0, 0 ) );
        b.push_back( WPosition( 24,  0, 0 ) );
        b.push_back( WPosition( 29,  2, 0 ) );
        b.push_back( WPosition( 32,  5, 0 ) );
        b.push_back( WPosition( 33,  8, 0 ) );
        b.push_back( WPosition( 33, 12, 0 ) );
        WDataSetFiberVector expected;
        expected.push_back( a );
        expected.push_back( b );

        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector( expected ) );
        ds->at( 1 ).reverseOrder();
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // second tract should flip over
        m_cluster->unifyDirection( ds );
        TS_ASSERT_EQUALS( *ds, expected ); // no tract should flip over
    }

private:
    /**
     * Compares to point sequences (aka lines) with a given delta.
     *
     * \param first First line to compare with
     * \param second Second line to compare with
     * \param delta The delta within two points are considered as equally
     */
    void assert_equals_delta( const WLine& first, const WLine& second, double delta = wlimits::DBL_EPS ) const
    {
        int diffPos = 0;
        if( ( diffPos =  equalsDelta( first, second, delta ) ) != -1 )
        {
            using string_utils::operator<<;
            std::stringstream msg;
            msg << "Lines are different in at least point: " << diffPos;
            TS_FAIL( msg.str() );
            std::cout << "first  line at: " << diffPos << std::endl << first[diffPos] << std::endl;
            std::cout << "second line at: " << diffPos << std::endl << second[diffPos] << std::endl;
            std::cout << "first  line: " << std::endl << first << std::endl;
            std::cout << "second line: " << std::endl << second << std::endl;
        }
    }

    /**
     * Generates a dataset for some unit tests.
     \verbatim
       Fiber B has 3 points: [0;2],[2;2],[4;2]
       Fiber A has 7 points: [0;0],[1;1],[2;1],[3;1],[4;1],[5;1],[6;1]

           |
        2 -b----------b------------b
           |
           |
           |
           |
           |
        1 -|     a-----a-----a-----a-----a-----a
           |    /
           |   /
           |  /
           | /
           |/
           a-----+-----+-----+-----+-----+-----+---
           0     1     2     3     4     5     6

     \endverbatim
     */
    void setUp( void )
    {
        // generate fiber dataset to operate on
        WFiber fib_a;
        fib_a.push_back( WPosition( 0, 0, 0 ) );
        fib_a.push_back( WPosition( 1, 1, 0 ) );
        fib_a.push_back( WPosition( 2, 1, 0 ) );
        fib_a.push_back( WPosition( 3, 1, 0 ) );
        fib_a.push_back( WPosition( 4, 1, 0 ) );
        fib_a.push_back( WPosition( 5, 1, 0 ) );
        fib_a.push_back( WPosition( 6, 1, 0 ) );
        WFiber fib_b;
        fib_b.push_back( WPosition( 0, 2, 0 ) );
        fib_b.push_back( WPosition( 2, 2, 0 ) );
        fib_b.push_back( WPosition( 4, 2, 0 ) );
        boost::shared_ptr< WDataSetFiberVector > ds( new WDataSetFiberVector() );
        ds->push_back( fib_a );
        ds->push_back( fib_b );
        generateFiberCluster( ds );
    }

    /**
     * Generates out of the given dataset a WFiberCluster containing all fibers.
     *
     * \param ds The fiber dataset
     */
    void generateFiberCluster( const boost::shared_ptr< WDataSetFiberVector > ds )
    {
        m_cluster.reset();
        m_cluster = boost::shared_ptr< WFiberCluster >( new WFiberCluster() );
        m_cluster->setDataSetReference( ds );
        std::list< size_t > idx;
        for( size_t i = 0; i < ds->size(); ++i )
        {
            idx.push_back( i );
        }
        m_cluster->setIndices( idx );
    }

    /**
     * Tidyups the dataset used in some unit tests
     */
    void tearDown( void )
    {
        m_cluster.reset();
    }

    boost::shared_ptr< WFiberCluster > m_cluster; //!< pre generated cluster for some unit tests
};

#endif  // WFIBERCLUSTER_TEST_H
