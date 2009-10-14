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

#ifndef WPROPERTIES_TEST_H
#define WPROPERTIES_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WProperties.h"

/**
 * unit tests the properties class
 */
class WPropertiesTest : public CxxTest::TestSuite
{
public:
    /**
     * tests creating an integer property
     */
    void testAddProp( void )
    {
        WProperties props;
        props.addInt( "test", 57, "test57", "longDesc57" );
        TS_ASSERT_EQUALS( props.getValue<int>( "test" ), 57 );
    }

    /**
     * tests creating an integer property and changing its value
     */
    void testChangeProp( void )
    {
        WProperties props;
        props.addInt( "test", 57, "test57", "longDesc57" );
        props.setValue( "test", 61 );
        TS_ASSERT_EQUALS( props.getValue<int>( "test" ), 61 );
    }
};

#endif  // WPROPERTIES_TEST_H
