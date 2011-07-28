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

#ifndef WTENSORBASE_TEST_H
#define WTENSORBASE_TEST_H

// unset the WASSERT_AS_CASSERT flag (just in case), so WAssert throws a WException
// note that this may be overwritten by defines in WTensorBase.h
// or any files included from there
#ifdef WASSERT_AS_CASSERT
#define WASSERT_FLAG_CHANGED
#undefine WASSERT_AS_CASSERT
#endif

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>
#include "../WTensorBase.h"
#include "../WMatrix.h"
#include "../WValue.h"
#include "../../WException.h"

/**
 * Test class for WTensorBase.
 *
 * \note We cannot test invalid template parameters here, as these should lead to compiler errors.
 */
class WTensorBaseTest : public CxxTest::TestSuite
{
public:
    // remember we are testing class templates
    // we'll need to instantiate every operation/member function at least once
    // in order to verify that everything compiles
    /**
     * The standard constructor should allocate enough memory and set all elements to zero.
     */
    void testTensorBaseConstructor()
    {
        // define tensor types
        typedef WTensorBase< 1, 2, double > T12;
        typedef WTensorBase< 2, 3, float > T23;
        typedef WTensorBase< 4, 2, int > T42;
        typedef WTensorBase< 6, 3, double > T63;
        typedef WTensorBase< 0, 0, int > T00;

        // standard constructor should never throw
        TS_ASSERT_THROWS_NOTHING( T12 t12() );
        T12 t12;

        // number of data elements should be 2
        // note that dataSize is private, direct access is only for testing purposes
        std::size_t ds = T12::dataSize;
        TS_ASSERT_EQUALS( ds, 2 );

        // test if all elements were set to zero
        // note that m_data is private
        for( std::size_t k = 0; k < 2; ++k )
        {
            TS_ASSERT_EQUALS( t12.m_data[ k ], 0.0 );
        }

        // do the same for some more tensors
        TS_ASSERT_THROWS_NOTHING( T23 t23() );
        T23 t23;
        ds = T23::dataSize;
        TS_ASSERT_EQUALS( ds, 9 );
        for( std::size_t k = 0; k < 9; ++k )
        {
            TS_ASSERT_EQUALS( t23.m_data[ k ], 0.0f );
        }

        TS_ASSERT_THROWS_NOTHING( T42 t42() );
        T42 t42;
        ds = T42::dataSize;
        TS_ASSERT_EQUALS( ds, 16 );
        for( std::size_t k = 0; k < 16; ++k )
        {
            TS_ASSERT_EQUALS( t42.m_data[ k ], 0 );
        }

        TS_ASSERT_THROWS_NOTHING( T63 t63() );
        T63 t63;
        ds = T63::dataSize;
        TS_ASSERT_EQUALS( ds, 729 );
        for( std::size_t k = 0; k < 729; ++k )
        {
            TS_ASSERT_EQUALS( t63.m_data[ k ], 0.0 );
        }

        TS_ASSERT_THROWS_NOTHING( T00 t00() );
        T00 t00;
        TS_ASSERT_EQUALS( t00.m_data, 0 );
    }

    /**
     * The copy constructor should copy all values.
     */
    void testWTensorBaseCopyConstructor()
    {
        typedef WTensorBase< 2, 3, int > T23;
        typedef WTensorBase< 5, 4, double > T54;
        typedef WTensorBase< 3, 3, float > T33;
        typedef WTensorBase< 0, 2, int > T02;

        {
            // create a tensor and fill in some values
            T23 t;

            t.m_data[ 2 ] = 3;
            t.m_data[ 3 ] = 2;
            t.m_data[ 7 ] = -1;

            // also test the first and last elements to avoid off-by-one error
            t.m_data[ 8 ] = -25;
            t.m_data[ 0 ] = 26;

            TS_ASSERT_THROWS_NOTHING( T23 m( t ) );
            T23 m( t );

            // the data arrays of t and m should be the same
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 9 * sizeof( int ) );

            // copy from a const ref
            T23 const& w = t;
            T23 const r( w );

            // the data arrays of r and t should be the same
            TS_ASSERT_SAME_DATA( &r.m_data[ 0 ], &t.m_data[ 0 ], 9 * sizeof( int ) );
        }

        // now test some other tensors
        {
            T54 t;

            t.m_data[ 2 ] = 3.0;
            t.m_data[ 3 ] = 2.4;
            t.m_data[ 7 ] = -1.0;
            t.m_data[ 675 ] = 20.0;
            t.m_data[ 239 ] = -134.243;
            t.m_data[ 964 ] = 567.534;
            t.m_data[ 1001 ] = -5.4276;
            t.m_data[ 543 ] = 1233.4;
            t.m_data[ 827 ] = -9878.765;

            t.m_data[ 1023 ] = -265.63;
            t.m_data[ 0 ] = 2453.0;

            TS_ASSERT_THROWS_NOTHING( T54 m( t ) );
            T54 m( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 1024 * sizeof( double ) );
        }

        {
            T33 t;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 16 ] = 200.0f;
            t.m_data[ 23 ] = -13.4243f;
            t.m_data[ 19 ] = 5675.34f;
            t.m_data[ 10 ] = -54276.0f;
            t.m_data[ 24 ] = 123.34f;
            t.m_data[ 18 ] = -98787.65f;

            t.m_data[ 26 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            TS_ASSERT_THROWS_NOTHING( T33 m( t ) );
            T33 m( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 27 * sizeof( float ) );
        }

        {
            T02 t;
            t.m_data = -5;

            TS_ASSERT_THROWS_NOTHING( T02 m( t ) );
            T02 m( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }
    }

    /**
     * Test the copy operator.
     */
    void testWTensorBaseCopyOperatorSimple()
    {
        // this is essentially the same test as with the copy constructor,
        // only this time we use the copy operator
        typedef WTensorBase< 2, 3, int > T23;
        typedef WTensorBase< 5, 4, double > T54;
        typedef WTensorBase< 3, 3, float > T33;
        typedef WTensorBase< 0, 3, double > T03;

        {
            // create a tensor and fill in some values
            T23 t, m;

            t.m_data[ 2 ] = 3;
            t.m_data[ 3 ] = 2;
            t.m_data[ 7 ] = -1;

            // also test the first and last elements to avoid off-by-one error
            t.m_data[ 8 ] = -25;
            t.m_data[ 0 ] = 26;

            // force operator =
            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            // the data arrays of t and m should be the same
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 9 * sizeof( int ) );

            // copy from a const ref
            T23 const& w = t;
            T23 r;
            r.operator = ( w );

            // the data arrays of r and t should be the same
            TS_ASSERT_SAME_DATA( &r.m_data[ 0 ], &t.m_data[ 0 ], 9 * sizeof( int ) );
        }

