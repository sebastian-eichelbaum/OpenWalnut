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

#ifndef WMATH_TEST_H
#define WMATH_TEST_H

#include <iomanip>

#include <cxxtest/TestSuite.h>

#include "../WMath.h"
#include "WPositionTraits.h"

using wmath::WPosition;
using wmath::WVector3D;

/**
 * Unit tests wmath's helper functions.
 */
class WMathTest : public CxxTest::TestSuite
{
public:
    /**
     * If some points are inside the plane the triangle is considered to intersect with that plane.
     */
    void testTrianglePlaneIntersectionWithPointsInPlane( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        TS_ASSERT( wmath::testIntersectTriangle( WPosition( 0, 0, 0 ), WPosition( 0, 0, 1 ), WPosition( 1, 0, 1 ), p ) );
        TS_ASSERT( wmath::testIntersectTriangle( WPosition( 0, 0, 0 ), WPosition( 0, 1, 0 ), WPosition( 1, 0, 1 ), p ) );
        TS_ASSERT( wmath::testIntersectTriangle( WPosition( 0, 0, 0 ), WPosition( 0, 1, 0 ), WPosition( 1, 0, 0 ), p ) );
    }

    /**
     * If no point is in plane but at not all are on one side the trinagle intersects.
     */
    void testTrianglePlaneIntersectionWithNoPointInPlane( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        TS_ASSERT( wmath::testIntersectTriangle( WPosition( 0, 0, -1 ), WPosition( 0, 0, 1 ), WPosition( 1, 0, 1 ), p ) );
    }

    /**
     * If all point are on one side of the plane no intersection can take place.
     */
    void testTrianglePlaneIntersectionWithAllPointsOnOneSide( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        TS_ASSERT( !wmath::testIntersectTriangle( WPosition( 0, 0, -1 ), WPosition( 0, 1, -1 ), WPosition( 1, 0, -3 ), p ) );
    }


    /**
     * The intersection of a plane plane with a segment, having a starting and
     * an endpoint. It should be a commutative operation in terms of staring
     * and ending points.
     */
    void testIntersectionOfPlaneWithSegment( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 3, 0, 1 ), wmath::WPosition( 1, 0, -1 ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT_EQUALS( *cutPoint, wmath::WPosition( 2, 0, 0 ) );
        actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 1, 0, -1 ), wmath::WPosition( 3, 0, 1 ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT_EQUALS( *cutPoint, wmath::WPosition( 2, 0, 0 ) );
    }

    /**
     * If there is no intersection at all the position 0,0,0 should be written
     * into the point of intersection and false should be returned.
     */
    void testNoIntersectionOfPlaneWithSegment( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = wmath::intersectPlaneSegment( p,
                                                    wmath::WPosition( 3, 0, 10 * wlimits::DBL_EPS ),
                                                    wmath::WPosition( 10 * wlimits::DBL_EPS, 0, 10 * wlimits::DBL_EPS ), cutPoint );
        TS_ASSERT_EQUALS( actual, false );
        TS_ASSERT_EQUALS( *cutPoint, wmath::WPosition( 0, 0, 0 ) );
    }

    /**
     * If an endpoint is inside of the plane then a point of intersection
     * must exists and true must be returned.
     */
    void testIntersectionOfPlaneWithSegmentHavingOnePointInPlane( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 3, 0, wlimits::DBL_EPS ), wmath::WPosition( 1, 0, 1 ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 3.0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
        actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 1, 0, 1 ), wmath::WPosition( 3, 0, wlimits::DBL_EPS ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 3.0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
    }

