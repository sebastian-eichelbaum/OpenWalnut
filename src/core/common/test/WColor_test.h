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

#ifndef WCOLOR_TEST_H
#define WCOLOR_TEST_H

#include <sstream>
#include <string>

#include <cxxtest/TestSuite.h>

#include "../WColor.h"
#include "WColorTraits.h"

/**
 * Unit tests the color helping functions
 */
class WColorTest : public CxxTest::TestSuite
{
public:

    /**
     * Red in HSV is ( 0, 1, 1 ) and in RGB ( 1, 0, 0 )
     * Green in HSV is ( 0.3, 1, 1 ) and in RGB ( 0, 1, 0 )
     * and checks some dark green
     */
    void testHSVConversion( void )
    {
        WColor c = convertHSVtoRGBA( 0, 1, 1 );
        TS_ASSERT_DELTA( c[0], 1, 0.00001 );
        TS_ASSERT_DELTA( c[1], 0, 0.00001 );
        TS_ASSERT_DELTA( c[2], 0, 0.00001 );
        c = convertHSVtoRGBA( 1, 1, 1 ); // this is also red
        TS_ASSERT_DELTA( c[0], 1, 0.00001 );
        TS_ASSERT_DELTA( c[1], 0, 0.00001 );
        TS_ASSERT_DELTA( c[2], 0, 0.00001 );
        c = convertHSVtoRGBA( 1.0 / 3.0, 1, 1 );
        TS_ASSERT_DELTA( c[0], 0, 0.00001 );
        TS_ASSERT_DELTA( c[1], 1, 0.00001 );
        TS_ASSERT_DELTA( c[2], 0, 0.00001 );
        c = convertHSVtoRGBA( 0.3, 0.3, 0.3 ); // dark green
        TS_ASSERT_DELTA( c[0], 0.2280, 0.0001 );
        TS_ASSERT_DELTA( c[1], 0.3, 0.0001 );
        TS_ASSERT_DELTA( c[2], 0.2099, 0.0001 );
    }
};

#endif  // WCOLOR_TEST_H
