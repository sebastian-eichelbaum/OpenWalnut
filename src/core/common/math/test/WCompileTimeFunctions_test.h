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

#ifndef WCOMPILETIMEFUNCTIONS_TEST_H
#define WCOMPILETIMEFUNCTIONS_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>
#include "../WCompileTimeFunctions.h"

/**
 * Test class for WBinom and WPower template.
 */
class WCompileTimeFunctionsTest : public CxxTest::TestSuite
{
public:
    /**
     * Test WBinom.
     */
    void testWBinom()
    {
        std::size_t value;
        value = WBinom< 1, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 1, 1 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 2, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 2, 1 >::value;
        TS_ASSERT_EQUALS( value, 2 );
        value = WBinom< 2, 2 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 3, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 3, 1 >::value;
        TS_ASSERT_EQUALS( value, 3 );
        value = WBinom< 3, 2 >::value;
        TS_ASSERT_EQUALS( value, 3 );
        value = WBinom< 3, 3 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 4, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 4, 1 >::value;
        TS_ASSERT_EQUALS( value, 4 );
        value = WBinom< 4, 2 >::value;
        TS_ASSERT_EQUALS( value, 6 );
        value = WBinom< 4, 3 >::value;
        TS_ASSERT_EQUALS( value, 4 );
        value = WBinom< 4, 4 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 5, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 5, 1 >::value;
        TS_ASSERT_EQUALS( value, 5 );
        value = WBinom< 5, 2 >::value;
        TS_ASSERT_EQUALS( value, 10 );
        value = WBinom< 5, 3 >::value;
        TS_ASSERT_EQUALS( value, 10 );
        value = WBinom< 5, 4 >::value;
        TS_ASSERT_EQUALS( value, 5 );
        value = WBinom< 5, 5 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 6, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WBinom< 6, 1 >::value;
        TS_ASSERT_EQUALS( value, 6 );
        value = WBinom< 6, 2 >::value;
        TS_ASSERT_EQUALS( value, 15 );
        value = WBinom< 6, 3 >::value;
        TS_ASSERT_EQUALS( value, 20 );
        value = WBinom< 6, 4 >::value;
        TS_ASSERT_EQUALS( value, 15 );
        value = WBinom< 6, 5 >::value;
        TS_ASSERT_EQUALS( value, 6 );
        value = WBinom< 6, 6 >::value;
        TS_ASSERT_EQUALS( value, 1 );
    }

    /**
     * Test WPower.
     */
    void testWPower()
    {
        std::size_t value;
        value = WPower< 1, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 2, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 3, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 4, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 5, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 6, 0 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 1, 1 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 2, 1 >::value;
        TS_ASSERT_EQUALS( value, 2 );
        value = WPower< 3, 1 >::value;
        TS_ASSERT_EQUALS( value, 3 );
        value = WPower< 4, 1 >::value;
        TS_ASSERT_EQUALS( value, 4 );
        value = WPower< 5, 1 >::value;
        TS_ASSERT_EQUALS( value, 5 );
        value = WPower< 0, 2 >::value;
        TS_ASSERT_EQUALS( value, 0 );
        value = WPower< 1, 2 >::value;
        TS_ASSERT_EQUALS( value, 1 );
        value = WPower< 2, 2 >::value;
        TS_ASSERT_EQUALS( value, 4 );
        value = WPower< 3, 2 >::value;
        TS_ASSERT_EQUALS( value, 9 );
        value = WPower< 4, 2 >::value;
        TS_ASSERT_EQUALS( value, 16 );
        value = WPower< 5, 2 >::value;
        TS_ASSERT_EQUALS( value, 25 );
        value = WPower< 6, 2 >::value;
        TS_ASSERT_EQUALS( value, 36 );
        value = WPower< 3, 3 >::value;
        TS_ASSERT_EQUALS( value, 27 );
        value = WPower< 4, 3 >::value;
        TS_ASSERT_EQUALS( value, 64 );
        value = WPower< 5, 3 >::value;
        TS_ASSERT_EQUALS( value, 125 );
        value = WPower< 2, 4 >::value;
        TS_ASSERT_EQUALS( value, 16 );
        value = WPower< 3, 4 >::value;
        TS_ASSERT_EQUALS( value, 81 );
        value = WPower< 2, 5 >::value;
        TS_ASSERT_EQUALS( value, 32 );
        value = WPower< 3, 5 >::value;
        TS_ASSERT_EQUALS( value, 243 );
    }
};

#endif  // WCOMPILETIMEFUNCTIONS_TEST_H
