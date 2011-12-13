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

#ifndef WMARCHINGCUBESALGORITHM_TEST_H
#define WMARCHINGCUBESALGORITHM_TEST_H

#include <vector>
#include <cxxtest/TestSuite.h>

#include "../WMarchingCubesAlgorithm.h"

/**
 * Tests for the class computing the actual marching cubes.
 */
class WMarchingCubesAlgorithmTest : public CxxTest::TestSuite
{
public:
    /**
     * Test interpolate on edge
     */
    void testInterpolate()
    {
        WMarchingCubesAlgorithm mc;
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
        WMarchingCubesAlgorithm mc;
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
        WMarchingCubesAlgorithm mc;
        mc.m_nCellsX = 10;
        mc.m_nCellsY = 11;
        mc.m_nCellsZ = 12;

        unsigned int nbVertsInXDir = ( mc.m_nCellsX + 1 );
        unsigned int nbVertsInSlice = nbVertsInXDir * ( mc.m_nCellsY + 1 );

        // test edge numbers for(0,0,0) case
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
        WMarchingCubesAlgorithm mc;
        mc.m_tIsoLevel = 1.7;
        mc.m_nCellsX = 1;
        mc.m_nCellsY = 1;
        mc.m_nCellsZ = 1;

        std::vector< unsigned char > data;
        data.push_back( 0 );
        data.push_back( 1 );
        data.push_back( 2 );
        data.push_back( 3 );
        data.push_back( 4 );
        data.push_back( 5 );
        data.push_back( 6 );
        data.push_back( 7 );

        WPointXYZId expected;
        expected.newID = 0;
        expected.x = 1;
        expected.y = 0.35;
        expected.z = 0;

        // This is the edge between grid pos 3 and 1 which are cell verts 2 and 3
        WPointXYZId result = mc.calculateIntersection( &data, 0, 0, 0, 2 );

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
        WMarchingCubesAlgorithm mc;
        mc.m_tIsoLevel = 1.7;
        mc.m_nCellsX = 1;
        mc.m_nCellsY = 1;
        mc.m_nCellsZ = 1;

        std::vector< float > data;
        data.push_back( 0 );
        data.push_back( 1 );
        data.push_back( 2 );
        data.push_back( 3 );
        data.push_back( 4 );
        data.push_back( 5 );
        data.push_back( 6 );
        data.push_back( 7 );

        WPointXYZId expected;
        expected.newID = 0;
        expected.x = 1;
        expected.y = 0.35;
        expected.z = 0;

        // This is the edge between grid pos 3 and 1 which are cell verts 2 and 3
        WPointXYZId result = mc.calculateIntersection( &data, 0, 0, 0, 2 );

        double delta = 1e-9;
        TS_ASSERT_DELTA( expected.x, result.x, delta );
        TS_ASSERT_DELTA( expected.y, result.y, delta );
        TS_ASSERT_DELTA( expected.z, result.z, delta );
        TS_ASSERT_EQUALS( expected.newID, result.newID );
    }
};

#endif  // WMARCHINGCUBESALGORITHM_TEST_H