        // now test some other tensors
        {
            T54 t, m;

            t.m_data[ 2 ] = 3.0;
            t.m_data[ 3 ] = 2.4;
            t.m_data[ 7 ] = -1.0;
            t.m_data[ 675 ] = 20.0;
            t.m_data[ 239 ] = -134.243;
            t.m_data[ 964 ] = 567.534;
            t.m_data[ 1001 ] = -5.4276;
            t.m_data[ 543 ] = 1233.4;
            t.m_data[ 827 ] = -9878.765;

            t.m_data[ 1023 ] = -265.63;
            t.m_data[ 0 ] = 2453.0;

            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 1024 * sizeof( double ) );
        }

        {
            T33 t, m;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 16 ] = 200.0f;
            t.m_data[ 23 ] = -13.4243f;
            t.m_data[ 19 ] = 5675.34f;
            t.m_data[ 10 ] = -54276.0f;
            t.m_data[ 24 ] = 123.34f;
            t.m_data[ 18 ] = -98787.65f;

            t.m_data[ 26 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 27 * sizeof( float ) );
        }

        {
            T03 t;

            t.m_data = -4;

            TS_ASSERT_THROWS_NOTHING( T03 m( t ) );

            T03 m( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }

        // test multiple assignments in one statement
        {
            T33 t, m, u, z;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 16 ] = 200.0f;
            t.m_data[ 23 ] = -13.4243f;
            t.m_data[ 19 ] = 5675.34f;
            t.m_data[ 10 ] = -54276.0f;
            t.m_data[ 24 ] = 123.34f;
            t.m_data[ 18 ] = -98787.65f;

            t.m_data[ 26 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            z = u = m = t;

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 27 * sizeof( float ) );
            TS_ASSERT_SAME_DATA( &m.m_data[ 0 ], &u.m_data[ 0 ], 27 * sizeof( float ) );
            TS_ASSERT_SAME_DATA( &u.m_data[ 0 ], &z.m_data[ 0 ], 27 * sizeof( float ) );
        }
    }

    /**
     * Test if the copy operator handles assignments of variables to themselves correctly.
     */
    void testWTensorBaseCopyOperatorSelfCopy()
    {
        typedef WTensorBase< 3, 3, double > T33;
        typedef WTensorBase< 0, 0, int > T00;

        {
            T33 t;

            // set some elements
            t.m_data[ 0 ] = 347.856;
            t.m_data[ 26 ] = -4.0;
            t.m_data[ 4 ] = -564.4;

            // create a copy of t for comparison
            T33 m( t );

            // now try copying t onto itself
            // this should not throw anything, as the WTensor documentation states that Data_T
            // ( in this case double ) shouldn't throw on assignment
            // this is also the reason that there is no test with a datatype whose operator = throws
            TS_ASSERT_THROWS_NOTHING( t.operator = ( t ) );
            t.operator = ( t );

            // t and m should still be equal
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 27 * sizeof( double ) );
        }

        {
            T00 t;

            t.m_data = -57;

            T00 m( t );

            TS_ASSERT_THROWS_NOTHING( t.operator = ( t ) );
            t.operator = ( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }
    }

    /**
     * Test if the access operator correctly throws Exceptions only when the input indices are invalid.
     */
    void testWTensorBaseArrayAccessErrorConditions()
    {
        typedef WTensorBase< 4, 4, double > T44;
        typedef WTensorBase< 1, 4, int > T14;
        typedef WTensorBase< 3, 2, float > T32;
        typedef WTensorBase< 0, 654, int > T0;

        {
            // instantiate a tensor
            T44 t;

            // now create an index array
            int idx[] = { 0, 1, 2, 3 };
            // this should work
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // try some invalid indices, indices of a tensor of dimension 4 may only be 0, 1, 2 or 3
            idx[ 3 ] = 4; // idx == [ 0, 1, 2, 4 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            // indices are cast to std::size_t (which should be unsigned)
            idx[ 3 ] = -1; // idx == [ 0, 1, 2, -1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 2;
            idx[ 0 ] = 4537; // idx == [ 4537, 1, 2, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = -434; // idx == [ -434, 1, 2, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            // some indices that should be valid
            idx[ 0 ] = 3;
            idx[ 1 ] = 3;
            idx[ 2 ] = 3;
            idx[ 3 ] = 3;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 0;
            idx[ 3 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 1;
            idx[ 1 ] = 3;
            idx[ 2 ] = 2;
            idx[ 3 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 2;
            idx[ 3 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // a larger array should also work, all unneeded values should be ignored
            std::size_t idx2[] = { 0, 1, 2, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );

            // note that the length of the index array cannot be checked
        }

        // now do the same for another tensor
        {
            T14 t;

            int idx[] = { 0 };
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 4537;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 2;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 3;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            std::size_t idx2[] = { 0, 1, 2, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }

        // and another one
        {
            T32 t;

            // note that only values 0 and 1 are valid indices
            int idx[] = { 0, 1, 1 };
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 2 ] = 3;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 2 ] = 2;
            idx[ 0 ] = 4537;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 0;
            idx[ 1 ] = 1;
            idx[ 2 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 1;
            idx[ 1 ] = 0;
            idx[ 2 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            std::size_t idx2[] = { 0, 1, 1, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }

        {
            T0 t;

            std::size_t idx[] = { 0, 1 };
            std::size_t* idx2 = NULL;

            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }
    }

    /**
     * Test if the array access operator returns the correct elements.
     */
    void testWTensorBaseArrayAccess()
    {
        typedef WTensorBase< 2, 3, std::size_t > T23;
        typedef WTensorBase< 3, 4, std::size_t > T34;
        typedef WTensorBase< 0, 1, std::size_t > T01;

        // now test if operator [] returns the correct elements
        {
            // create a new tensor
            T23 t;

            // enumerate all elements
            for( std::size_t k = 0; k < T23::dataSize; ++k )
            {
                t.m_data[ k ] = k;
            }

            // the order of elements should be
            // 0 1 2
            // 3 4 5
            // 6 7 8
            std::size_t idx[] = { 0, 0 };
            TS_ASSERT_EQUALS( t[ idx ], 0 );
            idx[ 1 ] = 1; // idx == [ 0, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 1 );
            idx[ 1 ] = 2; // idx == [ 0, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 2 );
            idx[ 0 ] = 1; // idx == [ 1, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 5 );
            idx[ 1 ] = 1; // idx == [ 1, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 4 );
            idx[ 1 ] = 0; // idx == [ 1, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 3 );
            idx[ 0 ] = 2; // idx == [ 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 6 );
            idx[ 1 ] = 1; // idx == [ 2, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 7 );
            idx[ 1 ] = 2; // idx == [ 2, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 8 );

            // const refs should also work
            T23 const& w = t;
            idx[ 0 ] = idx[ 1 ] = 0;
            TS_ASSERT_EQUALS( w[ idx ], 0 );
            idx[ 1 ] = 1; // idx == [ 0, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 1 );
            idx[ 1 ] = 2; // idx == [ 0, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 2 );
            idx[ 0 ] = 1; // idx == [ 1, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 5 );
            idx[ 1 ] = 1; // idx == [ 1, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 4 );
            idx[ 1 ] = 0; // idx == [ 1, 0 ]
            TS_ASSERT_EQUALS( w[ idx ], 3 );
            idx[ 0 ] = 2; // idx == [ 2, 0 ]
            TS_ASSERT_EQUALS( w[ idx ], 6 );
            idx[ 1 ] = 1; // idx == [ 2, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 7 );
            idx[ 1 ] = 2; // idx == [ 2, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 8 );
        }
        {
            // create a new tensor
            T34 t;

            // enumerate all elements
            for( std::size_t k = 0; k < T34::dataSize; ++k )
            {
                t.m_data[ k ] = k;
            }

            // order should be
            //
            // idx[0] == 0  idx[0] == 1   idx[0] == 2   idx[0] == 3
            //
            // 0  1  2  3   16 17 18 19   32 33 34 35   48 49 50 51     idx[1] == 0
            // 4  5  6  7   20 21 22 23   36 37 38 39   52 53 54 55     idx[1] == 1
            // 8  9 10 11   24 25 26 27   40 41 42 43   56 57 58 59     idx[1] == 2
            //12 13 14 15   28 29 30 31   44 45 46 47   60 61 62 63     idx[1] == 3

            std::size_t idx[] = { 0, 0, 0 };
            TS_ASSERT_EQUALS( t[ idx ], 0 );
            idx[ 1 ] = 2; // idx == [ 0, 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 8 );
            idx[ 2 ] = 3; // idx == [ 0, 2, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 11 );
            idx[ 0 ] = 1; // idx == [ 1, 2, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 27 );
            idx[ 1 ] = 1; // idx == [ 1, 1, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 23 );
            idx[ 0 ] = 3; // idx == [ 3, 1, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 55 );
            idx[ 2 ] = 0; // idx == [ 3, 1, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 52 );
            idx[ 1 ] = 3; // idx == [ 3, 3, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 60 );
            idx[ 1 ] = 2; // idx == [ 3, 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 56 );
        }

        // zero order tensor
        {
            T01 t;
            t.m_data = 65;

            std::size_t idx[] = { 0, 1 };
            std::size_t* idx2 = NULL;

            TS_ASSERT_EQUALS( t[ idx ], 65 );
            TS_ASSERT_EQUALS( t[ idx2 ], 65 );
        }
    }

    /**
     * Test the std::vector version of operator [] for correct handling of
     * various input vector sizes.
     */
    void testWTensorBaseVectorAccess()
    {
        typedef WTensorBase< 4, 4, double > T44;
        typedef WTensorBase< 1, 4, double > T14;
        typedef WTensorBase< 6, 2, double > T62;
        typedef WTensorBase< 0, 1, double > T01;

        {
            T44 t;

            // test a vector of invalid size
            std::vector< int > idx;

            // this should throw a WException (using the WAssert macro)
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 ); // idx == [ 0, 1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 2 ); // idx == [ 0, 1, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 3 ); // idx == [ 0, 1, 2, 3 ]
            // now idx has the correct size and all valid indices
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // a larger vector should also work
            idx.push_back( 456 ); // idx == [ 0, 0, 2, 0, 456 ]

            // this should simply ignore all values after the 4th
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // bounds checking on the indices is done by the array version of operator [],
            // which is called by the vector version
            // I'll add some tests here though, in case this changes in the future
            idx[ 0 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 3;
            idx[ 3 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 2;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
        }
        {
            T14 t;

            std::vector< int > idx;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 3 ); // idx == [ 0, 3 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 456 ); // idx == [ 0, 3, 456 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
        }
        {
            T62 t;

            std::vector< int > idx;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 ); // idx == [ 0, 1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 0 ); // idx == [ 0, 1, 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 );
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 );
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 0 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 456 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
        }
        {
            T01 t;

            std::vector< int > idx;

            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 4 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            T01 const w;
            TS_ASSERT_THROWS_NOTHING( w[ idx ] );
        }
    }

    /**
     * Test if operator == works correctly.
     */
    void testWTensorBaseCompareOperator()
    {
        typedef WTensorBase< 1, 7, double > T17;
        typedef WTensorBase< 3, 2, int > T32;
        typedef WTensorBase< 0, 0, float > T00;

        {
            T17 t, m;

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // change some values
            std::size_t idx[] = { 4 };
            t[ idx ] = 5.0;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            m[ idx ] = 5.0;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 0 ] = 2;
            m[ idx ] = 543543.0;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // copying a tensor should lead to the respective tensors being equal
            t = m;
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // test const
            T17 const ct, cm;
            TS_ASSERT( ct == cm );
            TS_ASSERT( cm == ct );
            TS_ASSERT( ct == ct );
            TS_ASSERT( cm == cm );
        }
        {
            T32 t, m;

            std::size_t idx[] = { 0, 0, 0 };

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 1 ] = 1;

            m[ idx ] = -5643;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t = m;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 1 ] = 0;

            t[ idx ] = 564;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t.m_data[ 0 ] = 5;
            t.m_data[ 1 ] = -65464;
            t.m_data[ 2 ] = 89;
            t.m_data[ 3 ] = 3276;
            t.m_data[ 4 ] = -3276;
            t.m_data[ 5 ] = 47;
            t.m_data[ 6 ] = 68;
            t.m_data[ 7 ] = -239;

            m.m_data[ 0 ] = -5;
            m.m_data[ 1 ] = 65464;
            m.m_data[ 2 ] = -89;
            m.m_data[ 3 ] = -3276;
            m.m_data[ 4 ] = 3276;
            m.m_data[ 5 ] = -47;
            m.m_data[ 6 ] = -68;
            m.m_data[ 7 ] = 239;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );
        }
        {
            T00 t, m;

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t.m_data = 2;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            m.m_data = 2;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );
        }
    }

    /**
     * Test if operator != works correctly.
     */
    void testWTensorBaseCompareOperator2()
    {
        typedef WTensorBase< 3, 3, int > T33;
        typedef WTensorBase< 0, 0, int > T00;

        {
            T33 t, m;

            // two new instances should never not be equal
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );

            // a tensor should never not be equal to itself
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            // change some elements
            t.m_data[ 23 ] = -23467;

            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t = m;
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );

            t.m_data[ 0 ] = 1;
            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t = m;
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t.m_data[ 26 ] = -1;
            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            // test const
            T33 const ct, cm;
            TS_ASSERT( !( ct != cm ) );
            TS_ASSERT( !( cm != ct ) );
            TS_ASSERT( !( ct != ct ) );
            TS_ASSERT( !( cm != cm ) );
        }
        {
            T00 t, m;

            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t.m_data = 2;

            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            m.m_data = 2;

            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );
        }
    }
};

