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

#ifndef WVECTOR3D_TEST_H
#define WVECTOR3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "../WVector3D.h"

class WVector3DTest : public CxxTest::TestSuite
{
public:
    /**
     * Instatiation should throw nothing.
     */
    void testInstantiationStandard( void )
    {
        TS_ASSERT_THROWS_NOTHING( WVector3D vec() );
    }

    /**
     * Instatiation should throw nothing.
     */
    void testInstantiationWithDoubles( void )
    {
        TS_ASSERT_THROWS_NOTHING( WVector3D vec( 32.32, 42.42, 23.23 ) );
    }

    /**
     * Instatiation should throw nothing.
     */
    void testInstantiationWithCopyConstructor( void )
    {
        WVector3D vecToCopy;
        TS_ASSERT_THROWS_NOTHING( WVector3D vec( vecToCopy ) );
    }

    /**
     * Standard constructor should set values to zero
     */
    void testInitializationWithStandardConstructor( void )
    {
        WVector3D vec;
        TS_ASSERT_EQUALS( vec.m_components[0], 0. );
        TS_ASSERT_EQUALS( vec.m_components[1], 0. );
        TS_ASSERT_EQUALS( vec.m_components[2], 0. );
    }

    /**
     * Double constructor should set values to the provided params
     */
    void testInitializationWithDoubleConstructor( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec.m_components[0], a );
        TS_ASSERT_EQUALS( vec.m_components[1], b );
        TS_ASSERT_EQUALS( vec.m_components[2], c );
    }

    /**
     * Copy constructor should copy values
     */
    void testInitializationWithCopyConstructor( void )
    {
        WVector3D vecToCopy( 32.32, 42.42, 23.23 );
        WVector3D vec( vecToCopy );
        TS_ASSERT_EQUALS( vec.m_components[0], vecToCopy.m_components[0] );
        TS_ASSERT_EQUALS( vec.m_components[1], vecToCopy.m_components[1] );
        TS_ASSERT_EQUALS( vec.m_components[2], vecToCopy.m_components[2] );
    }

    /**
     * Element access operator should return the right values
     */
    void testElementAccessOperator( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec[0], a );
        TS_ASSERT_EQUALS( vec[1], b );
        TS_ASSERT_EQUALS( vec[2], c );
    }

    /**
     * Const element access operator should return the right values
     */
    void testConstElementAccessOperator( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        const WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec[0], a );
        TS_ASSERT_EQUALS( vec[1], b );
        TS_ASSERT_EQUALS( vec[2], c );
    }
};

#endif  // WVECTOR3D_TEST_H
