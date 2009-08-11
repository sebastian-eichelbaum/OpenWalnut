//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#include <cxxtest/TestSuite.h>

#include "../WGridRegular3D.h"
#include "../../math/WVector3D.h"

/**
 * Tests the WGridRegular3D class.
 */
class WGridRegular3DTest : public CxxTest::TestSuite
{
public:
    /**
     * Ensure that nothing is thrown when an instance is created.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 3, 3, 3, 1., 1., 1. ) );
        TS_ASSERT_THROWS_NOTHING( WGridRegular3D grid( 0., 0., 0., 3, 3, 3, 1., 1., 1. ) );
    }

    /**
     * After instantiation there should be the requested number of positions.
     */
    void testSize( void )
    {
        WGridRegular3D grid( 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.size(), 27 );

        WGridRegular3D grid2( 0., 0., 0., 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid2.size(), 27 );
    }

    /**
     * getNbCoords should return the samples prescribed by the use of the constructor
     */
    void testGetNbCoords( void )
    {
        size_t x = 3;
        size_t y = 4;
        size_t z = 5;
        WGridRegular3D grid( x, y, z, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.getNbCoordsX(), x );
        TS_ASSERT_EQUALS( grid.getNbCoordsY(), y );
        TS_ASSERT_EQUALS( grid.getNbCoordsZ(), z );
    }

    /**
     * getOffset should return the offsets prescribed by the use of the constructor
     */
    void testGetOffset( void )
    {
        double x = 1.2;
        double y = 3.4;
        double z = 5.6;
        WGridRegular3D grid( 10., 10., 10., x, y, z );
        TS_ASSERT_EQUALS( grid.getOffsetX(), x );
        TS_ASSERT_EQUALS( grid.getOffsetY(), y );
        TS_ASSERT_EQUALS( grid.getOffsetZ(), z );
    }

    /**
     * getOrigin should return the origin prescribed by the use of the constructor
     * or (0,0,0) when using the second constructor.
     */
    void testGetOrigin( void )
    {
        WVector3D zeroOrigin( 0., 0., 0. );
        WGridRegular3D grid( 10., 10., 10., 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.getOrigin(), zeroOrigin );

        WVector3D origin( 1.2, 3.4, 5.6 );
        WGridRegular3D grid2( origin[0], origin[1], origin[2], 10., 10., 10., 1., 1., 1. );
        TS_ASSERT_EQUALS( grid2.getOrigin(), origin );
    }
};

#endif  // WGRIDREGULAR3D_TEST_H