/**
 * Test class for WTensorBaseSym.
 *
 * \note We cannot test invalid template parameters here, as these should lead to compiler errors.
 */
class WTensorBaseSymTest : public CxxTest::TestSuite
{
public:
    /**
     * The standard constructor should allocate enough memory and set all elements to zero.
     */
    void testTensorBaseSymConstructor()
    {
        // define tensor types
        typedef WTensorBaseSym< 1, 2, double > T12;
        typedef WTensorBaseSym< 2, 3, float > T23;
        typedef WTensorBaseSym< 4, 2, int > T42;
        typedef WTensorBaseSym< 6, 3, double > T63;
        typedef WTensorBaseSym< 0, 0, int > T00;

        // standard constructor should never throw
        TS_ASSERT_THROWS_NOTHING( T12 t12() );
        T12 t12;

        // number of data elements should be 2
        // note that dataSize is private, direct access is only for testing purposes
        std::size_t ds = T12::dataSize;
        TS_ASSERT_EQUALS( ds, 2 );

        // test if all elements were set to zero
        // note that m_data is private
        for( std::size_t k = 0; k < 2; ++k )
        {
            TS_ASSERT_EQUALS( t12.m_data[ k ], 0.0 );
        }

        // do the same for some more tensors
        // symmetric tensors need less memory, 6 instead of 9 values in this case
        TS_ASSERT_THROWS_NOTHING( T23 t23() );
        T23 t23;
        ds = T23::dataSize;
        TS_ASSERT_EQUALS( ds, 6 );
        for( std::size_t k = 0; k < 6; ++k )
        {
            TS_ASSERT_EQUALS( t23.m_data[ k ], 0.0f );
        }

        TS_ASSERT_THROWS_NOTHING( T42 t42() );
        T42 t42;
        ds = T42::dataSize;
        TS_ASSERT_EQUALS( ds, 5 );
        for( std::size_t k = 0; k < 5; ++k )
        {
            TS_ASSERT_EQUALS( t42.m_data[ k ], 0 );
        }

        TS_ASSERT_THROWS_NOTHING( T63 t63() );
        T63 t63;
        ds = T63::dataSize;
        TS_ASSERT_EQUALS( ds, 28 );
        for( std::size_t k = 0; k < 28; ++k )
        {
            TS_ASSERT_EQUALS( t63.m_data[ k ], 0.0 );
        }

        TS_ASSERT_THROWS_NOTHING( T00 t00() );
        T00 t00;
        TS_ASSERT_EQUALS( t00.m_data, 0 );
    }

    /**
     * The copy constructor should copy all values.
     */
    void testWTensorBaseSymCopyConstructor()
    {
        typedef WTensorBaseSym< 2, 3, int > T23;
        typedef WTensorBaseSym< 5, 4, double > T54;
        typedef WTensorBaseSym< 3, 3, float > T33;
        typedef WTensorBaseSym< 0, 2, int > T02;

        {
            // create a tensor and fill in some values
            // use direct access to the m_data array as access operators aren't tested yet
            T23 t;

            t.m_data[ 2 ] = 3;
            t.m_data[ 3 ] = 2;
            t.m_data[ 4 ] = -1;

            // also test the first and last elements to avoid off-by-one error
            t.m_data[ 5 ] = -25;
            t.m_data[ 0 ] = 26;

            TS_ASSERT_THROWS_NOTHING( T23 m( t ) );
            T23 m( t );

            // the data arrays of t and m should be the same
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 6 * sizeof( int ) );

            // copy from a const ref
            T23 const& w = t;
            T23 const r( w );

            // the data arrays of r and t should be the same
            TS_ASSERT_SAME_DATA( &r.m_data[ 0 ], &t.m_data[ 0 ], 6 * sizeof( int ) );
        }

        // now test some other tensors
        {
            T54 t;

            t.m_data[ 2 ] = 3.0;
            t.m_data[ 3 ] = 2.4;
            t.m_data[ 7 ] = -1.0;
            t.m_data[ 33 ] = 20.0;
            t.m_data[ 21 ] = -134.243;
            t.m_data[ 54 ] = 567.534;
            t.m_data[ 48 ] = -5.4276;
            t.m_data[ 34 ] = 1233.4;
            t.m_data[ 27 ] = -9878.765;

            t.m_data[ 55 ] = -265.63;
            t.m_data[ 0 ] = 2453.0;

            TS_ASSERT_THROWS_NOTHING( T54 m( t ) );
            T54 m( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 56 * sizeof( double ) );
        }

        {
            T33 t;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 1 ] = -13.4243f;
            t.m_data[ 5 ] = 5675.34f;
            t.m_data[ 6 ] = -54276.0f;
            t.m_data[ 4 ] = 123.34f;
            t.m_data[ 8 ] = -98787.65f;

