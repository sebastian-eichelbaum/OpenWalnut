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

#ifndef WVALUE_TEST_H
#define WVALUE_TEST_H

#include <cxxtest/TestSuite.h>
#include "../WValue.hpp"

class WValueTest : public CxxTest::TestSuite
{
public:
    /**
     * Instatiation should throw nothing.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WValue< double > value( 3 ) );
        TS_ASSERT_THROWS_NOTHING( WValue< float > value( 3 ) );
    }

    /**
     * getNbComponents should give the value we hav put into the constructor
     */
    void testGetNbComponents( void )
    {
        const size_t size = 3;
        WValue< double > value( size );
        TS_ASSERT_EQUALS( value.getNbComponents(), size );
    }

    /**
     * Element access operator should return the right values
     */
    void testElementAccessOperator( void )
    {
        const size_t size = 3;
        WValue< double > value( size );
        TS_ASSERT_EQUALS( value[0], 0. );
        TS_ASSERT_EQUALS( value[1], 0. );
        TS_ASSERT_EQUALS( value[2], 0. );

        const double a = 3.14;
        value[1] = a;
        TS_ASSERT_EQUALS( value[1], a );
    }

    /**
     * Const element access operator should return the right values
     */
    void testConstElementAccessOperator( void )
    {
        const size_t size = 3;
        const WValue< double > value( size );
        TS_ASSERT_EQUALS( value[0], 0. );
        TS_ASSERT_EQUALS( value[1], 0. );
        TS_ASSERT_EQUALS( value[2], 0. );
    }
};

#endif  // WVALUE_TEST_H
