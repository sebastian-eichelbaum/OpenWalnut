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

#ifndef WGRIDREGULAR3D_TEST_H
#define WGRIDREGULAR3D_TEST_H

#include <cstdio>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../../common/exceptions/WOutOfBounds.h"
#include "../../common/math/test/WVector3dTraits.h"
#include "../../common/WLimits.h"
#include "../WGridRegular3D.h"


/**
 * Tests the WGridRegular3D class.
 */
class WGridRegular3DTest : public CxxTest::TestSuite
{
public:
    /**
     * Called before every test.
     */
    void setUp( void )
    {
        m_delta = 1e-14;
    }

    /**
     * Ensure that nothing is thrown when an instance is created.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 3, 3, 3 ) );
    }

    /**
     * After instantiation there should be the requested number of positions.
     */
    void testSize( void )
    {
        WGridTransformOrtho t( WMatrix< double >( 4, 4 ).makeIdentity() );
        WGridRegular3D grid( 3, 3, 3, t );
        TS_ASSERT_EQUALS( grid.size(), 27 );
    }

    /**
     * Each convinience function just assembles the three values into an boost array.
     */
    void testConvinienceFunctions( void )
    {
        boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( 3, 3, 3 ) );
        boost::array< unsigned int, 3 > expectedNbCoords = { { 3, 3, 3 } }; // NOLINT curly braces
        TS_ASSERT_EQUALS( expectedNbCoords, getNbCoords( grid ) );
        boost::array< double, 3 > expectedOffsets = { { 1.0, 1.0, 1.0 } }; // NOLINT curly braces
        TS_ASSERT_EQUALS( expectedOffsets, getOffsets( grid ) );
        boost::array< WVector3d, 3 > expectedDirections = { { WVector3d( 1.0, 0.0, 0.0 ), WVector3d( 0.0, 1.0, 0.0 ), WVector3d( 0.0, 0.0, 1.0 ) } }; // NOLINT curly braces line length
        TS_ASSERT_EQUALS( expectedDirections, getDirections( grid ) );
        TS_ASSERT_EQUALS( expectedDirections, getUnitDirections( grid ) );
    }

    /**
     * After instantiation there should be the right vectors, matrix and origin.
     */
    void testOrientation( void )
    {
        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = 2.2;
        mat( 1, 1 ) = 3.3;
        mat( 2, 2 ) = 4.4;

        WGridTransformOrtho t( mat );
        WGridRegular3D grid( 3, 3, 3, t );
        TS_ASSERT_EQUALS( grid.size(), 27 );
        TS_ASSERT_EQUALS( grid.getOrigin(), WPosition( 0., 0., 0. ) );
        TS_ASSERT_EQUALS( grid.getDirectionX(), WVector3d( 2.2, 0., 0. ) );
        TS_ASSERT_EQUALS( grid.getDirectionY(), WVector3d( 0., 3.3, 0. ) );
        TS_ASSERT_EQUALS( grid.getDirectionZ(), WVector3d( 0., 0., 4.4 ) );
    }

    /**
     * getNbCoords should return the samples prescribed by the use of the constructor
     */
    void testGetNbCoords( void )
    {
        size_t x = 3;
        size_t y = 4;
        size_t z = 5;
        WGridRegular3D grid( x, y, z );
        TS_ASSERT_EQUALS( grid.getNbCoordsX(), x );
        TS_ASSERT_EQUALS( grid.getNbCoordsY(), y );
        TS_ASSERT_EQUALS( grid.getNbCoordsZ(), z );
    }

    /**
     * getOffset should return the vector offsets prescribed by the use of the
     * constructor
     */
    void testGetVectorOffset( void )
    {
        WVector3d x( 3., 1., 2. );
        WVector3d y( 2., -6., 0. );
        WVector3d z( 12., 4., -20 );

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];

        WGridTransformOrtho t( mat );
        WGridRegular3D grid( 3, 3, 3, t );

        TS_ASSERT_DELTA( grid.getOffsetX(), length( x ), m_delta );
        TS_ASSERT_DELTA( grid.getOffsetY(), length( y ), m_delta );
        TS_ASSERT_DELTA( grid.getOffsetZ(), length( z ), m_delta );
    }

    /**
     * getPosition should return the correct position for scalar offsets
     */
    void testGetPositionScalarOffset( void )
    {
        unsigned int nX = 10, nY = 11, nZ = 12;
        unsigned int iX = 8, iY = 9, iZ = 5;
        unsigned int i = iX + iY * nX + iZ * nX * nY;

        double orX = 1.2;
        double orY = 3.4;
        double orZ = 5.6;

        double ofX = 1.1;
        double ofY = 2.2;
        double ofZ = 3.3;

        double x = orX + iX * ofX;
        double y = orY + iY * ofY;
        double z = orZ + iZ * ofZ;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = ofX;
        mat( 1, 1 ) = ofY;
        mat( 2, 2 ) = ofZ;
        mat( 0, 3 ) = orX;
        mat( 1, 3 ) = orY;
        mat( 2, 3 ) = orZ;

        WPosition expected( x, y, z );
        WGridTransformOrtho t( mat );
        WGridRegular3D grid( nX, nY, nZ, t );

        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( iX, iY, iZ )[2], expected[2], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[0], expected[0], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[1], expected[1], m_delta );
        TS_ASSERT_DELTA( grid.getPosition( i )[2], expected[2], m_delta );
    }

    /**
     * The cell number of a Position is defined as follows:
     *
       \verbatim
        y-axis
          |_____ _____ ___   _
        3 |     |     |       |
          |     |     | ...   + dy
          |_____|_____|___   _|
        2 |     | . Line
          |     |/    | ...
          |_____/___ _|___
        1 |    /|     |
          |   ' |     | ...
          |_____|_____|______ x-axis
         /0     1     2
        / `--.--´
       /    dx
       origin e.g. ( 3.1, 3.2, -6 ) and dx == dy == 1.0 ( the z-axis is ignored in this example )
       \endverbatim
     *
     * Hence the line starts at approx. ( 3.85, 3.7, -6 ) and ends at
     * approx. ( 4.35, 5.0 , -6 ). The Cell number e.g. of the start point
     * is then: 4 and of the end point: 7.
     */
    void testGetVoxelNumberOfGeneralPosition( void )
    {
        using boost::shared_ptr;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 3 ) = 3.1;
        mat( 1, 3 ) = 3.2;
        mat( 2, 3 ) = -6.;

        WGridTransformOrtho t( mat );
        shared_ptr< WGridRegular3D > g = shared_ptr< WGridRegular3D >( new WGridRegular3D( 3, 3, 3, t ) );
        TS_ASSERT_EQUALS( g->getVoxelNum( WPosition( 4.35, 5.0, -6 ) ), 7 );
    }

    /**
     * If a grid point is outside of the grid then -1 should be returned.
     */
    void testGetVoxelNumberOfPositionOutsideOfGrid( void )
    {
        using boost::shared_ptr;

        shared_ptr< WGridRegular3D > g = shared_ptr< WGridRegular3D >( new WGridRegular3D( 3, 3, 3 ) );
        TS_ASSERT_EQUALS( g->getVoxelNum( WPosition( 0 - m_delta, 0, 0 ) ), -1 );
        TS_ASSERT_EQUALS( g->getVoxelNum( WPosition( 0, 2 + m_delta, 0 ) ), -1 );
    }

    /**
     * All points of the surfaces belonging to the lower,left,front corner of a
     * voxel belong to this voxel. Instead all points located on the three
     * surfaces belonging to the upper right back corner are considered not to
     * belong to this voxel.
     */
    void testGetVoxelNumberOfPositionExactlyBetweenVoxels( void )
    {
        // A voxel is defined as this ( a cuboid with a center which is a grid point ):
        //             ______________ ____ (0.5, 0.5, 0.5)
        //            /:            /|
        //           / :           / |
        //          /  :          /  |
        //         /   :         /   |
        //       _/____:_ ___ __/    |
        //        |    :        |    |
        //        |    :    *<--|--------- grid point (0, 0, 0)
        //        |    :........|....|__
        // dz == 1|   ´         |   /
        //        |  ´          |  / dy == 1
        //        | ´           | /
        //       _|´____________|/__
        //        |<- dx == 1 ->|
        // -0.5,-0.5,-0.5
        //
        // the grid is as follows
        //         ______________ ____ 2,2,2
        //        /:     /      /|
        //       / :    /:     / |
        //      /------+------/| |
        //     /   :……/……:………/…|…|
        //    /____:_/___:__/  | |---- 2,2,1
        //    |    : |   :  |  |/|
        //    |    : |   :  |  | |
        //    |    :.|...:..| /| |____ 2,2,0
        //    +------+------+´ |/
        //    |  '   |      |  /---- 2,1,0
        //    | '    |      | /
        //    |'_____|______|/____
        //    |      |      |
        // 0,0,0   1,0,0  2,0,0

        WGridRegular3D  g( 3, 3, 3 );

        // center point of the grid
        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 1, 1, 1 ) ), 13 );

        // front lower left corner of the last cell
        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 1.5, 1.5, 1.5 ) ), 26 );

        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 1, 1, 0.5 ) ), 13 );
        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 0 , 1.5 , 1 ) ), 15 );
        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 0.5, 1, 0 ) ), 4 );

        // origin
        TS_ASSERT_EQUALS( g.getVoxelNum( WPosition( 0, 0, 0 ) ), 0 );
    }

    /**
     * A voxel inside a grid (not located on a border) has 6 neighbours.
     */
    void testNeighboursInsideAGrid( void )
    {
        WGridRegular3D g( 3, 3, 3 );
        size_t data[] = { 12, 14, 10, 16, 4, 22 };
        std::vector< size_t > expected( data, data + 6 );
        TS_ASSERT_EQUALS( expected, g.getNeighbours( 13 ) );
    }

    /**
     * The correct voxel numbers should be returned in a rotated grid.
     */
    void testRotatedVoxelNum()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );
        y *= 2.0;
        z *= 1.5;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];
        mat( 0, 3 ) = 1.0;

        WGridTransformOrtho t( mat );
        WGridRegular3D g( 5, 5, 5, t );

        WVector3d v = WVector3d( 1.0, 0.0, 0.0 ) + 0.3 * z + 2.4 * y + 2.9 * x;

        TS_ASSERT_EQUALS( g.getXVoxelCoord( v ), 3 );
        TS_ASSERT_EQUALS( g.getYVoxelCoord( v ), 2 );
        TS_ASSERT_EQUALS( g.getZVoxelCoord( v ), 0 );
    }

    /**
     * Positions outside of a rotated grid should return voxel positions of -1.
     */
    void testRotatedVoxelOutOfGrid()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );
        y *= 2.0;
        z *= 1.5;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];
        mat( 0, 3 ) = 1.0;

        WGridTransformOrtho t( mat );
        WGridRegular3D g( 5, 5, 5, t );

        WVector3d v( 1.0, 0.0, 0.0 );
        v -= wlimits::FLT_EPS * x;

        TS_ASSERT_EQUALS( g.getXVoxelCoord( v ), -1 );
        TS_ASSERT_DIFFERS( g.getYVoxelCoord( v ), -1 );
        TS_ASSERT_DIFFERS( g.getZVoxelCoord( v ), -1 );

        v -= wlimits::FLT_EPS * z;

        TS_ASSERT_EQUALS( g.getXVoxelCoord( v ), -1 );
        TS_ASSERT_DIFFERS( g.getYVoxelCoord( v ), -1 );
        TS_ASSERT_EQUALS( g.getZVoxelCoord( v ), -1 );

        v = WVector3d( 1.0, 0.0, 0.0 ) + ( 4.0 + wlimits::FLT_EPS ) * y;

        TS_ASSERT_DIFFERS( g.getXVoxelCoord( v ), -1 );
        TS_ASSERT_EQUALS( g.getYVoxelCoord( v ), -1 );
        TS_ASSERT_DIFFERS( g.getZVoxelCoord( v ), -1 );
    }

    /**
     * A voxel with voxel-coordinates 0,0,0 has only three neighbours: 1,0,0; 0,1,0 and 0,0,1.
     */
    void testNeighboursOnFrontLowerLeft( void )
    {
        WGridRegular3D g( 3, 3, 3 );
        size_t data[] = { 1, 3, 9 };
        std::vector< size_t > expected( data, data + 3 );
        TS_ASSERT_EQUALS( expected, g.getNeighbours( 0 ) );
    }

    /**
     * A voxel in the back upper right corner should also have only 3 neighbours.
     */
    void testNeighbourOnBackUpperRight( void )
    {
        WGridRegular3D g( 3, 3, 3 );
        size_t data[] = { 25, 23, 17 };
        std::vector< size_t > expected( data, data + 3 );
        TS_ASSERT_EQUALS( expected, g.getNeighbours( 26 ) );
    }

    /**
     * A Voxel on a border plane should have neighbours on the plane but not
     * out side the grid.
     */
    void testNeighbourOnLeftBorderPlane( void )
    {
        WGridRegular3D g( 3, 3, 3 );
        size_t data[] = { 13, 9, 15, 3, 21 };
        std::vector< size_t > expected( data, data + 5 );
        TS_ASSERT_EQUALS( expected, g.getNeighbours( 12 ) );
    }

    /**
     * If the neighbours of a voxel not inside this grid are requested an Exception
     * WOutOfBounds should be thrown.
     */
    void testNeighbourOfVoxelNotInsideThisGrid( void )
    {
        WGridRegular3D g( 3, 3, 3 );
        TS_ASSERT_THROWS_EQUALS( g.getNeighbours( 27 ), const WOutOfBounds &e, std::string( e.what() ),
                "This point: 27 is not part of this grid:  nbPosX: 3 nbPosY: 3 nbPosZ: 3" );
    }

    /**
     * Check whether we get the right Ids.
     */
    void testGetCellVertexIds( void )
    {
        WGridRegular3D g( 5, 3, 3 );
        WGridRegular3D::CellVertexArray expected = { { 23, 24, 28, 29, 38, 39, 43, 44 } };
        TS_ASSERT_EQUALS( g.getCellVertexIds( 15 ), expected );
    }

    /**
     * Check whether we get the right cellId.
     */
    void testGetCellId( void )
    {
        WGridRegular3D g( 5, 3, 3 );
        bool isInside = true;

        // Test some value
        size_t cellId = g.getCellId( WPosition( 3.3, 1.75, 0.78 ), &isInside );
        TS_ASSERT_EQUALS( cellId, 7 );
        TS_ASSERT_EQUALS( isInside, true );

        // Test bounds for X direction
        cellId = g.getCellId( WPosition( 4.0, 1.75, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );

        cellId = g.getCellId( WPosition( 4.0 - wlimits::FLT_EPS, 1.75, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 0.0, 1.75, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 0.0 - wlimits::FLT_EPS, 1.75, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );

        // Test bounds for Y direction
        cellId = g.getCellId( WPosition( 3.3, 2.0, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );

        cellId = g.getCellId( WPosition( 3.3, 2.0 - wlimits::FLT_EPS, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 3.3, 0.0, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 3.3, 0.0 - wlimits::FLT_EPS, 0.3 ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );

        // Test bounds for Z direction
        cellId = g.getCellId( WPosition( 3.3, 1.75, 2.0 ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );

        cellId = g.getCellId( WPosition( 3.3, 1.75, 2.0 - wlimits::FLT_EPS ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 3.3, 1.75, 0.0 ), &isInside );
        TS_ASSERT_EQUALS( isInside, true );

        cellId = g.getCellId( WPosition( 3.3, 1.75, 0.0 - wlimits::FLT_EPS ), &isInside );
        TS_ASSERT_EQUALS( isInside, false );
    }

    /**
     * If a point is inside of the boundary of a grid encloses should return true, otherwise false.
     */
    void testEnclosesQuery( void )
    {
        WGridRegular3D g( 2, 2, 2 );

        // Test bounds for X direction
        TS_ASSERT( !g.encloses( WPosition( 0 - wlimits::FLT_EPS, 0, 0 ) ) );
        TS_ASSERT( g.encloses( WPosition( 0, 0, 0 ) ) );
        TS_ASSERT( g.encloses( WPosition( 1.0 - wlimits::FLT_EPS, 0.5, 0.5 ) ) );
        TS_ASSERT( !g.encloses( WPosition( 1, 0.5, 0.5 ) ) );

        // Test bounds for Y direction
        TS_ASSERT( !g.encloses( WPosition( 0, 0 - wlimits::FLT_EPS, 0 ) ) );
        TS_ASSERT( g.encloses( WPosition( 0, 0, 0 ) ) );
        TS_ASSERT( g.encloses( WPosition( 0.5, 1.0 - wlimits::FLT_EPS, 0.5 ) ) );
        TS_ASSERT( !g.encloses( WPosition( 0.5, 1.0, 0.5 ) ) );

        // Test bounds for Z direction
        TS_ASSERT( !g.encloses( WPosition( 0, 0, 0 - wlimits::FLT_EPS ) ) );
        TS_ASSERT( g.encloses( WPosition( 0, 0, 0 ) ) );
        TS_ASSERT( g.encloses( WPosition( 0.5, 0.5, 1.0 - wlimits::FLT_EPS ) ) );
        TS_ASSERT( !g.encloses( WPosition( 0.5, 0.5, 1 ) ) );
    }

    /**
     * If a point is inside of the boundary of a grid encloses should return true, otherwise false.
     */
    void testEnclosesRotated()
    {
        WVector3d x( 0.707, 0.707, 0.0 );
        WVector3d y( -0.707, 0.707, 0.0 );
        WVector3d z( 0.0, 0.0, 1.0 );
        x = normalize( x );
        y = normalize( y );
        y *= 2.0;
        z *= 1.5;

        WMatrix< double > mat( 4, 4 );
        mat.makeIdentity();
        mat( 0, 0 ) = x[ 0 ];
        mat( 1, 0 ) = x[ 1 ];
        mat( 2, 0 ) = x[ 2 ];
        mat( 0, 1 ) = y[ 0 ];
        mat( 1, 1 ) = y[ 1 ];
        mat( 2, 1 ) = y[ 2 ];
        mat( 0, 2 ) = z[ 0 ];
        mat( 1, 2 ) = z[ 1 ];
        mat( 2, 2 ) = z[ 2 ];
        mat( 0, 3 ) = 1.0;

        WGridTransformOrtho t( mat );
        WGridRegular3D g( 5, 5, 5, t );

        WVector3d o = WVector3d( 1.0, 0.0, 0.0 ) + ( x + y + z ) * 2.0 * wlimits::FLT_EPS;
        WVector3d v = o - 4.0 * wlimits::FLT_EPS * x;
        TS_ASSERT( !g.encloses( v ) );
        v = o;
        TS_ASSERT( g.encloses( v ) );
        v = o + ( 4.0 - 4.0 * wlimits::FLT_EPS ) * x;
        TS_ASSERT( g.encloses( v ) );
        v += 4.0 *  wlimits::FLT_EPS * x;
        TS_ASSERT( !g.encloses( v ) );

        v = o - 4.0 * wlimits::FLT_EPS * y;
        TS_ASSERT( !g.encloses( v ) );
        v = o + ( 4.0 - 4.0 * wlimits::FLT_EPS ) * y;
        TS_ASSERT( g.encloses( v ) );
        v += 4.0 * wlimits::FLT_EPS * y;
        TS_ASSERT( !g.encloses( v ) );

        v = o - 4.0 * wlimits::FLT_EPS * z;
        TS_ASSERT( !g.encloses( v ) );
        v = o + ( 4.0 - 4.0 * wlimits::FLT_EPS ) * z;
        TS_ASSERT( g.encloses( v ) );
        v += 4.0 * wlimits::FLT_EPS * z;
        TS_ASSERT( !g.encloses( v ) );
    }

private:
    double m_delta; //!< Maximum amount to values are allowed to differ.
};

#endif  // WGRIDREGULAR3D_TEST_H
