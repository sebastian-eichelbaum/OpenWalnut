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

#include <string>
#include <sstream>

#include <cxxtest/TestSuite.h>

#include "../WColor.h"

/**
 * Unit tests the WColor class
 */
class WColorTest : public CxxTest::TestSuite
{
public:
    /**
     * To write a WColor object to an output stream we must ensure that every
     * component (red, green, blue and alpha) are written to that stream and 
     * that we have a special delimiter char.
     */
    void testOutputOperator( void )
    {
        std::stringstream ss;
        WColor c;
        c.setBlue( 0.4711 );
        ss << c;
        std::string expected = "0;0;0.4711;1";
        TS_ASSERT_EQUALS( ss.str(), expected );
    }

    /**
     * When given a string separated by semicolons then it should be handled
     * as red green blue and alpha values. There has to be 4 such values.
     */
    void testInputOperator( void )
    {
        WColor c( 1.0, 0.5, 0.25 );
        std::stringstream ss( "1.0;0.5;0.25;0.0" );
        ss >> c;
        TS_ASSERT_EQUALS( c.getRed(), 1.0 );
        TS_ASSERT_EQUALS( c.getGreen(), 0.5 );
        TS_ASSERT_EQUALS( c.getBlue(), 0.25 );
        TS_ASSERT_EQUALS( c.getAlpha(), 0.0 );
    }
};

#endif  // WCOLOR_TEST_H
