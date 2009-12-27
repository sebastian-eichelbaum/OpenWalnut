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

#ifndef WCOLORUTILS_TEST_H
#define WCOLORUTILS_TEST_H

#include <cxxtest/TestSuite.h>

#include "../../common/WColor.h"
#include "../../common/test/WColorTraits.h"
#include "../WColorUtils.h"

/**
 * Unit tests the functions in color_utils namespace
 */
class WColorUtilsTest : public CxxTest::TestSuite
{
public:
    /**
     * The color of two positions is mapped as follows to an RGB color:
     * absolute value of normalized x-axis distance to red
     * absolute value of normalized y-axis distance to green
     * absolute value of normalized z-axis distance to blue
     */
    void testColorFromDirection( void )
    {
        wmath::WPosition zero( 0, 0, 0 );
        wmath::WPosition one( 1, 1, -1 );
        WColor result = color_utils::getRGBAColorFromDirection( zero, one );
        WColor expected( 1.0 / sqrt( 3 ), 1.0 / sqrt( 3 ), 1.0 / sqrt( 3 ) );
        TS_ASSERT_EQUALS( result, expected );
        result = color_utils::getRGBAColorFromDirection( one, zero );
        TS_ASSERT_EQUALS( result, expected );
    }
};

#endif  // WCOLORUTILS_TEST_H
