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

#ifndef WLINE_TEST_H
#define WLINE_TEST_H

#include <string>
#include <sstream>

#include <cxxtest/TestSuite.h>

#include "../../common/WLimits.h"
#include "../WLine.h"
#include "WLineTraits.h"

/**
 * Unit tests the WLine class
 */
class WLineTest : public CxxTest::TestSuite
{
public:
    /**
     * When printing a line to stdout it the output should be as follows:
     * [first_pos, ..., last_pos] where the positions also should be in
     * the same format, so in the end we would have:
     * [[0,1,0],...[0,0,0]]
     */
    void testOutputOperator( void )
    {
        using wmath::WPosition;
        wmath::WLine l;
        l.push_back( WPosition( 1., 1., 3.1415 ) );
        l.push_back( WPosition( 0., 0., .44 ) );
        l.push_back( WPosition( 1., 1., 1. ) );
        std::string expected( "[[1.0000000000000000e+00, 1.0000000000000000e+00, 3.1415000000000002e+00], "
                               "[0.0000000000000000e+00, 0.0000000000000000e+00, 4.4000000000000000e-01], "
                               "[1.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00]]" );
        std::stringstream ss;
        ss << l;
        TS_ASSERT_EQUALS( expected, ss.str() );
    }

    /**
     * Two Lines are equal if they have the same points in the same order.
     */
    void testEqualityOperator( void )
    {
        using wmath::WPosition;
        wmath::WLine line1;
        line1.push_back( WPosition( 1., 1., 3.1415 ) );
        line1.push_back( WPosition( 0., 0., .44 ) );
        line1.push_back( WPosition( 1., 1., 1. ) );
        wmath::WLine line2;
        line2.push_back( WPosition( 1., 1., 3.1415 ) );
        line2.push_back( WPosition( 0., 0., .44 ) );
        line2.push_back( WPosition( 1., 1., 1. ) );
        TS_ASSERT_EQUALS( line1, line2 );
        line2.back()[1] += 0.0000000001;
        TS_ASSERT_DIFFERS( line1, line2 );
    }

    /**
     * When accessing an item within 0..length-1 a const reference to the
     * WPosition object should be returned.
     */
    void testAccessOperatorWithinValidBounds( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 1., 1., 3.1415 ) );
        line.push_back( WPosition( 0., 0., .44 ) );
        line.push_back( WPosition( 1., 1., 1. ) );
        WPosition expected( 1., 1., 1. );
        TS_ASSERT_EQUALS( expected, line[2] );
    }

    /**
     * If for example the start and end points of two fibers are in opposite
     * direction we cant use them for center line generation. One of the
     * fiber tracts must reverse its ordering.
     */
    void testReverseOrdering( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 1, 2, 3 ) );
        line.push_back( WPosition( 4, 5, 6 ) );
        line.push_back( WPosition( 7, 8, 9 ) );
        wmath::WLine expected;
        expected.push_back( WPosition( 7, 8, 9 ) );
        expected.push_back( WPosition( 4, 5, 6 ) );
        expected.push_back( WPosition( 1, 2, 3 ) );
        line.reverseOrder();
        TS_ASSERT_EQUALS( line, expected );
    }

    /**
     * The path length of the fiber is the accumulated path lengths of all
     * segements (point to point) in that fiber.
     */
    void testPathLength( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 1, 2, 3 ) );
        line.push_back( WPosition( 4, 5, 6 ) );
        line.push_back( WPosition( 7, 8, 9 ) );
        double expected = ( WPosition( 1, 2, 3 ) - WPosition( 4, 5, 6 ) ).norm() +
                          ( WPosition( 4, 5, 6 ) - WPosition( 7, 8, 9 ) ).norm();
        TS_ASSERT_EQUALS( expected, line.pathLength() );
    }

    /**
     * When resampling a line a new line is generated having the given number
     * of sampling points. Its start and end points remain the same, but its
     * curvature may change a bit and also its length!
     * Down sampling means you will have
     */
    void testDownSampleLine( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 0, 0, 0 ) );
        line.push_back( WPosition( 1, 1, 0 ) );
        line.push_back( WPosition( 2, 0, 0 ) );
        line.push_back( WPosition( 3, 1, 0 ) );
        line.resample( 3 );
        wmath::WLine expected;
        expected.push_back( WPosition( 0, 0, 0 ) );
        expected.push_back( WPosition( 1.5, 0.5, 0 ) );
        expected.push_back( WPosition( 3, 1, 0 ) );
        TS_ASSERT_EQUALS( line.size(), expected.size() );
        for( size_t i = 0; i < line.size(); ++i )
        {
            for( int j = 0; j < 3; ++j )
            {
                TS_ASSERT_DELTA( expected[i][j], line[i][j], wlimits::DBL_EPS );
            }
        }
    }
};
#endif  // WLINE_TEST_H
