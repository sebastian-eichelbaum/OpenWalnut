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

#ifndef WMMARCHINGCUBES_TEST_H
#define WMMARCHINGCUBES_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../../common/WLogger.h"
#include "../../../math/WPosition.h"
#include "../../../utils/WIOTools.h"
#include "../WMMarchingCubes.h"

static WLogger logger;
static bool loggerInitialized = false;

/**
 * Test for WMMarchingCubes
 */
class WMMarchingCubesTest : public CxxTest::TestSuite
{
public:

    /**
     * Setup method called before every test case.
     */
    void setUp()
    {
        if ( !loggerInitialized )
        {
            std::cout << "Initialize logger." << std::endl;
            logger.setColored( false );

            // NOTE: the logger does not need to be run, since the logger main thread just prints the messages. If compiled in
            // debug mode, the messages will be printed directly, without the logger thread.
            //logger.run();
            loggerInitialized = true;
        }
    }

    /**
     * Ensure instatiation does not throw and does initialization right.
     */
    void testInstatiation()
    {
        TS_ASSERT_THROWS_NOTHING( WMMarchingCubes() );

        // Test initialization
        WMMarchingCubes mc;
        TS_ASSERT_EQUALS( mc.m_nCellsX, 0 );
        TS_ASSERT_EQUALS( mc.m_nCellsY, 0 );
        TS_ASSERT_EQUALS( mc.m_nCellsZ, 0 );
        TS_ASSERT_EQUALS( mc.m_fCellLengthX, 1 );
        TS_ASSERT_EQUALS( mc.m_fCellLengthY, 1 );
        TS_ASSERT_EQUALS( mc.m_fCellLengthZ, 1 );
        TS_ASSERT_EQUALS( mc.m_tIsoLevel, 100 );
        TS_ASSERT_EQUALS( mc.m_idToVertices.size(), 0 );
        TS_ASSERT_EQUALS( mc.m_trivecTriangles.size(), 0 );
    }

    /**
     * Test interpolate on edge
     */
    void testInterpolate()
    {
        WMMarchingCubes mc;
        mc.m_tIsoLevel = 1.7;  // mu = 0.5454...

        WPointXYZId expected;
        expected.newID = 0;
        expected.x = 1.3545454545454545;
        expected.y = 2.4545454545454545;
        expected.z = 5.4090909090909091;

        WPointXYZId result = mc.interpolate( 1.3, 2.4, 3.5,
                                             1.4, 2.5, 7.0,
                                             1.1, 2.2 );

        double delta = 1e-9;
        TS_ASSERT_DELTA( expected.x, result.x, delta );
        TS_ASSERT_DELTA( expected.y, result.y, delta );
        TS_ASSERT_DELTA( expected.z, result.z, delta );
        TS_ASSERT_EQUALS( expected.newID, result.newID );
    }

    /**
     * Test computation of veretexID
     */
    void testGetVertexID()
    {
        WMMarchingCubes mc;
        mc.m_nCellsX = 10;
        mc.m_nCellsY = 11;
        mc.m_nCellsZ = 12;

        unsigned int x = 4;
        unsigned int y = 5;
        unsigned int z = 6;

        unsigned int nbVertsInXDir = ( mc.m_nCellsX + 1 );
        unsigned int nbVertsInSlice = nbVertsInXDir * ( mc.m_nCellsY + 1 );
        unsigned int expected = z * nbVertsInSlice + y * nbVertsInXDir + x;

        TS_ASSERT_EQUALS( expected, mc.getVertexID( x, y, z ) );
    }

    /**
     * Test computation of egeId
     */
    void testGetEdgeID()
    {
        WMMarchingCubes mc;
        mc.m_nCellsX = 10;
        mc.m_nCellsY = 11;
        mc.m_nCellsZ = 12;

        unsigned int nbVertsInXDir = ( mc.m_nCellsX + 1 );
        unsigned int nbVertsInSlice = nbVertsInXDir * ( mc.m_nCellsY + 1 );

        // test edge numbers for (0,0,0) case
        TS_ASSERT_EQUALS( 1 , mc.getEdgeID( 0, 0, 0, 0 ) );
        TS_ASSERT_EQUALS( 3 * nbVertsInXDir , mc.getEdgeID( 0, 0, 0, 1 ) );
        TS_ASSERT_EQUALS( 3 * 1 + 1 , mc.getEdgeID( 0, 0, 0, 2 ) );
        TS_ASSERT_EQUALS( 0 , mc.getEdgeID( 0, 0, 0, 3 ) );
        TS_ASSERT_EQUALS( 3 * nbVertsInSlice + 1 , mc.getEdgeID( 0, 0, 0, 4 ) );
        TS_ASSERT_EQUALS( 3 * ( nbVertsInSlice + nbVertsInXDir ), mc.getEdgeID( 0, 0, 0, 5 ) );
        TS_ASSERT_EQUALS( 3 * ( 1 + nbVertsInSlice ) + 1, mc.getEdgeID( 0, 0, 0, 6 ) );
        TS_ASSERT_EQUALS( 3 * nbVertsInSlice, mc.getEdgeID( 0, 0, 0, 7 ) );
        TS_ASSERT_EQUALS( 2 , mc.getEdgeID( 0, 0, 0, 8 ) );
        TS_ASSERT_EQUALS( 3 * nbVertsInXDir + 2, mc.getEdgeID( 0, 0, 0, 9 ) );
        TS_ASSERT_EQUALS( 3 * ( 1 + nbVertsInXDir ) + 2, mc.getEdgeID( 0, 0, 0, 10 ) );
        TS_ASSERT_EQUALS( 3 * 1 + 2, mc.getEdgeID( 0, 0, 0, 11 ) );

        // wrong edge numbers should return -1
        TS_ASSERT_EQUALS( -1 , mc.getEdgeID( 0, 0, 0, -1 ) );
        TS_ASSERT_EQUALS( -1 , mc.getEdgeID( 0, 0, 0, 12 ) );
        TS_ASSERT_DIFFERS( -1 , mc.getEdgeID( 0, 0, 0, 1 ) );
    }

