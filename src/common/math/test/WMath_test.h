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

#include <cxxtest/TestSuite.h>

#include "../WMath.h"

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
};

#endif  // WMATH_TEST_H