            t.m_data[ 9 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            TS_ASSERT_THROWS_NOTHING( T33 m( t ) );
            T33 m( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 10 * sizeof( float ) );
        }

        // test multiple assignments in one statement
        {
            T33 t, m, u, z;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 1 ] = -13.4243f;
            t.m_data[ 5 ] = 5675.34f;
            t.m_data[ 6 ] = -54276.0f;
            t.m_data[ 4 ] = 123.34f;
            t.m_data[ 8 ] = -98787.65f;

            t.m_data[ 9 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            z = u = m = t;

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 10 * sizeof( float ) );
            TS_ASSERT_SAME_DATA( &m.m_data[ 0 ], &u.m_data[ 0 ], 10 * sizeof( float ) );
            TS_ASSERT_SAME_DATA( &u.m_data[ 0 ], &z.m_data[ 0 ], 10 * sizeof( float ) );
        }

        {
            T02 t;
            t.m_data = -5;

            TS_ASSERT_THROWS_NOTHING( T02 m( t ) );
            T02 m( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }
    }

    /**
     * Test the copy operator.
     */
    void testWTensorBaseSymCopyOperatorSimple()
    {
        // this is essentially the same test as with the copy constructor,
        // only this time we use the copy operator

        typedef WTensorBaseSym< 2, 3, int > T23;
        typedef WTensorBaseSym< 5, 4, double > T54;
        typedef WTensorBaseSym< 3, 3, float > T33;
        typedef WTensorBaseSym< 0, 3, double > T03;

        {
            // create a tensor and fill in some values
            // use direct access to the m_data array as access operators aren't tested yet
            T23 t, m;

            t.m_data[ 2 ] = 3;
            t.m_data[ 3 ] = 2;
            t.m_data[ 1 ] = -1;

            // also test the first and last elements to avoid off-by-one error
            t.m_data[ 5 ] = -25;
            t.m_data[ 0 ] = 26;

            // force operator =
            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            // the data arrays of t and m should be the same
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 6 * sizeof( int ) );

            // copy from a const ref
            T23 const& w = t;
            T23 r;
            r.operator = ( w );

            // the data arrays of r and t should be the same
            TS_ASSERT_SAME_DATA( &r.m_data[ 0 ], &t.m_data[ 0 ], 6 * sizeof( int ) );
        }

        // now test some other tensors
        {
            T54 t, m;

            t.m_data[ 2 ] = 3.0;
            t.m_data[ 3 ] = 2.4;
            t.m_data[ 7 ] = -1.0;
            t.m_data[ 33 ] = 20.0;
            t.m_data[ 21 ] = -134.243;
            t.m_data[ 54 ] = 567.534;
            t.m_data[ 48 ] = -5.4276;
            t.m_data[ 34 ] = 1233.4;
            t.m_data[ 27 ] = -9878.765;

            t.m_data[ 55 ] = -265.63;
            t.m_data[ 0 ] = 2453.0;

            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 56 * sizeof( double ) );
        }

        {
            T33 t, m;

            t.m_data[ 2 ] = 3.0f;
            t.m_data[ 3 ] = 2.0f;
            t.m_data[ 7 ] = -1.0f;
            t.m_data[ 1 ] = -13.4243f;
            t.m_data[ 5 ] = 5675.34f;
            t.m_data[ 6 ] = -54276.0f;
            t.m_data[ 4 ] = 123.34f;
            t.m_data[ 8 ] = -98787.65f;

            t.m_data[ 9 ] = -26.563f;
            t.m_data[ 0 ] = 245.3f;

            TS_ASSERT_THROWS_NOTHING( m.operator = ( t ) );
            m.operator = ( t );

            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 10 * sizeof( float ) );
        }

        {
            T03 t;

            t.m_data = -4;

            TS_ASSERT_THROWS_NOTHING( T03 m( t ) );

            T03 m( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }
    }

    /**
     * Test if the copy operator handles assignments of variables to themselves correctly.
     */
    void testWTensorBaseSymCopyOperatorSelfCopy()
    {
        typedef WTensorBaseSym< 3, 3, double > T33;
        typedef WTensorBaseSym< 0, 0, int > T00;

        {
            T33 t;

            // set some elements
            t.m_data[ 0 ] = 347.856;
            t.m_data[ 9 ] = -4.0;
            t.m_data[ 4 ] = -564.4;

            // create a copy of t for comparison
            T33 m( t );

            // now try copying t onto itself
            // this should not throw anything, as the WTensorSym documentation states that Data_T
            // ( in this case double ) shouldn't throw on assignment
            // this is also the reason that there is no test with a datatype whose operator = throws
            TS_ASSERT_THROWS_NOTHING( t.operator = ( t ) );
            t.operator = ( t );

            // t and m should still be equal
            TS_ASSERT_SAME_DATA( &t.m_data[ 0 ], &m.m_data[ 0 ], 10 * sizeof( double ) );
        }

        {
            T00 t;

            t.m_data = -57;

            T00 m( t );

            TS_ASSERT_THROWS_NOTHING( t.operator = ( t ) );
            t.operator = ( t );

            TS_ASSERT_EQUALS( m.m_data, t.m_data );
        }
    }

    /**
     * Test if the access operator correctly throws Exceptions only when the input indices are invalid.
     */
    void testWTensorBaseSymArrayAccessErrorConditions()
    {
        typedef WTensorBaseSym< 4, 4, double > T44;
        typedef WTensorBaseSym< 1, 4, int > T14;
        typedef WTensorBaseSym< 3, 2, float > T32;
        typedef WTensorBaseSym< 0, 654, int > T0;

        // first, we'll check some error conditions
        {
            // instantiate a tensor
            T44 t;

            // now create an index array
            int idx[] = { 0, 1, 2, 3 };
            // this should work
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // try some invalid indices, indices of a tensor of dimension 4 may only be 0, 1, 2 or 3
            idx[ 3 ] = 4; // idx == [ 0, 1, 2, 4 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            // indices are cast to std::size_t (which should be unsigned)
            idx[ 3 ] = -1; // idx == [ 0, 1, 2, -1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 2;
            idx[ 0 ] = 4537; // idx == [ 4537, 1, 2, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = -434; // idx == [ -434, 1, 2, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            // some indices that should be valid
            idx[ 0 ] = 3;
            idx[ 1 ] = 3;
            idx[ 2 ] = 3;
            idx[ 3 ] = 3;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 0;
            idx[ 3 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 1;
            idx[ 1 ] = 3;
            idx[ 2 ] = 2;
            idx[ 3 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 2;
            idx[ 3 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // a larger array should also work, all unneeded values should be ignored
            std::size_t idx2[] = { 0, 1, 2, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );

            // note that the length of the index array cannot be checked
        }

        // now do the same for another tensor
        {
            T14 t;

            int idx[] = { 0 };
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 4537;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 2;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 3;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            std::size_t idx2[] = { 0, 1, 2, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }

        // and another one
        {
            T32 t;

            // note that only values 0 and 1 are valid indices
            int idx[] = { 0, 1, 1 };
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 2 ] = 3;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 2 ] = 2;
            idx[ 0 ] = 4537;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 0;
            idx[ 1 ] = 1;
            idx[ 2 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 0;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 1;
            idx[ 1 ] = 0;
            idx[ 2 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx[ 0 ] = 0;
            idx[ 1 ] = 0;
            idx[ 2 ] = 1;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            std::size_t idx2[] = { 0, 1, 1, 3, 8, 54643 };
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }

        {
            T0 t;

            std::size_t idx[] = { 0, 1 };
            std::size_t* idx2 = NULL;

            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
            TS_ASSERT_THROWS_NOTHING( t[ idx2 ] );
        }
    }

    /**
     * Test if the array access operator returns the correct elements.
     */
    void testWTensorBaseSymArrayAccess()
    {
        typedef WTensorBaseSym< 2, 3, std::size_t > T23;
        typedef WTensorBaseSym< 3, 4, std::size_t > T34;

        // now test if operator [] returns the correct elements
        {
            // create a new tensor
            T23 t;

            // enumerate all elements
            for( std::size_t k = 0; k < T23::dataSize; ++k )
            {
                t.m_data[ k ] = k;
            }

            // the order of elements should be
            // 0 1 2
            //   3 4
            //     5
            std::size_t idx[] = { 0, 0 };
            TS_ASSERT_EQUALS( t[ idx ], 0 );
            idx[ 1 ] = 1; // idx == [ 0, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 1 );
            idx[ 1 ] = 2; // idx == [ 0, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 2 );
            idx[ 0 ] = 1; // idx == [ 1, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 4 );
            idx[ 1 ] = 1; // idx == [ 1, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 3 );
            idx[ 1 ] = 0; // idx == [ 1, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 1 );
            idx[ 0 ] = 2; // idx == [ 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 2 );
            idx[ 1 ] = 1; // idx == [ 2, 1 ]
            TS_ASSERT_EQUALS( t[ idx ], 4 );
            idx[ 1 ] = 2; // idx == [ 2, 2 ]
            TS_ASSERT_EQUALS( t[ idx ], 5 );

            // const refs should also work
            T23 const& w = t;
            idx[ 0 ] = idx[ 1 ] = 0;
            TS_ASSERT_EQUALS( w[ idx ], 0 );
            idx[ 1 ] = 1; // idx == [ 0, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 1 );
            idx[ 1 ] = 2; // idx == [ 0, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 2 );
            idx[ 0 ] = 1; // idx == [ 1, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 4 );
            idx[ 1 ] = 1; // idx == [ 1, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 3 );
            idx[ 1 ] = 0; // idx == [ 1, 0 ]
            TS_ASSERT_EQUALS( w[ idx ], 1 );
            idx[ 0 ] = 2; // idx == [ 2, 0 ]
            TS_ASSERT_EQUALS( w[ idx ], 2 );
            idx[ 1 ] = 1; // idx == [ 2, 1 ]
            TS_ASSERT_EQUALS( w[ idx ], 4 );
            idx[ 1 ] = 2; // idx == [ 2, 2 ]
            TS_ASSERT_EQUALS( w[ idx ], 5 );
        }
        {
            // create a new tensor
            T34 t;

            // enumerate all elements
            for( std::size_t k = 0; k < T34::dataSize; ++k )
            {
                t.m_data[ k ] = k;
            }

            // order should be
            //
            // idx[0] == 0  idx[0] == 1   idx[0] == 2   idx[0] == 3
            //
            // 0  1  2  3                                               idx[1] == 0
            //    4  5  6      10 11 12                                 idx[1] == 1
            //       7  8         13 14         16 17                   idx[1] == 2
            //          9            15            18            19     idx[1] == 3

            std::size_t idx[] = { 0, 0, 0 };
            TS_ASSERT_EQUALS( t[ idx ], 0 );
            idx[ 1 ] = 2; // idx == [ 0, 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 2 );
            idx[ 2 ] = 3; // idx == [ 0, 2, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 8 );
            idx[ 0 ] = 1; // idx == [ 1, 2, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 14 );
            idx[ 1 ] = 1; // idx == [ 1, 1, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 12 );
            idx[ 0 ] = 3; // idx == [ 3, 1, 3 ]
            TS_ASSERT_EQUALS( t[ idx ], 15 );
            idx[ 2 ] = 0; // idx == [ 3, 1, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 6 );
            idx[ 1 ] = 3; // idx == [ 3, 3, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 9 );
            idx[ 1 ] = 2; // idx == [ 3, 2, 0 ]
            TS_ASSERT_EQUALS( t[ idx ], 8 );
        }
    }

    /**
     * Test if operator [] correctly maps permutations of the same set of indices to
     * the same array positions.
     */
    void testWTensorBaseSymAccessOperatorPermutations()
    {
        typedef WTensorBaseSym< 3, 4, std::size_t > T34;

        T34 t;

        // enumerate all elements
        for( std::size_t k = 0; k < T34::dataSize; ++k )
        {
            t.m_data[ k ] = k;
        }

        // create some index set permutations
        std::size_t idx1[ 3 ][ 3 ] = { { 0, 0, 1 }, // NOLINT no extra lines for { or } in an array initialization
                                       { 0, 1, 0 }, // NOLINT
                                       { 1, 0, 0 } }; // NOLINT

        std::size_t idx2[ 6 ][ 3 ] = { { 0, 1, 2 }, // NOLINT
                                       { 0, 2, 1 }, // NOLINT
                                       { 1, 2, 0 }, // NOLINT
                                       { 2, 1, 0 }, // NOLINT
                                       { 1, 0, 2 }, // NOLINT
                                       { 2, 1, 0 } }; // NOLINT

        std::size_t idx3[ 3 ][ 3 ] = { { 0, 0, 3 }, // NOLINT
                                       { 0, 3, 0 }, // NOLINT
                                       { 3, 0, 0 } }; // NOLINT

        std::size_t idx4[ 6 ][ 3 ] = { { 0, 3, 2 }, // NOLINT
                                       { 0, 2, 3 }, // NOLINT
                                       { 3, 2, 0 }, // NOLINT
                                       { 2, 3, 0 }, // NOLINT
                                       { 3, 0, 2 }, // NOLINT
                                       { 2, 3, 0 } }; // NOLINT

        // operator [] should map any permutation of a set of indices onto the same array position
        TS_ASSERT_EQUALS( t[ idx1[ 0 ] ], t[ idx1[ 1 ] ] );
        TS_ASSERT_EQUALS( t[ idx1[ 1 ] ], t[ idx1[ 2 ] ] );

        TS_ASSERT_EQUALS( t[ idx2[ 0 ] ], t[ idx2[ 1 ] ] );
        TS_ASSERT_EQUALS( t[ idx2[ 1 ] ], t[ idx2[ 2 ] ] );
        TS_ASSERT_EQUALS( t[ idx2[ 2 ] ], t[ idx2[ 3 ] ] );
        TS_ASSERT_EQUALS( t[ idx2[ 3 ] ], t[ idx2[ 4 ] ] );
        TS_ASSERT_EQUALS( t[ idx2[ 4 ] ], t[ idx2[ 5 ] ] );

        TS_ASSERT_EQUALS( t[ idx3[ 0 ] ], t[ idx3[ 1 ] ] );
        TS_ASSERT_EQUALS( t[ idx3[ 1 ] ], t[ idx3[ 2 ] ] );

        TS_ASSERT_EQUALS( t[ idx4[ 0 ] ], t[ idx4[ 1 ] ] );
        TS_ASSERT_EQUALS( t[ idx4[ 1 ] ], t[ idx4[ 2 ] ] );
        TS_ASSERT_EQUALS( t[ idx4[ 2 ] ], t[ idx4[ 3 ] ] );
        TS_ASSERT_EQUALS( t[ idx4[ 3 ] ], t[ idx4[ 4 ] ] );
        TS_ASSERT_EQUALS( t[ idx4[ 4 ] ], t[ idx4[ 5 ] ] );

        // permutations of different index sets may never map onto the same position
        TS_ASSERT_DIFFERS( t[ idx1[ 0 ] ], t[ idx2[ 0 ] ] );
        TS_ASSERT_DIFFERS( t[ idx1[ 1 ] ], t[ idx2[ 5 ] ] );
        TS_ASSERT_DIFFERS( t[ idx2[ 0 ] ], t[ idx4[ 4 ] ] );
        TS_ASSERT_DIFFERS( t[ idx2[ 0 ] ], t[ idx3[ 2 ] ] );
        TS_ASSERT_DIFFERS( t[ idx2[ 3 ] ], t[ idx3[ 1 ] ] );
        TS_ASSERT_DIFFERS( t[ idx3[ 0 ] ], t[ idx2[ 0 ] ] );
        TS_ASSERT_DIFFERS( t[ idx3[ 1 ] ], t[ idx4[ 3 ] ] );
        TS_ASSERT_DIFFERS( t[ idx3[ 0 ] ], t[ idx1[ 2 ] ] );
        TS_ASSERT_DIFFERS( t[ idx4[ 2 ] ], t[ idx1[ 0 ] ] );
        TS_ASSERT_DIFFERS( t[ idx4[ 5 ] ], t[ idx3[ 2 ] ] );
    }

    /**
     * Test the std::vector version of operator [] for correct handling of
     * various input vector sizes.
     */
    void testWTensorBaseSymVectorAccess()
    {
        typedef WTensorBaseSym< 4, 4, double > T44;
        typedef WTensorBaseSym< 1, 4, double > T14;
        typedef WTensorBaseSym< 6, 2, double > T62;

        {
            T44 t;

            // test a vector of invalid size
            std::vector< int > idx;

            // this should throw a WException (using the WAssert macro)
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 ); // idx == [ 0, 1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 2 ); // idx == [ 0, 1, 2 ]
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 3 ); // idx == [ 0, 1, 2, 3 ]
            // now idx has the correct size and all valid indices
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // a larger vector should also work
            idx.push_back( 456 ); // idx == [ 0, 0, 2, 0, 456 ]

            // this should simply ignore all values after the 4th
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            // bounds checking on the indices is done by the array version of operator [],
            // which is called by the vector version
            // I'll add some tests here though, in case this changes in the future
            idx[ 0 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 0 ] = 3;
            idx[ 3 ] = -1;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 4;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx[ 3 ] = 2;
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
        }
        {
            T14 t;

            std::vector< int > idx;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 3 ); // idx == [ 0, 3 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 456 ); // idx == [ 0, 3, 456 ]
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );
        }
        {
            T62 t;

            std::vector< int > idx;
            TS_ASSERT_THROWS( t[ idx ], WException );

            idx.push_back( 0 ); // idx == [ 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 ); // idx == [ 0, 1 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 0 ); // idx == [ 0, 1, 0 ]
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 );
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 1 );
            TS_ASSERT_THROWS( t[ idx ], WException );
            idx.push_back( 0 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( 456 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            idx.push_back( -1 );
            TS_ASSERT_THROWS_NOTHING( t[ idx ] );

            T62 const w;
            TS_ASSERT_THROWS_NOTHING( w[ idx ] );
        }
    }

    /**
     * Test if operator == works correctly.
     */
    void testWTensorBaseSymCompareOperator()
    {
        typedef WTensorBaseSym< 1, 7, double > T17;
        typedef WTensorBaseSym< 3, 2, int > T32;
        typedef WTensorBaseSym< 0, 0, float > T00;

        {
            T17 t, m;

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // change some values
            std::size_t idx[] = { 4 };
            t[ idx ] = 5.0;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            m[ idx ] = 5.0;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 0 ] = 2;
            m[ idx ] = 543543.0;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // copying a tensor should lead to the respective tensors being equal
            t = m;
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            // test const
            T17 const ct, cm;
            TS_ASSERT( ct == cm );
            TS_ASSERT( cm == ct );
            TS_ASSERT( ct == ct );
            TS_ASSERT( cm == cm );
        }
        {
            T32 t, m;

            std::size_t idx[] = { 0, 0, 0 };

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 1 ] = 1;

            m[ idx ] = -5643;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t = m;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            idx[ 1 ] = 0;

            t[ idx ] = 564;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t.m_data[ 0 ] = 5;
            t.m_data[ 1 ] = -65464;
            t.m_data[ 2 ] = 89;
            t.m_data[ 3 ] = 3276;

            m.m_data[ 0 ] = -5;
            m.m_data[ 1 ] = 65464;
            m.m_data[ 2 ] = -89;
            m.m_data[ 3 ] = -3276;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );
        }
        {
            T00 t, m;

            // two tensors of the same type should be initialized to the same values
            TS_ASSERT( t == m );
            TS_ASSERT( m == t );

            // a tensor should always be equal to itself
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            t.m_data = 2;

            TS_ASSERT( !( t == m ) );
            TS_ASSERT( !( m == t ) );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );

            m.m_data = 2;

            TS_ASSERT( t == m );
            TS_ASSERT( m == t );
            TS_ASSERT( t == t );
            TS_ASSERT( m == m );
        }
    }

    /**
     * Test if operator != works correctly.
     */
    void testWTensorBaseSymCompareOperator2()
    {
        typedef WTensorBaseSym< 3, 3, int > T33;
        typedef WTensorBaseSym< 0, 0, int > T00;

        {
            T33 t, m;

            // two new instances should never not be equal
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );

            // a tensor should never not be equal to itself
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            // change some elements
            t.m_data[ 4 ] = -23467;

            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t = m;
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );

            t.m_data[ 0 ] = 1;
            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t = m;
            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t.m_data[ 9 ] = -1;
            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            // test const
            T33 const ct, cm;
            TS_ASSERT( !( ct != cm ) );
            TS_ASSERT( !( cm != ct ) );
            TS_ASSERT( !( ct != ct ) );
            TS_ASSERT( !( cm != cm ) );
        }

        {
            T00 t, m;

            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            t.m_data = 2;

            TS_ASSERT( t != m );
            TS_ASSERT( m != t );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );

            m.m_data = 2;

            TS_ASSERT( !( t != m ) );
            TS_ASSERT( !( m != t ) );
            TS_ASSERT( !( t != t ) );
            TS_ASSERT( !( m != m ) );
        }
    }
};

