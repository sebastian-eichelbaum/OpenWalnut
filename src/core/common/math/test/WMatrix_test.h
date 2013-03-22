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

#ifndef WMATRIX_TEST_H
#define WMATRIX_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WMatrix.h"

/**
 * Tests for WMatrix.
 */
class WMatrixTest : public CxxTest::TestSuite
{
public:
    /**
     * Instantiation should throw nothing.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WMatrix< double > matrix( 3, 2 ) );
        TS_ASSERT_THROWS_NOTHING( WMatrix< float > matrix( 3, 2 ) );
    }

    /**
     * Instantiation with copy constructor should throw nothing.
     */
    void testCopyInstantiation( void )
    {
        WMatrix< double > matrix( 3, 2 );
        TS_ASSERT_THROWS_NOTHING( WMatrix< double > matrix2( matrix ) );
    }

    /**
     * Number of rows and columns should be return correctly.
     */
    void testGetNbRowsAndCols( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        WMatrix< double > matrix( nbRows, nbCols );
        TS_ASSERT_EQUALS( matrix.getNbRows(), nbRows );
        TS_ASSERT_EQUALS( matrix.getNbCols(), nbCols );
    }

    /**
     * Element access operator should work correctly.
     */
    void testElementAccessOperator( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        WMatrix< double > matrix( nbRows, nbCols );
        TS_ASSERT_EQUALS( matrix( 0 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 0 , 1 ), 0. );
        TS_ASSERT_EQUALS( matrix( 1 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 1 , 1 ), 0. );
        TS_ASSERT_EQUALS( matrix( 2 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 2 , 1 ), 0. );

        const double a = 3.14;
        matrix( 2, 1 ) = a;
        TS_ASSERT_EQUALS( matrix( 2, 1 ), a );
    }

    /**
     * Constant element access operator should work correctly.
     */
    void testConstElementAccessOperator( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        const WMatrix< double > matrix( nbRows, nbCols );
        TS_ASSERT_EQUALS( matrix( 0 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 0 , 1 ), 0. );
        TS_ASSERT_EQUALS( matrix( 1 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 1 , 1 ), 0. );
        TS_ASSERT_EQUALS( matrix( 2 , 0 ), 0. );
        TS_ASSERT_EQUALS( matrix( 2 , 1 ), 0. );
    }

    /**
     * Test for equality comparison of two matrices
     */
    void testEqualityOperator( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        const double a = 1.2, b = 2.3, c = 3.4, d = 4.5, e = 5.6, f = 6.7;
        WMatrix< double > matrix1( nbRows, nbCols );
        WMatrix< double > matrix2( nbRows, nbCols );
        WMatrix< double > matrix3( nbCols, nbRows );

        matrix1( 0, 0 ) = a;
        matrix1( 0, 1 ) = b;
        matrix1( 1, 0 ) = c;
        matrix1( 1, 1 ) = d;
        matrix1( 2, 0 ) = e;
        matrix1( 2, 1 ) = f;

        matrix2( 0, 0 ) = a;
        matrix2( 0, 1 ) = b;
        matrix2( 1, 0 ) = c;
        matrix2( 1, 1 ) = d;
        matrix2( 2, 0 ) = e;
        matrix2( 2, 1 ) = f;

        matrix3( 0, 0 ) = a;
        matrix3( 0, 1 ) = b;
        matrix3( 0, 2 ) = c;
        matrix3( 1, 0 ) = d;
        matrix3( 1, 1 ) = e;
        matrix3( 1, 2 ) = f;

        TS_ASSERT_EQUALS( matrix1 == matrix2, true );
        TS_ASSERT_EQUALS( matrix1 == matrix3, false );

        matrix2( 0, 0 ) += 1.;

        TS_ASSERT_EQUALS( matrix1 == matrix2, false );
    }

