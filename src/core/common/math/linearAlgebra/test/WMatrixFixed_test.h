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

#ifndef WMATRIXFIXED_TEST_H
#define WMATRIXFIXED_TEST_H

#include <cxxtest/TestSuite.h>

#include <boost/array.hpp>

#include "../WMatrixFixed.h"
#include "../WVectorFixed.h"

/**
 * Tests for WMatrixFixed.
 */
class WMatrixFixedTest : public CxxTest::TestSuite
{
public:
    /**
     * Instantiation should throw nothing.
     */
    void testInstantiation( void )
    {
        typedef WMatrixFixed< double, 3, 2 > WMD32;
        typedef WMatrixFixed< float, 3, 2 > WMF32;
        typedef WMatrixFixed< double, 1, 1 > WMD11;
        typedef WMatrixFixed< float, 1, 1 > WMF11;
        typedef WMatrixFixed< double, 4, 4 > WMD44;
        typedef WMatrixFixed< float, 4, 4 > WMF44;
        typedef WMatrixFixed< int, 3, 2 > WMI32;
        typedef WMatrixFixed< std::size_t, 3, 2 > WMS32;

        TS_ASSERT_THROWS_NOTHING( WMD32 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMF32 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMD11 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMF11 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMD44 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMF44 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMI32 matrix() );
        TS_ASSERT_THROWS_NOTHING( WMS32 matrix() );
    }