/**
 * A class that tests the WTensorFunc template.
 */
class WTensorFuncTest : public CxxTest::TestSuite
{
public:
    /**
     * Test operator () error conditions.
     */
    void testAccessOperatorErrors()
    {
        // first test with an asymmetric tensor base
        typedef WTensorFunc< WTensorBase, 3, 3, double > F33;
        typedef WTensorFunc< WTensorBase, 1, 5, int > F15;

        {
            F33 f;

            // try some valid indices
            TS_ASSERT_THROWS_NOTHING( f( 0, 0, 0 ) );
            TS_ASSERT_THROWS_NOTHING( f( 1, 0, 0 ) );
            TS_ASSERT_THROWS_NOTHING( f( 2, 2, 2 ) );
            TS_ASSERT_THROWS_NOTHING( f( 1, 1, 1 ) );
            TS_ASSERT_THROWS_NOTHING( f( 1, 0, 2 ) );
            TS_ASSERT_THROWS_NOTHING( f( 0, 2, 0 ) );

            // try some invalid indices
            // negative indices are not allowed as the parameters are of type std::size_t
            TS_ASSERT_THROWS( f( 0, 0, 3 ), WException );
            TS_ASSERT_THROWS( f( 0, 654465, 0 ), WException );
            TS_ASSERT_THROWS( f( 4, 0, 0 ), WException );
            TS_ASSERT_THROWS( f( 0, 0, 45 ), WException );
            TS_ASSERT_THROWS( f( 0, 64, 0 ), WException );
            TS_ASSERT_THROWS( f( 792, 981, 5645 ), WException );
        }
        {
            F15 f;

            TS_ASSERT_THROWS_NOTHING( f( 0 ) );
            TS_ASSERT_THROWS_NOTHING( f( 1 ) );
            TS_ASSERT_THROWS_NOTHING( f( 2 ) );
            TS_ASSERT_THROWS_NOTHING( f( 3 ) );
            TS_ASSERT_THROWS_NOTHING( f( 4 ) );

            TS_ASSERT_THROWS( f( 5 ), WException );
            TS_ASSERT_THROWS( f( 5436 ), WException );
        }

        // now try a symmetric tensor base
        typedef WTensorFunc< WTensorBaseSym, 2, 4, double > F24;

        {
            F24 f;

            TS_ASSERT_THROWS_NOTHING( f( 0, 0 ) );
            TS_ASSERT_THROWS_NOTHING( f( 3, 0 ) );
            TS_ASSERT_THROWS_NOTHING( f( 2, 3 ) );
            TS_ASSERT_THROWS_NOTHING( f( 3, 3 ) );
            TS_ASSERT_THROWS_NOTHING( f( 0, 1 ) );

            TS_ASSERT_THROWS( f( 4, 0 ), WException );
            TS_ASSERT_THROWS( f( 3, 457 ), WException );
        }
    }

