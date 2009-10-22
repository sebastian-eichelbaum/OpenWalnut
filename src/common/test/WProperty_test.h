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

#ifndef WPROPERTY_TEST_H
#define WPROPERTY_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WProperty.h"

/**
 * unit tests the property class
 */
class WPropertyTest : public CxxTest::TestSuite
{
public:
    /**
     * Tests the property class by creating a boolean property
     */
    void testBool( void )
    {
        WProperty prop( "testProp", true );
        TS_ASSERT_EQUALS( prop.getValue<bool>(), true );
    }

    void testBoolSetFalse( void )
    {
        WProperty prop( "testProp", true );
        prop.setValue( false );
        TS_ASSERT_EQUALS( prop.getValue<bool>(), false );
    }

    void testBoolGetString( void )
    {
        WProperty prop( "testProp", true );
        TS_ASSERT_EQUALS( prop.getValueString(), "1" );
    }
};

#endif  // WPROPERTY_TEST_H
