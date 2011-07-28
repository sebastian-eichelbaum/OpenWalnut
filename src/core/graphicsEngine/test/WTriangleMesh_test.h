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

#include <algorithm>
#include <list>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../WTriangleMesh.h"
#include "WTriangleMeshTraits.h"

/**
 * Testing the WTriangleMesh class.
 */
class WTriangleMeshTest : public CxxTest::TestSuite
{
public:
// TODO(schurade): fix unit test from old triangle class
//    /**
//     * Ensure instatiation does not throw and does initialization right.
//     */
//    void testInstatiation()
//    {
//        TS_ASSERT_THROWS_NOTHING( WTriangleMesh() );
//        WTriangleMesh mesh;
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
//    }
//
//    /**
//     * clearMesh should reset all members.
//     */
//    void testClearMesh()
//    {
//        WTriangleMesh mesh;
//        mesh.m_fastAddVertId = 9;  // just a number
//        mesh.m_fastAddTriangleId = 10;  // just a number
//        mesh.m_vertices.resize( 11 );  // just some size
//        mesh.m_triangles.resize( 13 );  // just some size
//
//        mesh.clearMesh();
//
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
//        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 0 );
//        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 0 );
//    }
//
//    /**
//     * fastAddVert should increment the corresponding counter
//     */
//    void testFastAddVert()
//    {
//        WTriangleMesh mesh;
//        mesh.resizeVertices( 2 );
//        WPosition newVert;
//
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
//        mesh.fastAddVert( newVert );
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 1 );
//        mesh.fastAddVert( newVert );
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 2 );
//    }
//
//    /**
//     * fastAddTriangle should increment the corresponding counter
//     */
//    void testFastAddTriangle()
//    {
//        WTriangleMesh mesh;
//        mesh.resizeTriangles( 2 );
//
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
//        mesh.fastAddTriangle( 0, 1, 2 );
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 1 );
//        mesh.fastAddTriangle( 1, 2, 3 );
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 2 );
//    }
//
//    /**
//     * setVertices should set the fastAddId and resize the member
//     */
//    void testSetVertices()
//    {
//        WTriangleMesh mesh;
//        std::vector< WPosition > vertices( 4 );
//
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 0 );
//        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 0 );
//        mesh.setVertices( vertices );
//        TS_ASSERT_EQUALS( mesh.m_fastAddVertId, 4 );
//        TS_ASSERT_EQUALS( mesh.m_vertices.size(), 4 );
//    }
//
//    /**
//     * setTriangles should set the fastAddId and resize the member
//     */
//    void testSetTriangles()
//    {
//        WTriangleMesh mesh;
//        std::vector< Triangle > triangles( 5 );
//
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 0 );
//        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 0 );
//        mesh.setTriangles( triangles );
//        TS_ASSERT_EQUALS( mesh.m_fastAddTriangleId, 5 );
//        TS_ASSERT_EQUALS( mesh.m_triangles.size(), 5 );
//    }
//
//    /**
//     * Test getTriangleNormal
//     */
//    void testgetTriangleNormal()
//    {
//        WTriangleMesh mesh;
//
//        mesh.resizeVertices( 5 );
//        WPosition newVert0( 0, 0, 0 );
//        mesh.fastAddVert( newVert0 );
//
//        WPosition newVert1( 1.3, 2.25, 3.435 );
//        mesh.fastAddVert( newVert1 );
//
//        WPosition newVert2( 1, 2, 3 );
//        mesh.fastAddVert( newVert2 );
//
//        WPosition newVert3( 1.2, 1.5, 1.7 );
//        mesh.fastAddVert( newVert3 );
//
//        WPosition newVert4( 1, 1, 1 );
//        mesh.fastAddVert( newVert4 );
//
//        // one triangles
//        mesh.resizeTriangles( 1 );
//        mesh.fastAddTriangle( 0, 2, 4 );
//
//        WVector3d expectedNormal( -0.40824829, 0.816496581, -0.40824829 );
//
//        double delta = 1e-7;
//        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[0], expectedNormal[0], delta );
//        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[1], expectedNormal[1], delta );
//        TS_ASSERT_DELTA( mesh.getTriangleNormal( 0 )[2], expectedNormal[2], delta );
//    }
//
//    /**
//     * Test computeTriNormals
//     */
//    void testComputeTriNormals()
//    {
//        WTriangleMesh mesh;
//
//        mesh.resizeVertices( 5 );
//        WPosition newVert0( 0, 0, 0 );
//        mesh.fastAddVert( newVert0 );
//
//        WPosition newVert1( 1.3, 2.25, 3.435 );
//        mesh.fastAddVert( newVert1 );
//
//        WPosition newVert2( 1, 2, 3 );
//        mesh.fastAddVert( newVert2 );
//
//        WPosition newVert3( 1.2, 1.5, 1.7 );
//        mesh.fastAddVert( newVert3 );
//
//        WPosition newVert4( 1, 1, 1 );
//        mesh.fastAddVert( newVert4 );
//
//        // two triangles
//        mesh.resizeTriangles( 2 );
//        mesh.fastAddTriangle( 0, 2, 4 );
//        mesh.fastAddTriangle( 0, 1, 4 );
//
//        TS_ASSERT_EQUALS( mesh.m_computedTriNormals, false );
//        TS_ASSERT_EQUALS( mesh.m_triNormals.size(), 0 );
//
//        mesh.computeTriNormals();
//
//        TS_ASSERT_EQUALS( mesh.m_computedTriNormals, true );
//        TS_ASSERT_EQUALS( mesh.m_triNormals.size(), 2 );
//
//        double delta = 1e-7;
//        WVector3d expectedNormal( -0.40824829, 0.816496581, -0.40824829 );
//        TS_ASSERT_DELTA( length( expectedNormal ), 1, delta ); // check ouu expectance :-)
//
//        TS_ASSERT_DELTA( length( mesh.m_triNormals[0] ), 1, delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][0], expectedNormal[0], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][1], expectedNormal[1], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][2], expectedNormal[2], delta );
//        TS_ASSERT_DELTA( length( mesh.m_triNormals[1] ), 1, delta );
//    };
//
//    /**
//     * Test computeVertNormals
//     */
//    void testComputeVertNormals()
//    {
//        WTriangleMesh mesh;
//
//        mesh.resizeVertices( 5 );
//        WPosition newVert0( 0, 0, 0 );
//        mesh.fastAddVert( newVert0 );
//
//        WPosition newVert1( 1.3, 2.25, 3.435 );
//        mesh.fastAddVert( newVert1 );
//
//        WPosition newVert2( 1, 2, 3 );
//        mesh.fastAddVert( newVert2 );
//
//        WPosition newVert3( 1.2, 1.5, 1.7 );
//        mesh.fastAddVert( newVert3 );
//
//        WPosition newVert4( 1, 1, 1 );
//        mesh.fastAddVert( newVert4 );
//
//        // two triangles
//        mesh.resizeTriangles( 2 );
//        mesh.fastAddTriangle( 0, 2, 4 );
//        mesh.fastAddTriangle( 0, 1, 4 );
//
//        TS_ASSERT_EQUALS( mesh.m_computedVertNormals, false );
//        TS_ASSERT_EQUALS( mesh.m_vertNormals.size(), 0 );
//
//        // check this too. We need the tri normals for the vert normals
//        TS_ASSERT_EQUALS( mesh.m_computedTriNormals, false );
//        TS_ASSERT_EQUALS( mesh.m_triNormals.size(), 0 );
//
//        // call the funtion itself
//        mesh.computeVertNormals();
//
//        // what we expect
//        WVector3d expectedNormal0( -0.40824829, 0.816496581, -0.40824829 );
//        WVector3d expectedNormal1( -0.452271958, 0.814852852, -0.362580895 );
//        double delta = 1e-7;
//
//        // check this triangle stuff too. We need the tri normals for the vert normals
//        TS_ASSERT_EQUALS( mesh.m_computedTriNormals, true );
//        TS_ASSERT_EQUALS( mesh.m_triNormals.size(), 2 );
//        TS_ASSERT_DELTA( length( mesh.m_triNormals[0] ), 1, delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][0], expectedNormal0[0], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][1], expectedNormal0[1], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[0][2], expectedNormal0[2], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[1][0], expectedNormal1[0], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[1][1], expectedNormal1[1], delta );
//        TS_ASSERT_DELTA( mesh.m_triNormals[1][2], expectedNormal1[2], delta );
//
//        // check the vertex stuff
//        TS_ASSERT_EQUALS( mesh.m_computedVertNormals, true );
//        TS_ASSERT_EQUALS( mesh.m_vertNormals.size(), 5 );
//        // vertex 2 belongs only to one triangle so it should have the normal of the triangle
//        TS_ASSERT_DELTA( mesh.m_vertNormals[2][0], expectedNormal0[0], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[2][1], expectedNormal0[1], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[2][2], expectedNormal0[2], delta );
//        // vertex 1 belongs only to one triangle so it should have the normal of the triangle
//        TS_ASSERT_DELTA( mesh.m_vertNormals[1][0], expectedNormal1[0], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[1][1], expectedNormal1[1], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[1][2], expectedNormal1[2], delta );
//
//        // vertex 0 belongs to two triangles so it should have the average normal of the two triangles
//        WVector3d expectedVertNormal = 0.5 * ( expectedNormal0 + expectedNormal1 );
//        expectedVertNormal.normalize();
//        TS_ASSERT_DELTA( mesh.m_vertNormals[0][0], expectedVertNormal[0], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[0][1], expectedVertNormal[1], delta );
//        TS_ASSERT_DELTA( mesh.m_vertNormals[0][2], expectedVertNormal[2], delta );
//    }

    /**
     * Two WTriangleMeshes are considered to be equal only if they have the same
     * points in same order and the same indices of points for the triangles also
     * in same order.
     * From an objective point of view there might be different ordering resuling
     * in the same structure, but this is to expensive to compute.
     */
    void testEqualityOperator( void )
    {
        WTriangleMesh mesh( 3, 1 );
        mesh.addVertex( WPosition( 1,  0, 0 ) );
        mesh.addVertex( WPosition( 0,  1, 0 ) );
        mesh.addVertex( WPosition( 1,  0, 0 ) );
        mesh.addTriangle(  0,  1,  2 );

        WTriangleMesh expected( mesh );

        TS_ASSERT_EQUALS( expected, mesh );
        std::swap( mesh.m_triangles[0], mesh.m_triangles[1] );
        TS_ASSERT_DIFFERS( expected, mesh );
    }

    /**
     * Decompose the following scene into seven components A, B, C, D, E, F, G.
     \verbatim

     12      o-----o          o----o
            / \   / \         |\  /|
           /   \ /   \        | \/ |
     11   o-----o-----o       | /\ |
           \   / \   /        |/  \|
     10     \ /   \ /         o----o B
      9      o-----o  A

      8            o
      7      o     |\
            / \    | \
           /   \   |  \
      6   o-----o--+--o\
      5          \ o-+--o C
                  \ /
      4            o D

      3   o E (degenerated)

      2   o-----o F (degenerated)

      1   o-----o G
           \   /
            \ /
      0      o

          0  1  2  3  4 5     6    7

     \endverbatim
     */
    void testComponentDecomposition( void )
    {
        WTriangleMesh mesh( 25, 16 );

        mesh.addVertex( WPosition( 1,  0, 0 ) ); // 0
        mesh.addVertex( WPosition( 0,  1, 0 ) ); // 1
        mesh.addVertex( WPosition( 2,  1, 0 ) ); // 2
        mesh.addVertex( WPosition( 0,  2, 0 ) ); // 3
        mesh.addVertex( WPosition( 2,  2, 0 ) ); // 4
        mesh.addVertex( WPosition( 0,  3, 0 ) ); // 5
        mesh.addVertex( WPosition( 3,  4, 0 ) ); // 6
        mesh.addVertex( WPosition( 3,  5, 0 ) ); // 7
        mesh.addVertex( WPosition( 5,  5, 0 ) ); // 8
        mesh.addVertex( WPosition( 0,  6, 0 ) ); // 9
        mesh.addVertex( WPosition( 2,  6, 0 ) ); // 10
        mesh.addVertex( WPosition( 4,  6, 0 ) ); // 11
        mesh.addVertex( WPosition( 1,  7, 0 ) ); // 12
        mesh.addVertex( WPosition( 3,  8, 0 ) ); // 13
        mesh.addVertex( WPosition( 1,  9, 0 ) ); // 14
        mesh.addVertex( WPosition( 3,  9, 0 ) ); // 15
        mesh.addVertex( WPosition( 6, 10, 0 ) ); // 16
        mesh.addVertex( WPosition( 7, 10, 0 ) ); // 17
        mesh.addVertex( WPosition( 0, 11, 0 ) ); // 18
        mesh.addVertex( WPosition( 2, 11, 0 ) ); // 19
        mesh.addVertex( WPosition( 4, 11, 0 ) ); // 20
        mesh.addVertex( WPosition( 1, 12, 0 ) ); // 21
        mesh.addVertex( WPosition( 3, 12, 0 ) ); // 23
        mesh.addVertex( WPosition( 6, 12, 0 ) ); // 23
        mesh.addVertex( WPosition( 7, 12, 0 ) ); // 24

        mesh.addTriangle(  0,  1,  2 ); // 0
        mesh.addTriangle(  3,  4,  4 ); // 1
        mesh.addTriangle(  5,  5,  5 ); // 2
        mesh.addTriangle(  9, 10, 12 ); // 3
        mesh.addTriangle( 10,  6, 11 ); // 4
        mesh.addTriangle(  7,  8, 13 ); // 5
        mesh.addTriangle( 18, 14, 19 ); // 6
        mesh.addTriangle( 14, 15, 19 ); // 7
        mesh.addTriangle( 15, 20, 19 ); // 8
        mesh.addTriangle( 20, 22, 19 ); // 9
        mesh.addTriangle( 22, 21, 19 ); // 10
        mesh.addTriangle( 21, 18, 19 ); // 11
        mesh.addTriangle( 16, 17, 23 ); // 12
        mesh.addTriangle( 16, 17, 24 ); // 13
        mesh.addTriangle( 16, 23, 24 ); // 14
        mesh.addTriangle( 17, 23, 24 ); // 15

        boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > components = tm_utils::componentDecomposition( mesh );
        TS_ASSERT_EQUALS( components->size(), 7 );

        std::list< boost::shared_ptr< WTriangleMesh > >::const_iterator cit = components->begin();
        WTriangleMesh expected( 3, 1 );
        expected.addVertex( WPosition( 1, 0, 0 ) );
        expected.addVertex( WPosition( 0, 1, 0 ) );
        expected.addVertex( WPosition( 2, 1, 0 ) );
        expected.addTriangle( 0, 1, 2 );
        TS_ASSERT_EQUALS( **cit, expected );

//        expected.resizeVertices( 2 );
//        expected.m_vertices[ 0 ] = WPosition( 0, 2, 0 );
//        expected.m_vertices[ 1 ] = WPosition( 2, 2, 0 );
//        expected.resizeTriangles( 1 );
//        Triangle x1 = { { 0, 1, 1 } }; // NOLINT
//        expected.m_triangles[ 0 ] = x1;
//        TS_ASSERT_EQUALS( **( ++cit ), expected );
//
//        expected.resizeVertices( 1 );
//        expected.m_vertices[ 0 ] = WPosition( 0, 3, 0 );
//        expected.resizeTriangles( 1 );
//        Triangle x2 = { { 0, 0, 0 } }; // NOLINT
//        expected.m_triangles[ 0 ] = x2;
//        TS_ASSERT_EQUALS( **( ++cit ), expected );
//
//        expected.resizeVertices( 5 );
//        expected.m_vertices[ 0 ] = WPosition( 0,  6, 0 );
//        expected.m_vertices[ 1 ] = WPosition( 2,  6, 0 );
//        expected.m_vertices[ 2 ] = WPosition( 1,  7, 0 );
//        expected.m_vertices[ 3 ] = WPosition( 3,  4, 0 );
//        expected.m_vertices[ 4 ] = WPosition( 4,  6, 0 );
//        expected.resizeTriangles( 2 );
//        Triangle x3 = { { 0, 1, 2 } }; // NOLINT
//        Triangle x4 = { { 1, 3, 4 } }; // NOLINT
//        expected.m_triangles[ 0 ] = x3;
//        expected.m_triangles[ 1 ] = x4;
//        TS_ASSERT_EQUALS( **( ++cit ), expected );
    }

    /**
     * On an empty mesh an empty list should be generated.
     */
    void testComponentDecompositionOnEmptyMesh( void )
    {
        WTriangleMesh mesh( 0, 0 );
        boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > result = tm_utils::componentDecomposition( mesh );
        TS_ASSERT( result->empty() );
    }
};
#endif  // WTRIANGLEMESH_TEST_H