    /**
     * Test if operator () returns the correct elements.
     */
    void testAccessOperator()
    {
        typedef WTensorFunc< WTensorBase, 6, 2, std::size_t > F62;
        typedef WTensorBase< 6, 2, std::size_t > Base62;

        F62 f;
        Base62& b = f;

        for( std::size_t k = 0; k < 64; ++k )
        {
            b.m_data[ k ] = k;
        }

        TS_ASSERT_EQUALS( f( 0, 0, 0, 0, 0, 0 ), 0 );
        TS_ASSERT_EQUALS( f( 0, 0, 0, 1, 0, 1 ), 5 );
        TS_ASSERT_EQUALS( f( 1, 1, 1, 0, 0, 0 ), 56 );
        TS_ASSERT_EQUALS( f( 0, 1, 0, 0, 0, 1 ), 17 );
        TS_ASSERT_EQUALS( f( 0, 0, 1, 0, 1, 0 ), 10 );
        TS_ASSERT_EQUALS( f( 1, 0, 1, 0, 0, 1 ), 41 );
        TS_ASSERT_EQUALS( f( 1, 1, 1, 1, 1, 1 ), 63 );

        F62 const& w = f;
        TS_ASSERT_EQUALS( w( 0, 0, 0, 0, 0, 0 ), 0 );
        TS_ASSERT_EQUALS( w( 0, 0, 0, 1, 0, 1 ), 5 );
        TS_ASSERT_EQUALS( w( 1, 1, 1, 0, 0, 0 ), 56 );
        TS_ASSERT_EQUALS( w( 0, 1, 0, 0, 0, 1 ), 17 );
        TS_ASSERT_EQUALS( w( 0, 0, 1, 0, 1, 0 ), 10 );
        TS_ASSERT_EQUALS( w( 1, 0, 1, 0, 0, 1 ), 41 );
        TS_ASSERT_EQUALS( w( 1, 1, 1, 1, 1, 1 ), 63 );
    }

