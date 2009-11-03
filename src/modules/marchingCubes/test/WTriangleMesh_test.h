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

#ifndef WTRIANGLEMESH_TEST_H
#define WTRIANGLEMESH_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WTriangleMesh.h"
#include "../../../math/WVector3D.h"

/**
 * Testing the WTriangleMesh class.
 */
class WTriangleMeshTest : public CxxTest::TestSuite
{
public:
    /**
     * Ensure instatiation does not throw and does initialization right.
     */
    void testInstatiation()
    {
        TS_ASSERT_THROWS_NOTHING( WTriangleMesh() );
        WTriangleMesh mesh;
        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
    }

    /**
     * clearMesh should reset all members.
     */
    void testClearMesh()
    {
        WTriangleMesh mesh;
        mesh.m_fastAddVertId = 9;  // just a number
        mesh.m_fastAddTriangleId = 10;  // just a number
        mesh.m_vertices.resize( 11 );  // just some size
        mesh.m_triangles.resize( 13 );  // just some size

        mesh.clearMesh();

        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 0 );
        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 0 );
    }

    /**
     * fastAddVert should increment the corresponding counter
     */
    void testFastAddVert()
    {
        WTriangleMesh mesh;
        mesh.resizeVertices( 2 );
        wmath::WPosition newVert;

        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
        mesh.fastAddVert( newVert );
        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 1 );
        mesh.fastAddVert( newVert );
        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 2 );
    }

    /**
     * fastAddTriangle should increment the corresponding counter
     */
    void testFastAddTriangle()
    {
        WTriangleMesh mesh;
        mesh.resizeTriangles( 2 );

        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
        mesh.fastAddTriangle( 0, 1, 2 );
        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 1 );
        mesh.fastAddTriangle( 1, 2, 3 );
        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 2 );
    }

    /**
     * setVertices should set the fastAddId and resize the member
     */
    void testSetVertices()
    {
        WTriangleMesh mesh;
        std::vector< wmath::WPosition > vertices( 4 );

        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 0 );
        mesh.setVertices( vertices );
        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 4 );
        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 4 );
    }

    /**
     * setTriangles should set the fastAddId and resize the member
     */
    void testSetTriangles()
    {
        WTriangleMesh mesh;
        std::vector< Triangle > triangles( 5 );

        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 0 );
        mesh.setTriangles( triangles );
        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 5 );
        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 5 );
    }

    /**
     * Test getTriangleNormal
     */
    void testgetTriangleNormal()
    {
        WTriangleMesh mesh;

        mesh.resizeVertices( 5 );
        wmath::WPosition newVert0( 0, 0, 0 );
        mesh.fastAddVert( newVert0 );

        wmath::WPosition newVert1( 1.3, 2.25, 3.435 );
        mesh.fastAddVert( newVert1 );

        wmath::WPosition newVert2( 1, 2, 3 );
        mesh.fastAddVert( newVert2 );

        wmath::WPosition newVert3( 1.2, 1.5, 1.7 );
        mesh.fastAddVert( newVert3 );

        wmath::WPosition newVert4( 1, 1, 1 );
        mesh.fastAddVert( newVert4 );

        mesh.resizeTriangles( 1 );
        mesh.fastAddTriangle( 0, 2, 4 );

        wmath::WVector3D expectedNormal( -0.40824829, 0.816496581, -0.40824829 );

        double delta = 1e-7;
        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[0], expectedNormal[0], delta );
        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[1], expectedNormal[1], delta );
        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[2], expectedNormal[2], delta );
    }
};

#endif  // WTRIANGLEMESH_TEST_H