    /**
     * Instantiation with copy constructor should throw nothing.
     */
    void testCopyInstantiation( void )
    {
        typedef WMatrixFixed< double, 3, 2 > WMD32;
        WMD32 matrix;
        TS_ASSERT_THROWS_NOTHING( WMD32 matrix2( matrix ) );

        typedef WMatrixFixed< double, 1, 1 > WMD11;
        WMD11 scalar;
        TS_ASSERT_THROWS_NOTHING( WMD11 scalar2( scalar ) );

        typedef WMatrixFixed< double, 1, 3 > WMD13;
        WMD13 vector;
        TS_ASSERT_THROWS_NOTHING( WMD13 vector2( vector ) );

        // access operator is tested in another place
        WMatrixFixed< int, 3, 3 > mat;
        mat( 0, 0 ) = 1;
        mat( 0, 1 ) = 2;
        mat( 0, 2 ) = 3;
        mat( 1, 0 ) = 4;
        mat( 1, 1 ) = 5;
        mat( 1, 2 ) = 6;
        mat( 2, 0 ) = 7;
        mat( 2, 1 ) = 8;
        mat( 2, 2 ) = 9;

        WMatrixFixed< int, 3, 3 > mat2( mat );
        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( mat( i, j ), mat2( i, j ) );
            }
        }
    }

    /**
     * Number of rows and columns should be returned correctly.
     */
    void testGetNbRowsAndCols( void )
    {
        const size_t nbRows = 3, nbCols = 2;
        WMatrixFixed< double, 3, 2 > matrix;
        TS_ASSERT_EQUALS( matrix.getRows(), nbRows );
        TS_ASSERT_EQUALS( matrix.getColumns(), nbCols );
    }

    /**
     * Tests the access operator for the standard storage type. Row major storage is assumed.
     */
    void testAccessOperator()
    {
        WMatrixFixed< int, 3, 3 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;

        for( int i = 0; i < 9; ++i )
        {
            TS_ASSERT_EQUALS( matrix.m_values.m_values[ i ], i + 1 );
        }
    }

    /**
     * Check if at() correctly checks for out of bounds indices and returns
     * the same values as operator ().
     */
    void testAt()
    {
        WMatrixFixed< int, 3, 4 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;

        matrix( 0, 3 ) = 10;
        matrix( 1, 3 ) = 11;
        matrix( 2, 3 ) = 12;

        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t j = 0; j < 4; ++j )
            {
                TS_ASSERT_EQUALS( matrix( i, j ), matrix.at( i, j ) );
            }
        }

        TS_ASSERT_THROWS( matrix.at( 0, 4 ), WOutOfBounds );
        TS_ASSERT_THROWS( matrix.at( 1, 5 ), WOutOfBounds );
        TS_ASSERT_THROWS( matrix.at( 1, 4 ), WOutOfBounds );
        TS_ASSERT_THROWS( matrix.at( 1, 100000 ), WOutOfBounds );
        TS_ASSERT_THROWS( matrix.at( 3, 1 ), WOutOfBounds );
        TS_ASSERT_THROWS( matrix.at( -1, 0 ), WOutOfBounds );
    }

    /**
     * Check if getRowVector() returns the correct contents.
     */
    void testGetRowVector()
    {
        WMatrixFixed< int, 3, 3 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;

        WMatrixFixed< int, 3, 1 > rowVector;
        rowVector( 0, 0 ) = matrix( 0, 0 );
        rowVector( 1, 0 ) = matrix( 0, 1 );
        rowVector( 2, 0 ) = matrix( 0, 2 );

        TS_ASSERT_EQUALS( matrix.getRowVector( 0 )( 0, 0 ),  rowVector( 0, 0 ) );
        TS_ASSERT_EQUALS( matrix.getRowVector( 0 )( 0, 1 ),  rowVector( 1, 0 ) );
        TS_ASSERT_EQUALS( matrix.getRowVector( 0 )( 0, 2 ),  rowVector( 2, 0 ) );
    }

    /**
     * Check if getColumnVector() returns the correct contents.
     */
    void testGetColVector()
    {
        WMatrixFixed< int, 3, 3 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;

        WMatrixFixed< int, 3, 1 > colVector;
        colVector( 0, 0 ) = matrix( 0, 1 );
        colVector( 1, 0 ) = matrix( 1, 1 );
        colVector( 2, 0 ) = matrix( 2, 1 );

        TS_ASSERT_EQUALS( matrix.getColumnVector( 1 )( 0, 0 ), colVector.at( 0, 0 ) );
        TS_ASSERT_EQUALS( matrix.getColumnVector( 1 )( 1, 0 ), colVector.at( 1, 0 ) );
        TS_ASSERT_EQUALS( matrix.getColumnVector( 1 )( 2, 0 ), colVector.at( 2, 0 ) );
    }

    /**
     * Check if setRowVector() sets the matrix contents correctly.
     */
    void testSetRowVector()
    {
        WMatrixFixed< int, 3, 1 > rowVector;
        rowVector( 0, 0 ) = 1;
        rowVector( 1, 0 ) = 2;
        rowVector( 2, 0 ) = 3;

        WMatrixFixed< int, 3, 3 > matrix;
        matrix.setRowVector( 0, rowVector );

        TS_ASSERT_EQUALS( matrix( 0, 0 ), rowVector( 0, 0 ) );
        TS_ASSERT_EQUALS( matrix( 0, 1 ), rowVector( 1, 0 ) );
        TS_ASSERT_EQUALS( matrix( 0, 2 ), rowVector( 2, 0 ) );
    }

    /**
     * Check if setColumnVector() sets the matrix contents correctly.
     */
    void testSetColVector()
    {
        WMatrixFixed< int, 3, 1 > colVector;
        colVector( 0, 0 ) = 2;
        colVector( 1, 0 ) = 5;
        colVector( 2, 0 ) = 8;

        WMatrixFixed< int, 3, 3 > matrix;
        matrix.setColumnVector( 1, colVector );

        TS_ASSERT_EQUALS( matrix( 0, 1 ), colVector( 0, 0 ) );
        TS_ASSERT_EQUALS( matrix( 1, 1 ), colVector( 1, 0 ) );
        TS_ASSERT_EQUALS( matrix( 2, 1 ), colVector( 2, 0 ) );
    }

    /**
     * The zero function should return a matrix that contains only zeros.
     */
    void testZero()
    {
        typedef WMatrixFixed< double, 1, 3 > WMD13;
        TS_ASSERT_EQUALS( WMD13::zero()( 0, 0 ), 0.0 );
        TS_ASSERT_EQUALS( WMD13::zero()( 0, 1 ), 0.0 );
        TS_ASSERT_EQUALS( WMD13::zero()( 0, 2 ), 0.0 );

        typedef WMatrixFixed< unsigned char, 2, 2 > WMU32;
        TS_ASSERT_EQUALS( WMU32::zero()( 0, 0 ), 0 );
        TS_ASSERT_EQUALS( WMU32::zero()( 0, 1 ), 0 );
        TS_ASSERT_EQUALS( WMU32::zero()( 1, 0 ), 0 );
        TS_ASSERT_EQUALS( WMU32::zero()( 1, 1 ), 0 );
    }

    /**
     * Tests the identity function.
     */
    void testIdentity()
    {
        typedef WMatrixFixed< unsigned char, 4, 5 > WMU45;
        typedef WMatrixFixed< unsigned char, 5, 4 > WMU54;

        // rows < cols
        for( int i = 0; i < 4; i++ )
        {
            for( int j = 0; j < 5; ++j )
            {
                if( i == j )
                {
                    TS_ASSERT_EQUALS( WMU45::identity()( i, j ), 1 );
                }
                else
                {
                    TS_ASSERT_EQUALS( WMU45::identity()( i, j ), 0 );
                }
            }
        }

        // rows > cols
        for( int i = 0; i < 5; i++ )
        {
            for( int j = 0; j < 4; ++j )
            {
                if( i == j )
                {
                    TS_ASSERT_EQUALS( WMU54::identity()( i, j ), 1 );
                }
                else
                {
                    TS_ASSERT_EQUALS( WMU54::identity()( i, j ), 0 );
                }
            }
        }
    }

    /**
     * Assignment from matrices with matching or different integral types should work correctly.
     */
    void testAssignmentMatchingOrDifferentType()
    {
        // matching type
        {
            WMatrixFixed< double, 3, 3 > matrix2;
            matrix2 = m_matrix;
            for( std::size_t i = 0; i < 3; ++i )
            {
                for( std::size_t j = 0; j < 3; ++j )
                {
                    TS_ASSERT_EQUALS( m_matrix( i, j ), matrix2( i, j ) );
                }
            }
        }
        // differing type
        {
            WMatrixFixed< int, 3, 3 > matrix2;
            matrix2 = m_matrix;

            TS_ASSERT_EQUALS( matrix2( 0, 0 ), 1 );
            TS_ASSERT_EQUALS( matrix2( 0, 1 ), 0.0 );
            TS_ASSERT_EQUALS( matrix2( 0, 2 ), 3 );
            TS_ASSERT_EQUALS( matrix2( 1, 0 ), 4000 );
            TS_ASSERT_EQUALS( matrix2( 1, 1 ), 5 );
            TS_ASSERT_EQUALS( matrix2( 1, 2 ), -5343 );
            TS_ASSERT_EQUALS( matrix2( 2, 0 ), 1 );
            TS_ASSERT_EQUALS( matrix2( 2, 1 ), 0 );
            TS_ASSERT_EQUALS( matrix2( 2, 2 ), 0 );
        }
    }

    /**
     * A class used for a test with different data storage, we use column major order.
     */
    template< typename ValueT, size_t Rows, size_t Cols >
    class DifferentStorageType
    {
    public:
        /**
         * Returns a reference to the component of an row and column in order to provide access to the component. It does not check for validity of
         * the indices.
         *
         * \param row the row, staring with 0
         * \param col the column, starting with 0
         *
         * \return A reference to the component of an row and column.
         */
        ValueT& operator()( size_t row, size_t col ) throw()
        {
            return m_values[ row + col * Rows ];
        }

        /**
         * Returns a const reference to the component of an row and column in order to provide access to the component.
         * It does not check for validity of
         * the indices.
         *
         * \param row the row, staring with 0
         * \param col the column, starting with 0
         *
         * \return A const reference to the component of an row and column.
         */
        const ValueT& operator()( size_t row, size_t col ) const throw()
        {
            return m_values[ row + col * Rows ];
        }

        /**
         * Replaces the values in this array.
         *
         * \tparam RHSValueT the value type. This is casted to ValueT.
         * \tparam RHSValueStoreT The value store given
         * \param rhs the values to set.
         *
         * \return this
         */
        template < typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
        ValueStore< ValueT, Rows, Cols >& operator=( RHSValueStoreT< RHSValueT, Rows, Cols > const& rhs )
        {
            for( size_t row = 0; row < Rows; ++row )
            {
                for( size_t col = 0; col < Cols; ++col )
                {
                    ( row, col ) = rhs( row, col );
                }
            }
        }

        //! The value array. Stored column-major.
        // this needs to be public for testing purposes
        boost::array< ValueT, Rows * Cols > m_values;
    };

    /**
     * Assignment from matrices with different storage types should work correctly.
     */
    void testAssignmentDifferentStorage()
    {
        WMatrixFixed< double, 3, 3, DifferentStorageType > matrix;
        matrix = m_matrix;

        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 0 ], 1.52234 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 1 ], 4e3 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 2 ], 1 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 3 ], -0.4534 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 4 ], 5.666 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 5 ], 0 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 6 ], 3.0 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 7 ], -5343.959 );
        TS_ASSERT_EQUALS( matrix.m_values.m_values[ 8 ], 0.1 );

        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( matrix( i, j ), m_matrix( i, j ) );
            }
        }
    }

    /**
     * Test self-assignment.
     */
    void testSelfAssignment()
    {
        TS_ASSERT_THROWS_NOTHING( m_matrix = m_matrix );

        m_matrix = m_matrix;

        TS_ASSERT_EQUALS( m_matrix( 0, 0 ), 1.52234 );
        TS_ASSERT_EQUALS( m_matrix( 0, 1 ), -0.4534 );
        TS_ASSERT_EQUALS( m_matrix( 0, 2 ), 3.0 );
        TS_ASSERT_EQUALS( m_matrix( 1, 0 ), 4e3 );
        TS_ASSERT_EQUALS( m_matrix( 1, 1 ), 5.666 );
        TS_ASSERT_EQUALS( m_matrix( 1, 2 ), -5343.959 );
        TS_ASSERT_EQUALS( m_matrix( 2, 0 ), 1 );
        TS_ASSERT_EQUALS( m_matrix( 2, 1 ), 0 );
        TS_ASSERT_EQUALS( m_matrix( 2, 2 ), 0.1 );
    }

    /**
     * Matrices should be converted to eigen3 matrices correctly.
     * Conversion to eigen3 and re-conversion to WMatrix should yield the original matrix.
     */
    void testEigen3Coversion()
    {
        Eigen::Matrix< double, 3, 3 > emat = m_matrix;
        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( emat( i, j ), m_matrix( i, j ) );
            }
        }

        WMatrixFixed< double, 3, 3 > matrix2( emat );
        for( std::size_t i = 0; i < 3; ++i )
        {
            for( std::size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( matrix2( i, j ), m_matrix( i, j ) );
            }
        }
    }

    /**
     * Test conversion between several matrix types
     */
    void testConversion()
    {
        WMatrixFixed< double, 2, 2 > md;
        md( 0, 0 ) = 0.0;
        md( 1, 0 ) = 1.0;
        md( 0, 1 ) = 2.0;
        md( 1, 1 ) = 3.0;
        WMatrixFixed< int, 2, 2 > mi( md );

        TS_ASSERT( mi( 0, 0 ) == 0 );
        TS_ASSERT( mi( 1, 0 ) == 1 );
        TS_ASSERT( mi( 0, 1 ) == 2 );
        TS_ASSERT( mi( 1, 1 ) == 3 );
    }

    /**
     * Test matrix multiplication.
     */
    void testMatrixMultiplication()
    {
        // note we do not need to check for matching number of rows/columns as this is done by the compiler
        typedef WMatrixFixed< int, 3, 4 > WMI34;
        WMI34 matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 0, 3 ) = -3;
        matrix( 1, 0 ) = 2;
        matrix( 1, 1 ) = -5;
        matrix( 1, 2 ) = 0;
        matrix( 1, 3 ) = 9;
        matrix( 2, 0 ) = 0;
        matrix( 2, 1 ) = 1;
        matrix( 2, 2 ) = 1;
        matrix( 2, 3 ) = 2;

        // matrix-vector
        {
            typedef WMatrixFixed< int, 4, 1 > WMI41;
            WMI41 vec;
            vec[ 0 ] = -1;
            vec[ 1 ] = 2;
            vec[ 2 ] = 0;
            vec[ 3 ] = 1;

            typedef WMatrixFixed< int, 4, 4 > WMI44;
            TS_ASSERT_EQUALS( WMI44::identity() * vec, vec );
            TS_ASSERT_EQUALS( WMI44::zero() * vec, WMI41::zero() );

            WMatrixFixed< int, 3, 1 > res = matrix * vec;

            TS_ASSERT_EQUALS( res[ 0 ], 0 );
            TS_ASSERT_EQUALS( res[ 1 ], -3 );
            TS_ASSERT_EQUALS( res[ 2 ], 4 );
        }

        // matrix-matrix
        {
            typedef WMatrixFixed< int, 4, 4 > WMI44;

            TS_ASSERT_EQUALS( WMI44::zero() * WMI44::zero(), WMI44::zero() );
            TS_ASSERT_EQUALS( WMI44::zero() * WMI44::identity(), WMI44::zero() );
            TS_ASSERT_EQUALS( WMI44::identity() * WMI44::zero(), WMI44::zero() );
            TS_ASSERT_EQUALS( WMI44::identity() * WMI44::identity(), WMI44::identity() );

            TS_ASSERT_EQUALS( matrix * WMI44::identity(), matrix );
            TS_ASSERT_EQUALS( matrix * WMI44::zero(), WMI34::zero() );

            typedef WMatrixFixed< int, 3, 3 > WMI33;
            WMI33 mat;
            mat( 0, 0 ) = mat( 2, 2 ) = 1;
            mat( 1, 1 ) = 0;
            mat( 0, 1 ) = mat( 1, 0 ) = -2;
            mat( 0, 2 ) = mat( 2, 0 ) = 3;
            mat( 1, 2 ) = mat( 2, 1 ) = 2;

            WMI34 res = mat * matrix;
            TS_ASSERT_EQUALS( res( 0, 0 ), -3 );
            TS_ASSERT_EQUALS( res( 1, 2 ), -4 );
            TS_ASSERT_EQUALS( res( 2, 0 ), 7 );
            TS_ASSERT_EQUALS( res( 2, 3 ), 11 );
            TS_ASSERT_EQUALS( res( 1, 3 ), 10 );

            // special test for self-assigning multiplication of a matrix with itself
            mat *= mat;
            TS_ASSERT_EQUALS( mat( 0, 0 ), 14 );
            TS_ASSERT_EQUALS( mat( 2, 2 ), 14 );
            TS_ASSERT_EQUALS( mat( 0, 1 ), 4 );
            TS_ASSERT_EQUALS( mat( 2, 1 ), -4 );
            TS_ASSERT_EQUALS( mat( 1, 2 ), -4 );
        }
    }

    /**
     * Matrix-scalar multiplication.
     */
    void testMatrixTimesScalar()
    {
        WMatrix3d mat = m_matrix * 2.0;

        for( int i = 0; i < 3; i++ )
        {
            for( int j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( mat( i, j ), 2 * m_matrix( i, j ) );
            }
        }

        mat *= 2;
        for( int i = 0; i < 3; i++ )
        {
            for( int j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( mat( i, j ), 4 * m_matrix( i, j ) );
            }
        }
    }

    /**
     * Matrix addition and subtraction.
     */
    void testMatrixAdditionAndSubtraction()
    {
        WMatrixFixed< int, 3, 4 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;
        matrix( 0, 3 ) = 10;
        matrix( 1, 3 ) = 11;
        matrix( 2, 3 ) = 12;

        WMatrixFixed< int, 3, 4 > mat = matrix + matrix;

        TS_ASSERT_EQUALS( mat, matrix * 2 );
        TS_ASSERT_EQUALS( mat - matrix, matrix );
    }

    /**
     * Test the dot product.
     */
    void testDot()
    {
        typedef WMatrixFixed< int, 6, 1 > WMI61;

        WMI61 v;
        v[ 0 ] = 0;
        v[ 1 ] = 1;
        v[ 2 ] = 2;
        v[ 3 ] = 4;
        v[ 4 ] = 1;
        v[ 5 ] = 2;

        WMI61 w;
        w[ 0 ] = 73;
        w[ 1 ] = 1;
        w[ 2 ] = 1;
        w[ 3 ] = 1;
        w[ 4 ] = 5;
        w[ 5 ] = 6;

        int i = dot( v, w );
        WMatrixFixed< int, 1, 1 > j = transpose( v ) * w;

        TS_ASSERT_EQUALS( i, 24 );
        TS_ASSERT_EQUALS( i, j( 0, 0 ) );
    }

    /**
     * Test vector length.
     */
    void testLength()
    {
        WVector3d vec;
        vec[ 0 ] = 0.0;
        vec[ 1 ] = 4.0;
        vec[ 2 ] = 3.0;

        TS_ASSERT_DELTA( length( vec ), 5.0, 1e-10 );
        TS_ASSERT_DELTA( length( transpose( vec ) ), 5.0, 1e-10 );

        vec[ 0 ] = 1.0;
        vec[ 1 ] = 1.0;
        vec[ 2 ] = 1.0;

        TS_ASSERT_DELTA( length( vec ), sqrt( 3.0 ), 1e-10 );
        TS_ASSERT_DELTA( length( transpose( vec ) ), sqrt( 3.0 ), 1e-10 );
    }

    /**
     * Test vector distance.
     */
    void testDistance()
    {
        WVector3d vec1;
        vec1[ 0 ] = 0.0;
        vec1[ 1 ] = 4.0;
        vec1[ 2 ] = 3.0;

        WVector3d vec2;
        vec2[ 0 ] = 0.0;
        vec2[ 1 ] = 0.0;
        vec2[ 2 ] = 0.0;

        TS_ASSERT_DELTA( distance( vec1, vec2 ), 5.0, 1e-10 );
        TS_ASSERT_DELTA( distance( transpose( vec1 ), transpose( vec2 ) ), 5.0, 1e-10 );

        vec1[ 0 ] = 0.0;
        vec1[ 1 ] = 4.0;
        vec1[ 2 ] = 3.0;

        vec2[ 0 ] = 0.0;
        vec2[ 1 ] = 1.0;
        vec2[ 2 ] = 4.0;

        TS_ASSERT_DELTA( distance( vec1, vec2 ), sqrt( 10.0 ), 1e-10 );
        TS_ASSERT_DELTA( distance( transpose( vec1 ), transpose( vec2 ) ), sqrt( 10.0 ), 1e-10 );
    }

    /**
     * Test vector normalization.
     */
    void testNormalize()
    {
        WVector3d vec;

        vec[ 0 ] = 2.0;
        vec[ 1 ] = 0.0;
        vec[ 2 ] = 0.0;

        TS_ASSERT_EQUALS( normalize( vec )[ 0 ], 1.0 );
        TS_ASSERT_DELTA( length( normalize( vec ) ), 1.0, 1e-10 );

        vec[ 0 ] = -3.0;
        vec[ 1 ] = 1.0;
        vec[ 2 ] = 5.0;

        TS_ASSERT_DELTA( length( normalize( vec ) ), 1.0, 1e-10 );
    }

    /**
     * Test matrix inversion.
     */
    void testMatrixInverse()
    {
        WMatrix3d matrix = invert( m_matrix );
        matrix *= m_matrix;
        for( int i = 0; i < 3; i++ )
        {
            for( int j = 0; j < 3; ++j )
            {
                if( i == j )
                {
                    TS_ASSERT_DELTA( WMatrix3d::identity()( i, j ), 1, 1e-10 );
                }
                else
                {
                    TS_ASSERT_DELTA( WMatrix3d::identity()( i, j ), 0, 1e-10 );
                }
            }
        }

        typedef WMatrixFixed< float, 1, 1 > WMF11;
        WMF11 mat;
        mat( 0, 0 ) = 2.0f;

        WMF11 mat2 = invert( mat );
        TS_ASSERT_EQUALS( mat2( 0, 0 ), 0.5f );
    }

    /**
     * Test for equality comparison of two matrices.
     */
    void testEqualityOperators()
    {
        WMatrixFixed< double, 3, 3 > matrix = m_matrix;

        TS_ASSERT( matrix == m_matrix );
        TS_ASSERT( ( matrix != m_matrix ) == false );

        m_matrix( 0, 0 ) += 0.1;

        TS_ASSERT( matrix != m_matrix );
        TS_ASSERT( ( matrix == m_matrix ) == false );

        m_matrix( 1, 1 ) += 0.1;

        TS_ASSERT( matrix != m_matrix );
        TS_ASSERT( ( matrix == m_matrix ) == false );

        m_matrix( 0, 0 ) -= 0.1;

        TS_ASSERT( matrix != m_matrix );
        TS_ASSERT( ( matrix == m_matrix ) == false );
    }

    /**
     * Test transpose method.
     */
    void testTranspose()
    {
        {
            WMatrixFixed< double, 3, 3 > mat = transpose( m_matrix );
            for( std::size_t i = 0; i < 3; ++i )
            {
                for( std::size_t j = 0; j < 3; ++j )
                {
                    TS_ASSERT_EQUALS( mat.at( j, i ), m_matrix.at( i, j ) );
                }
            }
        }
        {
            WMatrixFixed< int, 3, 4 > matrix;
            matrix( 0, 0 ) = 1;
            matrix( 0, 1 ) = 2;
            matrix( 0, 2 ) = 3;
            matrix( 1, 0 ) = 4;
            matrix( 1, 1 ) = 5;
            matrix( 1, 2 ) = 6;
            matrix( 2, 0 ) = 7;
            matrix( 2, 1 ) = 8;
            matrix( 2, 2 ) = 9;
            matrix( 0, 3 ) = 10;
            matrix( 1, 3 ) = 11;
            matrix( 2, 3 ) = 12;

            WMatrixFixed< int, 4, 3 > mat = transpose( matrix );
            for( std::size_t i = 0; i < 3; ++i )
            {
                for( std::size_t j = 0; j < 4; ++j )
                {
                    TS_ASSERT_EQUALS( mat.at( j, i ), matrix.at( i, j ) );
                }
            }
        }

        TS_ASSERT_EQUALS( transpose( transpose( m_matrix ) ), m_matrix );
    }

    /**
     * Test stream operators.
     */
    void testStreamOperators()
    {
        WMatrixFixed< int, 3, 4 > matrix;
        matrix( 0, 0 ) = 1;
        matrix( 0, 1 ) = 2;
        matrix( 0, 2 ) = 3;
        matrix( 1, 0 ) = 4;
        matrix( 1, 1 ) = 5;
        matrix( 1, 2 ) = 6;
        matrix( 2, 0 ) = 7;
        matrix( 2, 1 ) = 8;
        matrix( 2, 2 ) = 9;
        matrix( 0, 3 ) = 10;
        matrix( 1, 3 ) = 11;
        matrix( 2, 3 ) = 12;

        std::stringstream s;

        s << matrix;

        TS_ASSERT_EQUALS( s.str(), "1;2;3;10;4;5;6;11;7;8;9;12;" );

        WMatrixFixed< int, 3, 4 > matrix2;
        s >> matrix2;

        TS_ASSERT_EQUALS( matrix, matrix2 );
    }

private:
    /**
     * Set up a matrix used for a lot of tests.
     */
    void setUp()
    {
        m_matrix( 0, 0 ) = 1.52234;
        m_matrix( 0, 1 ) = -0.4534;
        m_matrix( 0, 2 ) = 3.0;
        m_matrix( 1, 0 ) = 4e3;
        m_matrix( 1, 1 ) = 5.666;
        m_matrix( 1, 2 ) = -5343.959;
        m_matrix( 2, 0 ) = 1;
        m_matrix( 2, 1 ) = 0;
        m_matrix( 2, 2 ) = 0.1;
    }

    //! A matrix used for a lot of tests.
    WMatrixFixed< double, 3, 3 > m_matrix;
};

#endif  // WMATRIXFIXED_TEST_H