    /**
     * If the segment is totally inside of the plane, the first point must be
     * returned which is actually one (of many intersecting points)
     * intersecting point and true must be returned.
     */
    void testIntersectionOfPlaneWithSegmentInPlane( void )
    {
        WPlane p( WVector3D( 0, 0, 1 ), WPosition( 0, 0, 0 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 3, 0, 0 ), wmath::WPosition( 1, 0, 0 ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT_EQUALS( *cutPoint, wmath::WPosition( 3, 0, 0 ) );
        actual = wmath::intersectPlaneSegment( p, wmath::WPosition( 1, 0, 0 ), wmath::WPosition( 3, 0, 0 ), cutPoint );
        TS_ASSERT_EQUALS( actual, true );
        TS_ASSERT_EQUALS( *cutPoint, wmath::WPosition( 1, 0, 0 ) );
    }

    /**
     * If a set of consecutive segements (aka line) is tested again
     * intersection with that plane, each segment should be tested.
     */
    void testIntersectionOfPlaneWithLine( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( -1, 0, 0 ) );
        l.push_back( wmath::WPosition( 1, 0, 0 ) );
        l.push_back( wmath::WPosition( 2, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 4, 0 ) );
        WPlane p( WVector3D( 1, 0, 0 ), WPosition( 3.5, 13, 23 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = intersectPlaneLineNearCP( p, l, cutPoint );
        TS_ASSERT( actual );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 3.5 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
    }

    /**
     * If none of the segments of the line intersects with the plane, false and
     * 0,0,0 should be returned.
     */
    void testNoIntersectionOfPlaneWithLine( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( -1, 0, 0 ) );
        l.push_back( wmath::WPosition( 1, 0, 0 ) );
        l.push_back( wmath::WPosition( 2, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 4, 0 ) );
        WPlane p( WVector3D( 1, 0, 0 ), WPosition( 5.0001, 13, 23 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = intersectPlaneLineNearCP( p, l, cutPoint );
        TS_ASSERT( !actual );
        WPlane q( WVector3D( -1.1, 0, 0 ), WPosition( 5.0001, 13, 23 ) );
        actual = intersectPlaneLineNearCP( q, l, cutPoint );
        TS_ASSERT( !actual );
    }

    /**
     * If an endpoint of a line intersects with the plane, this point should be
     * returned and ofcourse true.
     */
    void testIntersectionPlaneLineWithJustOnePoint( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( -1, 0, 0 ) );
        l.push_back( wmath::WPosition( 1, 0, 0 ) );
        l.push_back( wmath::WPosition( 2, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 4, 0 ) );
        WPlane p( WVector3D( 1, 0, 0 ), WPosition( -1, 13, 23 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = intersectPlaneLineNearCP( p, l, cutPoint );
        TS_ASSERT( actual );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - ( -1 ) ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
    }

    /**
     * If a whole segment of the line is insisde that plane the first point
     * should be returned along with true as return code.
     */
    void testIntersectionPlaneLineWithSegmentInPlane( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( -1, 0, 0 ) );
        l.push_back( wmath::WPosition( 1, 0, 0 ) );
        l.push_back( wmath::WPosition( 2, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 0, 0 ) );
        l.push_back( wmath::WPosition( 5, 4, 0 ) );
        WPlane p( WVector3D( 1, 0, 0 ), WPosition( 5, 13, 23 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = intersectPlaneLineNearCP( p, l, cutPoint );
        TS_ASSERT( actual );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 5 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
    }

    /**
     * If many points of intersection occurs the closest to the base point
     * should be selected and return true.
     */
    void testMultipleIntersectionPlaneLine( void )
    {
        wmath::WLine l;
        l.push_back( wmath::WPosition( -1, 0, 0 ) );
        l.push_back( wmath::WPosition( 1, 0, 0 ) );
        l.push_back( wmath::WPosition( 2, 1, 0 ) );
        l.push_back( wmath::WPosition( -1, 1, 0 ) );
        WPlane p( WVector3D( 1, 0, 0 ), WPosition( 0, 0, 0 ) );
        boost::shared_ptr< wmath::WPosition > cutPoint( new wmath::WPosition( 0, 0, 0 ) );
        bool actual = intersectPlaneLineNearCP( p, l, cutPoint );
        TS_ASSERT( actual );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
        WPlane q( WVector3D( -1, 0, 0 ), WPosition( 0, 0.6, 0 ) );
        actual = intersectPlaneLineNearCP( q, l, cutPoint );
        TS_ASSERT( actual );
        TS_ASSERT( std::abs( ( *cutPoint )[0] - 0 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[1] - 1 ) <= 2*wlimits::DBL_EPS );
        TS_ASSERT( std::abs( ( *cutPoint )[2] - 0 ) <= 2*wlimits::DBL_EPS );
    }
};

#endif  // WMATH_TEST_H