    /**
     * Test if operator () keeps the symmetry of a WTensorBaseSym intact.
     */
    void testAccessOperatorSymmetry()
    {
        typedef WTensorFunc< WTensorBaseSym, 4, 5, std::size_t > F45;
        typedef WTensorBaseSym< 4, 5, std::size_t > Base45;

        F45 f;
        Base45& b = f;

        for( std::size_t k = 0; k < 70; ++k )
        {
            b.m_data[ k ] = k;
        }

        std::size_t idx[ 8 ][ 6 ] = { { 0, 1, 2, 4 }, // NOLINT no extra line per { or }
                                      { 3, 2, 4, 0 }, // NOLINT
                                      { 4, 4, 4, 0 }, // NOLINT
                                      { 0, 0, 0, 0 }, // NOLINT
                                      { 3, 4, 0, 1 }, // NOLINT
                                      { 2, 2, 2, 2 }, // NOLINT
                                      { 4, 4, 4, 4 }, // NOLINT
                                      { 2, 2, 0, 3 } };  // NOLINT

        TS_ASSERT( f( 0, 1, 2, 4 ) == f[ idx[ 0 ] ] );
        TS_ASSERT( f( 1, 0, 2, 4 ) == f[ idx[ 0 ] ] );
        TS_ASSERT( f( 4, 1, 0, 2 ) == f[ idx[ 0 ] ] );
        TS_ASSERT( f( 0, 3, 2, 4 ) == f[ idx[ 1 ] ] );
        TS_ASSERT( f( 0, 4, 4, 4 ) == f[ idx[ 2 ] ] );
        TS_ASSERT( f( 4, 0, 4, 4 ) == f[ idx[ 2 ] ] );
        TS_ASSERT( f( 0, 0, 0, 0 ) == f[ idx[ 3 ] ] );
        TS_ASSERT( f( 0, 1, 3, 4 ) == f[ idx[ 4 ] ] );
        TS_ASSERT( f( 2, 2, 2, 2 ) == f[ idx[ 5 ] ] );
        TS_ASSERT( f( 4, 4, 4, 4 ) == f[ idx[ 6 ] ] );
        TS_ASSERT( f( 2, 2, 3, 0 ) == f[ idx[ 7 ] ] );
        TS_ASSERT( f( 2, 3, 0, 2 ) == f[ idx[ 7 ] ] );

        F45 const& w = f;
        TS_ASSERT( w( 0, 1, 2, 4 ) == w[ idx[ 0 ] ] );
        TS_ASSERT( w( 1, 0, 2, 4 ) == w[ idx[ 0 ] ] );
        TS_ASSERT( w( 4, 1, 0, 2 ) == w[ idx[ 0 ] ] );
        TS_ASSERT( w( 0, 3, 2, 4 ) == w[ idx[ 1 ] ] );
        TS_ASSERT( w( 0, 4, 4, 4 ) == w[ idx[ 2 ] ] );
        TS_ASSERT( w( 4, 0, 4, 4 ) == w[ idx[ 2 ] ] );
        TS_ASSERT( w( 0, 0, 0, 0 ) == w[ idx[ 3 ] ] );
        TS_ASSERT( w( 0, 1, 3, 4 ) == w[ idx[ 4 ] ] );
        TS_ASSERT( w( 2, 2, 2, 2 ) == w[ idx[ 5 ] ] );
        TS_ASSERT( w( 4, 4, 4, 4 ) == w[ idx[ 6 ] ] );
        TS_ASSERT( w( 2, 2, 3, 0 ) == w[ idx[ 7 ] ] );
        TS_ASSERT( w( 2, 3, 0, 2 ) == w[ idx[ 7 ] ] );
    }
};

/**
 * Test all typecasts and copy operators that copy from another type.
 */
class WTensorTypesTest : public CxxTest::TestSuite
{
public:
    /**
     * Test constructing a WTensorBase from a WTensorBaseSym.
     */
    void testCopyContructorBaseFromBaseSym()
    {
        typedef WTensorBaseSym< 2, 4, double > S24;
        typedef WTensorBase< 2, 4, double > T24;
        typedef WTensorBaseSym< 1, 4, double > S14;
        typedef WTensorBase< 1, 4, double > T14;
        typedef WTensorBaseSym< 0, 4, double > S04;
        typedef WTensorBase< 0, 4, double > T04;

        // construct a symmetric tensor and initialize an asymmetric tensor from it
        {
            S24 s;

            std::size_t idx[ 2 ] = { 0, 3 };

            s[ idx ] = -2.0;

            idx[ 0 ] = 3;
            idx[ 1 ] = 2;

            s[ idx ] = 3.0;

            TS_ASSERT_THROWS_NOTHING( T24 t = T24( s ) );
            T24 t = T24( s );

            TS_ASSERT_EQUALS( t[ idx ], 3.0 );

            idx[ 0 ] = 3;
            idx[ 1 ] = 0;

            TS_ASSERT_EQUALS( t[ idx ], -2.0 );

            idx[ 0 ] = 1;

            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
        }
        // order = 1 is kind of a special case, as there is only one "permutation" of a single index
        {
            S14 s;

            std::size_t idx[ 1 ] = { 0 };

            s[ idx ] = -2.0;

            idx[ 0 ] = 3;

            s[ idx ] = 3.0;

            TS_ASSERT_THROWS_NOTHING( T14 t = T14( s ) );
            T14 t = T14( s );

            TS_ASSERT_EQUALS( t[ idx ], 3.0 );
            idx[ 0 ] = 0;
            TS_ASSERT_EQUALS( t[ idx ], -2.0 );
            idx[ 0 ] = 1;
            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
            idx[ 0 ] = 2;
            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
        }
        // now test the order = 0 version
        {
            S04 s;

            std::size_t* idx = NULL;
            s[ idx ] = 5.0;

            TS_ASSERT_THROWS_NOTHING( T04( s ) );
            T04 t = T04( s );

            TS_ASSERT_EQUALS( t[ idx ], 5.0 );
        }
    }

    /**
     * Test assignment of a WTensorBaseSym to a WTensorBase.
     */
    void testCopyOperatorBaseFromSym()
    {
        // same test as the last one, only this time we use the copy operator
        typedef WTensorBaseSym< 2, 4, double > S24;
        typedef WTensorBase< 2, 4, double > T24;
        typedef WTensorBaseSym< 1, 4, double > S14;
        typedef WTensorBase< 1, 4, double > T14;
        typedef WTensorBaseSym< 0, 4, double > S04;
        typedef WTensorBase< 0, 4, double > T04;

        {
            S24 s;
            T24 t;

            std::size_t idx[ 2 ] = { 0, 3 };

            s[ idx ] = -2.0;

            idx[ 0 ] = 3;
            idx[ 1 ] = 2;

            s[ idx ] = 3.0;

            TS_ASSERT_THROWS_NOTHING( t = s );
            t = s;

            TS_ASSERT_EQUALS( t[ idx ], 3.0 );

            idx[ 0 ] = 3;
            idx[ 1 ] = 0;

            TS_ASSERT_EQUALS( t[ idx ], -2.0 );

            idx[ 0 ] = 1;

            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
        }
        // order = 1 is kind of a special case, as there is only one "permutation" of a single index
        {
            S14 s;
            T14 t;

            std::size_t idx[ 1 ] = { 0 };

            s[ idx ] = -2.0;

            idx[ 0 ] = 3;

            s[ idx ] = 3.0;

            TS_ASSERT_THROWS_NOTHING( t = s );
            t = s;

            TS_ASSERT_EQUALS( t[ idx ], 3.0 );
            idx[ 0 ] = 0;
            TS_ASSERT_EQUALS( t[ idx ], -2.0 );
            idx[ 0 ] = 1;
            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
            idx[ 0 ] = 2;
            TS_ASSERT_EQUALS( t[ idx ], 0.0 );
        }
        // now test the order = 0 version
        {
            S04 s;
            T04 t;

            std::size_t* idx = NULL;
            s[ idx ] = 5.0;

            TS_ASSERT_THROWS_NOTHING( t = s );
            t = s;

            TS_ASSERT_EQUALS( t[ idx ], 5.0 );
        }
    }

