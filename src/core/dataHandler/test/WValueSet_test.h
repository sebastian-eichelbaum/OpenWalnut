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

#ifndef WVALUESET_TEST_H
#define WVALUESET_TEST_H

#include <stdint.h>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WValueSet.h"
#include "../WDataHandlerEnums.h"

/**
 * UnitTests the WValueSet class
 */
class WValueSetTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception
     */
    void testInstantiation( void )
    {
        double a[2] = { 0.0, 3.1415 };
        const boost::shared_ptr< std::vector< double > > v =
            boost::shared_ptr< std::vector< double > >(
                new std::vector< double >( a, a + sizeof( a ) / sizeof( double ) ) );
        TS_ASSERT_THROWS_NOTHING( WValueSet< double > valueSet( 0, 1, v, W_DT_DOUBLE ) );
    }

    /**
     * The number of values retrieved is correct
     */
    void testGetNumberOfValues( void )
    {
        int a[4] = { 0, -5, 1, 2 };
        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int ) ) );
        WValueSet< int8_t > first( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.size(), 4 );
        WValueSet< int8_t > second( 1, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( second.size(), 2 );
        WValueSet< int8_t > third( 2, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( third.size(), 1 );
    }

    /**
     * The raw size is the size of the number of integral elements inside
     * this ValueSet.
     */
    void testRawSize( void )
    {
        int8_t a[4] = { 0, -5, 1, 2 };
        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int8_t ) ) );
        WValueSet< int8_t > first( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.rawSize(), 4 );
        WValueSet< int8_t > second( 2, 2, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( first.rawSize(), 4 );
    }

    /**
     * This function should return the i-th value if the value set is scalar.
     */
    void testGetScalar( void )
    {
        int8_t a[4] = { 0, -5, 1, 2 };
        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int8_t ) ) );
        WValueSet< int8_t > set( 0, 1, v, W_DT_INT8 );
        TS_ASSERT_EQUALS( set.getScalar( 0 ), a[0] );
        TS_ASSERT_EQUALS( set.getScalar( 1 ), a[1] );
        TS_ASSERT_EQUALS( set.getScalar( 2 ), a[2] );
        TS_ASSERT_EQUALS( set.getScalar( 3 ), a[3] );
    }

    /**
     * Raw Access should provide data access to the underlying array.
     */
    void testReadOnlyRawAccess( void )
    {
        double a[2] = { 0.0, 3.1415 };
        const boost::shared_ptr< std::vector< double > > v =
            boost::shared_ptr< std::vector< double > >(
                new std::vector< double >( a, a + sizeof( a ) / sizeof( double ) ) );
        WValueSet< double > valueSet( 0, 1, v, W_DT_DOUBLE );
        const double * const b = valueSet.rawData();
        TS_ASSERT_EQUALS( b[0], 0.0 );
        TS_ASSERT_EQUALS( b[1], 3.1415 );
    }

    /**
     * This function should return the i-th WValue with of the used dimension (prerequisite the ValueSet has order 1)
     */
    void testGetWValue( void )
    {
        int8_t a[6] = { 1, 2, 3, 4, 5, 6 };
        std::size_t dim = 2;

        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int8_t ) ) );
        WValueSet< int8_t > set( 1, dim, v, W_DT_INT8 );

        // testing for valid dimension and values of the returned WValue
        for( std::size_t idx = 0; idx < v->size()/dim; idx++ )
        {
          WValue< int8_t > currentWValue( dim );
          for( std::size_t i = 0; i < dim; i++ ) currentWValue[ i ] = ( *v )[ idx*dim + i ];
          TS_ASSERT_EQUALS( set.getWValue( idx ), currentWValue );
          TS_ASSERT_EQUALS( set.getWValue( idx ).size(), dim );
        }

        // catch wrong indices?
        TS_ASSERT_THROWS_ANYTHING( set.getWValue( v->size() ) );
        TS_ASSERT_THROWS_ANYTHING( set.getWValue( v->size()*2 ) );

        // catch wrong order?
        WValueSet< int8_t > set2( 2, dim, v, W_DT_INT8 );
        TS_ASSERT_THROWS_ANYTHING( set2.getWValue( 0 ) );
    }

    /**
     * A subarray should never exceed the valuesets boundaries and should not have a length of 0.
     */
    void testSubArrayInstantiation()
    {
        int8_t a[4] = { 0, -5, 1, 2 };
        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new  std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int8_t ) ) );
        WValueSet< int8_t > set( 1, 2, v, W_DT_INT8 );
        TS_ASSERT_THROWS_NOTHING( set.getSubArray( 0, 2 ) );
        TS_ASSERT_THROWS_NOTHING( set.getSubArray( 3, 1 ) );
        TS_ASSERT_THROWS( set.getSubArray( 4, 1 ), WException );
        TS_ASSERT_THROWS( set.getSubArray( 3, 2 ), WException );
        TS_ASSERT_THROWS( set.getSubArray( 2, 0 ), WException );
    }

    /**
     * A subarray should return the correct elements.
     */
    void testSubArrayAccess()
    {
        int8_t a[ 8 ] = { 0, -5, 1, 2, -27, 6, 29, 8 };
        const boost::shared_ptr< std::vector< int8_t > > v =
            boost::shared_ptr< std::vector< int8_t > >(
                new std::vector< int8_t >( a, a + sizeof( a ) / sizeof( int8_t ) ) );
        WValueSet< int8_t > set( 1, 2, v, W_DT_INT8 );

        {
            WValueSet< int8_t >::SubArray const s = set.getSubArray( 0, 2 );

            TS_ASSERT_THROWS_NOTHING( s[ 0 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 1 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 2 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 100 ] );

            TS_ASSERT_EQUALS( s[ 0 ], 0 );
            TS_ASSERT_EQUALS( s[ 1 ], -5 );
            TS_ASSERT_EQUALS( s[ 2 ], 0 );
            TS_ASSERT_EQUALS( s[ 100 ], 0 );
        }
        {
            WValueSet< int8_t >::SubArray const s = set.getSubArray( 1, 3 );

            TS_ASSERT_THROWS_NOTHING( s[ 0 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 1 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 2 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 100 ] );

            TS_ASSERT_EQUALS( s[ 0 ], -5 );
            TS_ASSERT_EQUALS( s[ 1 ], 1 );
            TS_ASSERT_EQUALS( s[ 2 ], 2 );
            TS_ASSERT_EQUALS( s[ 100 ], -5 );
        }
        {
            WValueSet< int8_t >::SubArray const s = set.getSubArray( 5, 3 );

            TS_ASSERT_THROWS_NOTHING( s[ 0 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 1 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 2 ] );
            TS_ASSERT_THROWS_NOTHING( s[ 100 ] );

            TS_ASSERT_EQUALS( s[ 0 ], 6 );
            TS_ASSERT_EQUALS( s[ 1 ], 29 );
            TS_ASSERT_EQUALS( s[ 2 ], 8 );
            TS_ASSERT_EQUALS( s[ 100 ], 6 );
        }
    }
};

#endif  // WVALUESET_TEST_H