    /**
     * Test for inequality comparison of two matrices
     */
    void testInequalityOperator( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        const double a = 1.2, b = 2.3, c = 3.4, d = 4.5, e = 5.6, f = 6.7;
        WMatrix< double > matrix1( nbRows, nbCols );
        WMatrix< double > matrix2( nbRows, nbCols );
        WMatrix< double > matrix3( nbCols, nbRows );

        matrix1( 0, 0 ) = a;
        matrix1( 0, 1 ) = b;
        matrix1( 1, 0 ) = c;
        matrix1( 1, 1 ) = d;
        matrix1( 2, 0 ) = e;
        matrix1( 2, 1 ) = f;

        matrix2( 0, 0 ) = a;
        matrix2( 0, 1 ) = b;
        matrix2( 1, 0 ) = c;
        matrix2( 1, 1 ) = d;
        matrix2( 2, 0 ) = e;
        matrix2( 2, 1 ) = f;

        matrix3( 0, 0 ) = a;
        matrix3( 0, 1 ) = b;
        matrix3( 0, 2 ) = c;
        matrix3( 1, 0 ) = d;
        matrix3( 1, 1 ) = e;
        matrix3( 1, 2 ) = f;

        TS_ASSERT_EQUALS( matrix1 != matrix2, false );
        TS_ASSERT_EQUALS( matrix1 != matrix3, true );

        matrix2( 0, 0 ) += 1.;

        TS_ASSERT_EQUALS( matrix1 != matrix2, true );
    }

    /**
     * Test assignment operator of WMatrix
     */
    void testAssignmentOperator( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        const double a = 1.2, b = 2.3, c = 3.4, d = 4.5, e = 5.6, f = 6.7;
        WMatrix< double > matrix1( nbRows, nbCols );
        WMatrix< double > matrix2( nbRows, nbCols );

        matrix1( 0, 0 ) = a;
        matrix1( 0, 1 ) = b;
        matrix1( 1, 0 ) = c;
        matrix1( 1, 1 ) = d;
        matrix1( 2, 0 ) = e;
        matrix1( 2, 1 ) = f;

        matrix2( 0, 0 ) = a + 1.;
        matrix2( 0, 1 ) = b + 2.;
        matrix2( 1, 0 ) = c + 3.;
        matrix2( 1, 1 ) = d + 4.;
        matrix2( 2, 0 ) = e + 5.;
        matrix2( 2, 1 ) = f + 6.;

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( matrix1 == matrix2, false );

        // test simple assignment
        matrix1 = matrix2;
        TS_ASSERT_EQUALS( matrix1 == matrix2, true );

        WMatrix< double > matrix3( nbRows, nbCols );
        WMatrix< double > matrix4( nbRows, nbCols );

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( matrix2 == matrix3, false );
        TS_ASSERT_EQUALS( matrix2 == matrix4, false );

        // test whether return the reference to self works
        // for multiple assignment
        matrix4 = matrix3 = matrix2;
        TS_ASSERT_EQUALS( matrix2 == matrix3, true );
        TS_ASSERT_EQUALS( matrix2 == matrix4, true );
        TS_ASSERT_EQUALS( matrix3 == matrix4, true );
    }

    /**
     * Test transposed method of WMatrix
     */
    void testTransposed( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        WMatrix< int > matrix( nbRows, nbCols );

        for( size_t row = 0; row < nbRows; row++ )
          for( size_t col = 0; col < nbCols; col++ )
            matrix( row, col ) = ( row+1 )*10 + col+1;

        WMatrix< int > matrixTransposed( matrix.transposed() );

        //test dimensions
        TS_ASSERT_EQUALS( matrixTransposed.getNbCols(), matrix.getNbRows() );
        TS_ASSERT_EQUALS( matrixTransposed.getNbRows(), matrix.getNbCols() );

        // test values
        for( size_t row = 0; row < nbRows; row++ )
          for( size_t col = 0; col < nbCols; col++ )
            TS_ASSERT_EQUALS( matrixTransposed( col, row ), ( row+1 )*10 + col + 1 );
    }

    /**
     * Test isIdentity method of WMatrix
     */
    void testIsIdentity( void )
    {
        WMatrix< double > a( 3, 2 );
        a.makeIdentity();
        TS_ASSERT_EQUALS( a.isIdentity(), false );

        WMatrix< double > b( 3, 3 );
        b.makeIdentity();
        TS_ASSERT_EQUALS( b.isIdentity(), true );

        b( 0, 0 ) += 1e-3;
        TS_ASSERT_EQUALS( b.isIdentity(), false );
        TS_ASSERT_EQUALS( b.isIdentity( 1e-3 ), true );

        b( 0, 1 ) += 2e-3;
        TS_ASSERT_EQUALS( b.isIdentity( 1e-3 ), false );
        TS_ASSERT_EQUALS( b.isIdentity( 2e-3 ), true );
    }
};

#endif  // WMATRIX_TEST_H
