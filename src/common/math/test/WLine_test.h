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

#include "../../WLimits.h"
#include "../../exceptions/WOutOfBounds.h"
#include "../WLine.h"
#include "WLineTraits.h"
#include "WPositionTraits.h"

/**
 * Unit tests the WLine class
 */
class WLineTest : public CxxTest::TestSuite
{
public:
    /**
     * If two lines have different lengths they are considered diffrent
     * regardless on the given delta and the first point on which they
     * differ should be returned.
     */
    void testEqualsDeltaDifferentLength( void )
    {
        wmath::WLine line;
        line.push_back( wmath::WPosition( 0, 0, 0 ) );
        line.push_back( wmath::WPosition( 1, 0, 0 ) );
        wmath::WLine other;
        other.push_back( wmath::WPosition( 0, 0, 0 ) );
        TS_ASSERT_EQUALS( line.equalsDelta( other, 0.0 ), 1 );
    }

    /**
     * If both lines are of same size, but there is a point differing on
     * more than the given delta the first point which differs should be
     * returned.
     */
    void testEqualsDeltaOnRealDifferentLines( void )
    {
        wmath::WLine line;
        line.push_back( wmath::WPosition( 0, 0, 0 ) );
        line.push_back( wmath::WPosition( 1, 0, 0 ) );
        wmath::WLine other;
        other.push_back( wmath::WPosition( 0, 0, 0 ) );
        other.push_back( wmath::WPosition( 1, 0, 0 + 2 * wlimits::DBL_EPS ) );
        TS_ASSERT_EQUALS( line.equalsDelta( other, wlimits::DBL_EPS ), 1 );
    }

    /**
     * If both lines are of same size and every point pair don't differ in
     * terms of the given delta, -1 should be returned.
     */
    void testEqualsDeltaOnDifferentLinesButWithinDelta( void )
    {
        wmath::WLine line;
        line.push_back( wmath::WPosition( 0, 0, 0 ) );
        line.push_back( wmath::WPosition( 1, 0, 0 ) );
        wmath::WLine other;
        other.push_back( wmath::WPosition( 0, 0, 0 ) );
        other.push_back( wmath::WPosition( 1, 0, 0 + 2 * wlimits::DBL_EPS ) );
        TS_ASSERT_EQUALS( line.equalsDelta( other, 2 * wlimits::DBL_EPS ), -1 );
    }

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
     * If for example the start and end points of two lines are in opposite
     * direction we may want to change its direction.
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
     * The path length of the line is the accumulated path lengths of all
     * segements (point to point) in that line.
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
        assert_equals_delta( line, expected );
    }

    /**
     * If the resampling rate of a line has as many points as the original line,
     * no sampling should be applied.
     */
    void testSamplingWithSameNumberOfPoints( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        for( size_t i = 0; i < 10; ++i )
        {
            line.push_back( WPosition( i, 3 * i, 10 - i ) );
        }
        TS_ASSERT( line.size() == 10 );
        wmath::WLine expected( line ); // make a copy of the original
        line.resample( 10 );
        assert_equals_delta( line, expected );
    }

    /**
     * If the points are exactly in between of a segement nothing should fail.
     */
    void testSamplingPointsAreExactlyInTheOldSegmentCenterAndCorners( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        wmath::WLine expected;
        for( int i = 0; i < 3; ++i )
        {
            line.push_back( WPosition( i, std::pow( -1.0, i % 2 ), 0 ) );
            expected.push_back( WPosition( i, std::pow( -1.0, i % 2 ), 0 ) );
            expected.push_back( WPosition( i + 0.5, 0, 0 ) );
        }
        expected.pop_back();
        line.resample( 5 );
        assert_equals_delta( expected, line );
    }

    /**
     * When resampling with many sample points the numerical errors may sum
     * up. Hence I watch if it not breaks a certain threshold: 1.0e-10*newSegmentLength.
     */
    void testNumericalStabilityOfResampling( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        wmath::WLine expected;
        for( int i = 0; i < 100; ++i )
        {
            line.push_back( WPosition( i, std::pow( -1.0, i % 2 ), 0 ) );
            expected.push_back( WPosition( i, std::pow( -1.0, i % 2 ), 0 ) );
            expected.push_back( WPosition( i + 0.25, std::pow( -1.0, i % 2 ) * 0.5, 0 ) );
            expected.push_back( WPosition( i + 0.5,  0, 0 ) );
            expected.push_back( WPosition( i + 0.75, std::pow( -1.0, ( i + 1 ) % 2 ) * 0.5, 0 ) );
        }
        expected.pop_back();
        expected.pop_back();
        expected.pop_back();
        line.resample( 4 * 99 + 1 );
        assert_equals_delta( expected, line, 1.0e-10 * std::sqrt( 5.0 ) / 4 );
    }

    /**
     * If there are many new sample points between two old sample points nothing should fail either.
     */
    void testManySampelsInBetweenOfTwoOldPoints( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 0, 0, 0 ) );
        line.push_back( WPosition( 1, 1, 0 ) );
        line.resample( 1001 );
        wmath::WLine expected;
        expected.push_back( WPosition( 0, 0, 0 ) );
        for( size_t i = 1; i < 1001; ++i )
        {
            expected.push_back( WPosition( i / 1000.0, i / 1000.0, 0 ) );
        }
        assert_equals_delta( expected, line, 1.0 / 1001.0 * 1.0e-10 );
    }

   /**
     * The mid point of a WLine is just the point in the middle of the line.
     * For lines with even numbered size the element before that non existing
     * midPoint is selected.
     */
    void testMidPointOnEvenSize( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 0, 0, 0 ) );
        line.push_back( WPosition( 1, 1, 0 ) );
        line.push_back( WPosition( 2, 0, 0 ) );
        line.push_back( WPosition( 3, 1, 0 ) );
        WPosition expected( 1, 1, 0 );
        TS_ASSERT_EQUALS( expected, WPosition( line.midPoint() ) );
    }

    /**
     * When a line has uneven numbered size, the mid point is unique.
     */
    void testMidPointOnUnevenSize( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        line.push_back( WPosition( 0, 0, 0 ) );
        line.push_back( WPosition( 1, 1, 0 ) );
        line.push_back( WPosition( 2, 0, 0 ) );
        WPosition expected( 1, 1, 0 );
        TS_ASSERT_EQUALS( expected, WPosition( line.midPoint() ) );
    }

    /**
     * When calling midPoint on empty lines => there is no point to return
     * hence an exception is generated.
     */
    void testMidPointOnEmptyLine( void )
    {
        using wmath::WPosition;
        wmath::WLine line;
        WPosition expected( 1, 1, 0 );
        TS_ASSERT_THROWS_EQUALS( line.midPoint(), WOutOfBounds &e, std::string( e.what() ), "There is no midpoint for an empty line." );
    }

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
};
#endif  // WLINE_TEST_H
