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

#ifndef WGRID_TEST_H
#define WGRID_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WGrid.h"

/**
 * Tests the WGrid class.
 */
class WGridTest : public CxxTest::TestSuite
{
public:
    /**
     * Ensure that nothing is thrown when an instance is created.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WGrid grid() );
    }

    /**
     * After instantiation there should be no positions.
     */
    void testGetNumberOfPositions( void )
    {
        WGrid grid;
        TS_ASSERT_EQUALS( grid.getNumberOfPositions(), 0 );
    }
};

#endif  // WGRID_TEST_H
