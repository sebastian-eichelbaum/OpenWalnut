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

#ifndef WIOTOOLS_TEST_H
#define WIOTOOLS_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WIOTools.h"

/**
 * Unit test helper functions in the wiotools namespace
 */
class WIOToolsTest : public CxxTest::TestSuite
{
public:
    /**
     * When switching byte order, the first and last bytes should be swapped
     * and the bytes inbetween too.
     */
    void testByteOrderSwitching( void )
    {
        uint32_t x = 1;
        TS_ASSERT_EQUALS( x, 1 );
        x = wiotools::switchByteOrder( x );
        TS_ASSERT_EQUALS( x, 16777216 );
        x = wiotools::switchByteOrder( x );
        TS_ASSERT_EQUALS( x, 1 );
    }

    /**
     * Since templates should work on multiple types we just use double here.
     */
    void testByteOrderSwitchingOnFloats( void )
    {
        double x = 3.141592653589793238462643383279502884197169399375105820974;
        double original = x;
        x = wiotools::switchByteOrder( x );
        TS_ASSERT_DIFFERS( x, original );
        x = wiotools::switchByteOrder( x );
        TS_ASSERT_EQUALS( x, original );
    }

    /**
     * On single bytes we should do nothing.
     */
    void testByteOrderSwitchingOnSingleBytes( void )
    {
        char x = 1;
        TS_ASSERT_EQUALS( wiotools::switchByteOrder( x ), x );
    }

    /**
     * When switching the byte order of an whole array every element should be
     * switched.
     */
    void testByteOrderSwitchOnArray( void )
    {
        uint32_t x[] = { 1, 16777216 };
        wiotools::switchByteOrderOfArray( x, 2 );
        TS_ASSERT_EQUALS( x[0], 16777216 );
        TS_ASSERT_EQUALS( x[1], 1 );
    }
};

#endif  // WIOTOOLS_TEST_H
