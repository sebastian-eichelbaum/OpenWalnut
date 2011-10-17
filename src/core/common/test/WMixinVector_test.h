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

#ifndef WMIXINVECTOR_TEST_H
#define WMIXINVECTOR_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WMixinVector.h"
#include "WMixinVectorTraits.h"

// \cond Suppress_Doxygen
// This is just a dummy class to test if the default constructor is called when
// not specified by WMixinVector instanziation.
class A {public: A(){m_x = 3.1415;} bool operator==(double x){return m_x == x;} double m_x; }; // NOLINT
// \endcond

/**
 * Unit tests the WMixinVector copy from OSG
 * \warning THIS IS FAR AWAY FROM BEING A COMPLETE UNIT TEST SUIT FOR WMIXINVECTOR!!!
 */
class WMixinVectorTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the default Ctor
     */
    void testDefaultCtor( void )
    {
        TS_ASSERT_THROWS_NOTHING( WMixinVector< double >() );
    }

    /**
     * You may initialize a vector with a given size and optionally with
     * a default value of the elments.
     */
    void testSizeValueCtor( void )
    {
        WMixinVector< std::string > stringV( 2, "bla" );
        TS_ASSERT( stringV.size() == 2 );
        TS_ASSERT_EQUALS( stringV[0], "bla" );
        TS_ASSERT_EQUALS( stringV[1], "bla" );
        WMixinVector< A > aV( 5 );
        TS_ASSERT_EQUALS( aV.size(), 5 );
        for( size_t i = 0; i < 5; ++i )
        {
            TS_ASSERT_EQUALS( aV[i], 3.1415 );
        }
    }

    /**
     * If you have another WMixinVector a copy construction should be possible
     */
    void testCopyCtorOnWMixinVector( void )
    {
        WMixinVector< int > intV( 4, -1 );
        WMixinVector< int > intV2( intV );
        TS_ASSERT_EQUALS( intV, intV2 );
        intV[0] = 0;
        TS_ASSERT_DIFFERS( intV, intV2 );
    }

    /**
     * If you have a std::vector< T > copy construction should still be possible.
     */
    void testCopyCtorOnRealSTDVector( void )
    {
        std::vector< char > charV( 5, 's' );
        WMixinVector< char > charV2( charV );
        TS_ASSERT_EQUALS( charV, charV2 );
        charV[0] = 'a';
        TS_ASSERT_DIFFERS( charV, charV2 );
    }

    /**
     * A creation should also be possible out of iterators
     */
    void testIteratorConstructor( void )
    {
         int myints[] = { 16, 2, 77, 29 }; // NOLINT
         WMixinVector< int > v( myints, myints + sizeof( myints ) / sizeof( int ) );
         TS_ASSERT_EQUALS( v.size(), 4 );
         TS_ASSERT_EQUALS( v[0], 16 );
         TS_ASSERT_EQUALS( v[1], 2 );
         TS_ASSERT_EQUALS( v[2], 77 );
         TS_ASSERT_EQUALS( v[3], 29 );
    }
};

#endif  // WMIXINVECTOR_TEST_H
