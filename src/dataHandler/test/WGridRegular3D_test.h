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
     * After instantiation there should be no positions.
     */
    void testSize( void )
    {
        WGridRegular3D grid( 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid.size(), 27 );

        WGridRegular3D grid2( 0., 0., 0., 3, 3, 3, 1., 1., 1. );
        TS_ASSERT_EQUALS( grid2.size(), 27 );
    }
};

#endif  // WGRIDREGULAR3D_TEST_H