    /**
     * Test casts from any tensorbase of order 0 to a value.
     */
    void testCastTensorToValue()
    {
        // create some "scalar tensors" and cast them to their respective Data_T
        {
            // types
            WTensorFunc< WTensorBase, 0, 1, double > td;
            WTensorFunc< WTensorBase, 0, 0, float > tf;
            WTensorFunc< WTensorBase, 0, 456, int > ti;

            // implicitly cast to Data_T
            td() = 3.0;
            double d = td;
            TS_ASSERT_EQUALS( d, 3.0 );

            tf() = 3.0f;
            float f = tf;
            TS_ASSERT_EQUALS( f, 3.0f );

            ti() = 3;
            int i = ti;
            TS_ASSERT_EQUALS( i, 3 );
        }
        // do the same test with symmetric tensors
        {
            WTensorFunc< WTensorBase, 0, 1, double > td;
            WTensorFunc< WTensorBase, 0, 0, float > tf;
            WTensorFunc< WTensorBase, 0, 456, int > ti;

            td() = 3.0;
            double d = td;
            TS_ASSERT_EQUALS( d, 3.0 );

            tf() = 3.0f;
            float f = tf;
            TS_ASSERT_EQUALS( f, 3.0f );

            ti() = 3;
            int i = ti;
            TS_ASSERT_EQUALS( i, 3 );
        }
    }

    /**
     * Test casts from any tensorbase of order 1 to a WValue.
     */
    void testCastTensorToVector()
    {
        {
            WTensorFunc< WTensorBase, 1, 5, double > t;
            t( 0 ) = -9.765;
            t( 1 ) = 154.06;
            t( 4 ) = -57.0;

            WValue< double > v = t;

            TS_ASSERT_EQUALS( v.size(), 5 );

            TS_ASSERT_EQUALS( v[ 0 ], -9.765 );
            TS_ASSERT_EQUALS( v[ 1 ], 154.06 );
            TS_ASSERT_EQUALS( v[ 2 ], 0.0 );
            TS_ASSERT_EQUALS( v[ 3 ], 0.0 );
            TS_ASSERT_EQUALS( v[ 4 ], -57.0 );
        }
        {
            WTensorFunc< WTensorBaseSym, 1, 5, double > t;
            t( 0 ) = -9.765;
            t( 1 ) = 154.06;
            t( 4 ) = -57.0;

            WValue< double > v = t;

            TS_ASSERT_EQUALS( v.size(), 5 );

            TS_ASSERT_EQUALS( v[ 0 ], -9.765 );
            TS_ASSERT_EQUALS( v[ 1 ], 154.06 );
            TS_ASSERT_EQUALS( v[ 2 ], 0.0 );
            TS_ASSERT_EQUALS( v[ 3 ], 0.0 );
            TS_ASSERT_EQUALS( v[ 4 ], -57.0 );
        }
    }

    /**
     * Test casts from any tensorbase of order 2 to a WMatrix.
     */
    void testCastTensorToMatrix()
    {
        {
            WTensorFunc< WTensorBase, 2, 3, double > t;
            t( 0, 0 ) = -9.765;
            t( 1, 0 ) = 154.06;
            t( 2, 2 ) = -57.0;

            WMatrix< double > m = t;

            TS_ASSERT_EQUALS( m.getNbCols(), 3 );
            TS_ASSERT_EQUALS( m.getNbRows(), 3 );

            for( std::size_t i = 0; i < 3; ++i )
            {
                for( std::size_t j = 0; j < 3; ++j )
                {
                    TS_ASSERT_EQUALS( m( i, j ), t( i, j ) );
                }
            }
        }
        {
            WTensorFunc< WTensorBaseSym, 2, 3, double > t;
            t( 0, 0 ) = -9.765;
            t( 1, 0 ) = 154.06;
            t( 2, 2 ) = -57.0;

            WMatrix< double > m = t;

            TS_ASSERT_EQUALS( m.getNbCols(), 3 );
            TS_ASSERT_EQUALS( m.getNbRows(), 3 );

            for( std::size_t i = 0; i < 3; ++i )
            {
                for( std::size_t j = 0; j < 3; ++j )
                {
                    TS_ASSERT_EQUALS( m( i, j ), t( i, j ) );
                }
            }
        }
    }
};

/**
 * Test some utility functions.
 */
class WTensorUtilityTest : public CxxTest::TestSuite
{
public:
    // the functions testet here are needed for the initialization of the
    // index permutation to array position mapping
    // note that these functions do not check for errors and are meant for internal use
    /**
     * Test iteration of indices.
     */
    void testIndexIteration()
    {
        std::vector< std::size_t > is( 3, 0 );   // is == ( 0, 0, 0 )
        std::vector< std::size_t > shouldBe( 3, 0 );
        shouldBe[ 2 ] = 1; // shouldBe == ( 0, 0, 1 )

        positionIterateOneStep< 3, 3 >( is );
        TS_ASSERT_SAME_DATA( &is[ 0 ], &shouldBe[ 0 ], 3 * sizeof( std::size_t ) );

        positionIterateOneStep< 3, 3 >( is );
        positionIterateOneStep< 3, 3 >( is );
        shouldBe[ 1 ] = 1;
        shouldBe[ 2 ] = 0; // shouldBe == ( 0, 1, 0 )

        TS_ASSERT_SAME_DATA( &is[ 0 ], &shouldBe[ 0 ], 3 * sizeof( std::size_t ) );

        // the dim = 2 case is more interesting
        positionIterateOneStep< 3, 2 >( is );
        positionIterateOneStep< 3, 2 >( is );
        shouldBe[ 0 ] = 1;
        shouldBe[ 1 ] = 0; // shouldBe == ( 1, 0, 0 )

        TS_ASSERT_SAME_DATA( &is[ 0 ], &shouldBe[ 0 ], 3 * sizeof( std::size_t ) );

        positionIterateOneStep< 3, 2 >( is );
        shouldBe[ 2 ] = 1;

        TS_ASSERT_SAME_DATA( &is[ 0 ], &shouldBe[ 0 ], 3 * sizeof( std::size_t ) );
    }

    /**
     * Test sorted iteration of indices.
     */
    void testIndexIterationSorted()
    {
        std::vector< std::size_t > v( 3, 0 );
        std::size_t numIter = WBinom< 5, 3 >::value - 1;

        // the indices should always be sorted
        for( std::size_t k = 0; k < numIter; ++k )
        {
            positionIterateSortedOneStep< 3, 3 >( v );
            TS_ASSERT( v[ 0 ] <= v[ 1 ] );
            TS_ASSERT( v[ 1 ] <= v[ 2 ] );
        }

        //after iterating numIter times, v should be ( 2, 2, 2 )
        TS_ASSERT_EQUALS( v[ 0 ], 2 );
        TS_ASSERT_EQUALS( v[ 1 ], 2 );
        TS_ASSERT_EQUALS( v[ 2 ], 2 );

        // now test the dim = 2 case
        v[ 0 ] = v[ 1 ] = v[ 2 ] = 0;
        numIter = WBinom< 4, 3 >::value - 1;

        // the indices should always be sorted
        for( std::size_t k = 0; k < numIter; ++k )
        {
            positionIterateSortedOneStep< 3, 2 >( v );
            TS_ASSERT( v[ 0 ] <= v[ 1 ] );
            TS_ASSERT( v[ 1 ] <= v[ 2 ] );
        }

        //after iterating numIter times, v should be ( 1, 1, 1 )
        TS_ASSERT_EQUALS( v[ 0 ], 1 );
        TS_ASSERT_EQUALS( v[ 1 ], 1 );
        TS_ASSERT_EQUALS( v[ 2 ], 1 );
    }
};

// restore WASSERT_AS_CASSERT flag
#ifdef WASSERT_FLAG_CHANGED
#define WASSERT_AS_CASSERT
#undefine WASSERT_FLAG_CHANGED
#endif

#endif  // WTENSORBASE_TEST_H
