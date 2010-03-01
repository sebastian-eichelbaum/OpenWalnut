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

#ifndef WGEGEOMETRYUTILS_TEST_H
#define WGEGEOMETRYUTILS_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WGEGeometryUtils.h"


/**
 * Tests for the GeometryUtils
 */
class WGEGeometryUtilsTest : public CxxTest::TestSuite
{
public:
    /**
     * Test triangulate()
     */
    void testTriangulate( void )
    {
        std::vector< wmath::WPosition > points( 4 );
        points[0] = wmath::WPosition( 0.0, 0.0, 0.0 );
        points[1] = wmath::WPosition( 1.0, 0.0, 0.0 );
        points[2] = wmath::WPosition( 1.0, 1.0, 0.0 );
        points[3] = wmath::WPosition( 2.0, 1.0, 0.0 );

        WTriangleMesh mesh = wge::triangulate( points );

        // test number of generated triangles
        TS_ASSERT_EQUALS( mesh.getNumTriangles(), 2 );

        // test by how many triangles the points are surrounded
        TS_ASSERT_EQUALS( mesh.getPositionTriangleNeighborsSlow( 0 ).size(), 1 );
        TS_ASSERT_EQUALS( mesh.getPositionTriangleNeighborsSlow( 1 ).size(), 2 );
        TS_ASSERT_EQUALS( mesh.getPositionTriangleNeighborsSlow( 2 ).size(), 2 );
        TS_ASSERT_EQUALS( mesh.getPositionTriangleNeighborsSlow( 3 ).size(), 1 );

        // This is enough for a unique topologie of the triangulation. The IDs
        // of the triangles and the order of the vertices at each triangle may
        // vary.
    }
};

#endif  // WGEGEOMETRYUTILS_TEST_H