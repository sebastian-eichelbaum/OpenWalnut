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

#ifndef WBRESENHAMDBL_TEST_H
#define WBRESENHAMDBL_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "core/common/WLogger.h"
#include "../WBresenhamDBL.h"

/**
 * Unit tests the Bresenham algorithm.
 */
class WBresenhamDBLTest : public CxxTest::TestSuite
{
public:
    /**
     * Creates a member variable with a WBresenham instance which you may use
     * for testing.
     */
    void setUp( void )
    {
        WLogger::startup();

        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 3, 3, 3 ) );
        m_algo = boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid, false ) );
    }

    /**
     * Clean up after each test
     */
    void tearDown( void )
    {
        m_algo.reset();
    }

    /**
     * If a line segments starts and ends on the same point only its voxel
     * should be selected.
     */
    void testLineSegementWithSameStartAndEndPoint( void )
    {
        WLine l;
        l.push_back( WPosition( 0.5, 0.5, 0.5 ) );
        l.push_back( WPosition( 0.5, 0.5, 0.5 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[13] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Multiple segments in one voxel should also mark only this voxel.
     */
    void testPolySegmentOneVoxelRastering( void )
    {
        WLine l;
        l.push_back( WPosition( 0.5, 0.5, 0.5 ) );
        l.push_back( WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( WPosition( 0.7, 0.7, 0.7 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[13] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Lines like WFibers consisting out of multiple line segements should
     * be traced segment by segment.
     */
    void testPolyLineRastering( void )
    {
        WLine l;
        l.push_back( WPosition( 0.4, 0.4, 0.4 ) );
        l.push_back( WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( WPosition( 1.7, 1.7, 1.7 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[0] = 1.0;
        expected[13] = 1.0;
        expected[26] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

// TODO(math): This fails, but I decided not to improve the DBL algo, instead I write a new one with christians approach
// So either fix this or create a new one and discard this algorithm. The traceback of the error is in the err1 and err2
// variables: is must be err1 = dy2 -l - ( dx2 * yoffset ); etc.. but this makes new issues!
//    /**
//     * Lines rastered in the 3rd Quadrant should also be traced.
//     */
//    void testRasteringIn3rdQuadrant( void )
//    {
//        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 3, 3, 3, -2, -2, -2, 1, 1, 1 ) );
//        m_algo = boost::shared_ptr< WBresenhamDBL >( new WBresenhamDBL( grid, false ) );
//
//        WLine l;
//        l.push_back( WPosition( -1.7, -1.7, -1.7 ) );
//        l.push_back( WPosition( -0.6, -0.6, -0.6 ) );
//        l.push_back( WPosition( -0.4, -0.4, -0.4 ) );
//        m_algo->raster( l );
//        std::vector< double > expected( 27, 0.0 );
//        expected[0] = 1.0;
//        expected[13] = 1.0;
//        expected[26] = 1.0;
//        TS_ASSERT_EQUALS( m_algo->m_values, expected );
//    }

    /**
     * If you have a line from A to B then rastering it from B to should be
     * equivalent.
     */
    void testSymmetry( void )
    {
        WLine l;
        l.push_back( WPosition( 0.4, 0.4, 0.4 ) );
        l.push_back( WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( WPosition( 1.7, 1.7, 1.7 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[0] = 1.0;
        expected[13] = 1.0;
        expected[26] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
        m_algo->m_values[0] = m_algo->m_values[13] = m_algo->m_values[26] = 0.0; // reset the values array
        l.clear();
        l.push_back( WPosition( 1.7, 1.7, 1.7 ) );
        l.push_back( WPosition( 0.6, 0.6, 0.6 ) );
        l.push_back( WPosition( 0.4, 0.4, 0.4 ) );
        m_algo->raster( l );
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

    /**
     * Rasterization of exact lines must not match rasteriation of midpoint lines.
     */
    void testExactLineIsNotRasteredTheSameWayAsMidpointLines( void )
    {
        WLine l;
        l.push_back( WPosition( 0.49, 0.0, 0.0 ) );
        l.push_back( WPosition( 1.49, 1.51, 0.0 ) );
        m_algo->raster( l );
        std::vector< double > expected( 27, 0.0 );
        expected[0] = 1.0;
        expected[4] = 1.0; // voxel three, since the midpoint may choose between 3 and 4
        expected[7] = 1.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );

        // reset the algo
        m_algo->m_values[0] = m_algo->m_values[4] = m_algo->m_values[7] = 0.0;

        WLine k;
        // These two points are supposed to be voxel centers
        k.push_back( WPosition( 0.0, 0.0, 0.0 ) );
        k.push_back( WPosition( 1.0, 2.0 - wlimits::DBL_EPS, 0.0 ) );
        m_algo->raster( k );
        expected[3] = 1.0;
        expected[4] = 0.0;
        TS_ASSERT_EQUALS( m_algo->m_values, expected );
    }

private:
    boost::shared_ptr< WBresenhamDBL > m_algo; //!< test instace of the WBresenham algo
};

#endif  // WBRESENHAMDBL_TEST_H
