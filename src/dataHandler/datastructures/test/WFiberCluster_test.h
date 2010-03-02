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
        wmath::WFiber expected;
        expected.push_back( wmath::WPosition( 0,                                                                                           1, 0 ) );
        expected.push_back( wmath::WPosition( ( ( 9.0 - 3 * std::sqrt( 2 ) ) / 4.0 - 1.0 ) / 2.0 + 1,                                    1.5, 0 ) );
        expected.push_back( wmath::WPosition( ( ( 9.0 - 3 * std::sqrt( 2 ) ) / 4.0 + ( 5 + std::sqrt( 2 ) ) / 4.0 - 2.0 ) / 2.0 + 2,     1.5, 0 ) );
        expected.push_back( wmath::WPosition( ( ( 9.0 - 3 * std::sqrt( 2 ) ) / 4.0 + 2 * ( 5 + std::sqrt( 2 ) ) / 4.0 - 3.0 ) / 2.0 + 3, 1.5, 0 ) );
        expected.push_back( wmath::WPosition( 5,                                                                                         1.5, 0 ) );

        m_cluster->generateCenterLine();
        assert_equals_delta( *m_cluster->getCenterLine(), expected );
        std::cout << *m_cluster->getDataSetReference() << std::endl;
    }

//    void testUnifyDirection( void )
//    {
//        WDataSetFiberVector expected( *m_cluster->getDataSetReference() ); // make a copy
//
//        boost::shared_ptr< WDataSetFiberVector > fibs = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector( expected ) );
//        fibs->at( 0 ).reverseOrder(); // simulate wrong direction of the first fiber
//
//        m_cluster->unifyDirection( fibs );
//        using string_utils::operator<<;
//        TS_ASSERT_EQUALS( *fibs, expected );
//    }

private:
    /**
     * TS_ASSERT_DELTA needs the operator+, operator- and operator< to be implemented especially for WPositions the operator< and operator +
     * makes not really sense. Hence I implemented an assert by my one, giving reasonable out put.
     *
     * \param first First line to compare with
     * \param second Second line to compare with
     * \param delta The delta within two points are considered as equally
     */
    void assert_equals_delta( const wmath::WLine& first, const wmath::WLine& second, double delta = wlimits::DBL_EPS ) const
    {
        int diffPos = 0;
        if( ( diffPos = first.equalsDelta( second, delta ) ) != -1 )
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
        using wmath::WFiber;
        using wmath::WPosition;
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
        boost::shared_ptr< WDataSetFiberVector > ds = boost::shared_ptr< WDataSetFiberVector >( new WDataSetFiberVector() );
        ds->push_back( fib_a );
        ds->push_back( fib_b );

        m_cluster = boost::shared_ptr< WFiberCluster >( new WFiberCluster() );
        m_cluster->setDataSetReference( ds );
        size_t idxData[] = { 0, 1 }; // NOLINT
        m_cluster->setIndices( std::list< size_t >( idxData, idxData + 2 ) );
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