    /**
     * Test calculateIntersection with unsigned char
     */
    void testCalculateIntersectionUnsignedChar()
    {
        WMMarchingCubes mc;
        mc.m_tIsoLevel = 1.7;
        mc.m_fCellLengthX = 1;
        mc.m_fCellLengthY = 1;
        mc.m_fCellLengthZ = 1;

        std::vector< unsigned char > data;
        data.push_back( 0 );
        data.push_back( 1 );
        data.push_back( 2 );
        data.push_back( 3 );
        data.push_back( 4 );
        data.push_back( 5 );
        data.push_back( 6 );
        data.push_back( 7 );

        boost::shared_ptr< WValueSet< unsigned char > > vals;
        vals = boost::shared_ptr< WValueSet< unsigned char > >( new WValueSet< unsigned char >( 0, 1, data, W_DT_UNSIGNED_CHAR ) );

        WPointXYZId expected;
        expected.newID = 0;
        expected.x = 0.7;
        expected.y = 1;
        expected.z = 0;

        WPointXYZId result = mc.calculateIntersection( vals, 0, 0, 0, 1 );

        double delta = 1e-9;
        TS_ASSERT_DELTA( expected.x, result.x, delta );
        TS_ASSERT_DELTA( expected.y, result.y, delta );
        TS_ASSERT_DELTA( expected.z, result.z, delta );
        TS_ASSERT_EQUALS( expected.newID, result.newID );
    }


    /**
     * Test calculateIntersection with float
     */
    void testCalculateIntersectionFloat()
    {
        WMMarchingCubes mc;
        mc.m_tIsoLevel = 1.7;
        mc.m_fCellLengthX = 1;
        mc.m_fCellLengthY = 1;
        mc.m_fCellLengthZ = 1;

        std::vector< float > data;
        data.push_back( 0 );
        data.push_back( 1 );
        data.push_back( 2 );
        data.push_back( 3 );
        data.push_back( 4 );
        data.push_back( 5 );
        data.push_back( 6 );
        data.push_back( 7 );

        boost::shared_ptr< WValueSet< float > > vals;
        vals = boost::shared_ptr< WValueSet< float > >( new WValueSet< float >( 0, 1, data, W_DT_FLOAT ) );

        WPointXYZId expected;
        expected.newID = 0;
        expected.x = 0.7;
        expected.y = 1;
        expected.z = 0;

        WPointXYZId result = mc.calculateIntersection( vals, 0, 0, 0, 1 );

        double delta = 1e-9;
        TS_ASSERT_DELTA( expected.x, result.x, delta );
        TS_ASSERT_DELTA( expected.y, result.y, delta );
        TS_ASSERT_DELTA( expected.z, result.z, delta );
        TS_ASSERT_EQUALS( expected.newID, result.newID );
    }

    /**
     * Test renderSurface
     */
    void testRenderSurface()
    {
        // TODO(wiebel): MC need to test this for #116
    }

    /**
     * Test renderMesh
     */
    void testRenderMesh()
    {
        // TODO(wiebel): MC need to test this for #116
    }

    /**
     * Test rejection of surfaces with zero triangles or vertices for save
     */
    void testSaveZero()
    {
        WMMarchingCubes mc;
        WTriangleMesh triMesh;
        std::string fileName = wiotools::tempFileName();

        bool result = mc.save( fileName, triMesh );
        TS_ASSERT_EQUALS( result, false ); // should return false as we did not have any vertices or triangles.
        TS_ASSERT( !wiotools::fileExists( fileName ) );
    }

    /**
     * Test rejection of surfaces with nan or inf in coordinates for save
     */
    void testSaveInfinteNan()
    {
        WMMarchingCubes mc;
        WTriangleMesh triMesh;

        const unsigned int nbPos = 10;
        std::vector< Triangle > triangles( 0 );
        Triangle tri;
        for( unsigned int i = 0; i < 3; ++i )
        {
            tri.pointID[i] = i;
        }
        triangles.push_back( tri );

        std::vector< wmath::WPosition > vertices( 0 );
        for( unsigned int posId = 0; posId < nbPos; ++posId )
        {
            double x = posId * posId + 3.4;
            double y = posId + 1;
            double z = 3. /  static_cast< double >( posId ); // provide nan values by dividing with zero
            vertices.push_back( wmath::WPosition( x, y, z ) );
        }
        triMesh.setVertices( vertices );

        std::string fileName = wiotools::tempFileName();

        mc.save( fileName, triMesh );

        bool result = mc.save( fileName, triMesh );
        TS_ASSERT_EQUALS( result, false ); // should return false as we did not have all coordinates values finite.
        TS_ASSERT( !wiotools::fileExists( fileName ) );
    }

    /**
     * Test reading of surfaces
     */
    void testLoad()
    {
        if( !loggerInitialized )
        {
            std::cout << "Initialize logger." << std::endl;
            logger.run();
            loggerInitialized = true;
        }

        WMMarchingCubes mc;
        WTriangleMesh triMesh;
        triMesh = mc.load( "./fixtures/surfaceMeshFile.vtk" );

        TS_ASSERT_EQUALS( triMesh.getNumTriangles(), 28 );
        TS_ASSERT_EQUALS( triMesh.getNumVertices(), 16 );
        TS_ASSERT_EQUALS( triMesh.getTriangleVertexId( 0, 0 ), 0 );
        TS_ASSERT_EQUALS( triMesh.getTriangleVertexId( 1, 0 ), 0 );
        TS_ASSERT_EQUALS( triMesh.getTriangleVertexId( 1, 2 ), 5 );
        TS_ASSERT_EQUALS( triMesh.getTriangleVertexId( 14, 1 ), 7 );

        wmath::WPosition expectedPosition( 93.5, 115.5, 41.5 );
        TS_ASSERT_EQUALS( triMesh.getVertex( 0 ), expectedPosition );
        TS_ASSERT_EQUALS( triMesh.getVertex( 5 ), expectedPosition );
        TS_ASSERT_EQUALS( triMesh.getVertex( 7 ), expectedPosition );
        TS_ASSERT_DIFFERS( triMesh.getVertex( 8 ), expectedPosition );

        TS_ASSERT_THROWS( mc.load( "no such file" ), std::runtime_error );
    }

    /**
     * Test first saving data and the loading it back.
     */
    void testSaveAndLoad()
    {
        WMMarchingCubes mc;
        WTriangleMesh triMesh;

        const unsigned int nbPos = 10;
        const unsigned int nbTris = nbPos - 2;
        std::vector< Triangle > triangles( 0 );
        for( unsigned int triId = 0; triId < nbTris; ++triId )
        {
            Triangle tri;
            for( unsigned int i = 0; i < 3; ++i )
            {
                tri.pointID[i] = triId + i;
            }
            triangles.push_back( tri );
        }
        triMesh.setTriangles( triangles );

        std::vector< wmath::WPosition > vertices( 0 );
        for( unsigned int posId = 0; posId < nbPos; ++posId )
        {
            double x = posId * posId + 3.4;
            double y = posId + 1;
            double z = 3. / static_cast< double >( posId + 1 );
            vertices.push_back( wmath::WPosition( x, y, z ) );
        }
        triMesh.setVertices( vertices );

        std::string fileName = wiotools::tempFileName();

        mc.save( fileName, triMesh );
        WTriangleMesh result = mc.load( fileName );

        TS_ASSERT_EQUALS( triMesh.getNumTriangles(), result.getNumTriangles() );
        TS_ASSERT_EQUALS( triMesh.getNumVertices(), result.getNumVertices() );
        TS_ASSERT_EQUALS( triMesh.getFastAddVertId(), result.getFastAddVertId() );
        TS_ASSERT_EQUALS( triMesh.getFastAddTriangleId(), result.getFastAddTriangleId() );

        for( unsigned int i = 0; i < nbPos; ++i)
        {
            for( unsigned int j = 0; j < 3; ++j)
            {
                double delta = 1e-5;
                // TODO(wiebel): find out why this works only for delta=1e-5
                TS_ASSERT_DELTA( triMesh.getVertex( i )[j], result.getVertex( i )[j], delta );
            }
        }
        TS_ASSERT( wiotools::fileExists( fileName ) );
        std::remove( fileName.c_str() );
    }
};

#endif  // WMMARCHINGCUBES_TEST_H
