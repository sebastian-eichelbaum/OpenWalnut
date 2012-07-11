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

#ifndef WMATRIXFIXED_H
#define WMATRIXFIXED_H

#include <string>
#include <algorithm>

#include <boost/static_assert.hpp>
#include <boost/tokenizer.hpp>

// Needed for conversion: OSG Types
#include <osg/Vec3>
#include <osg/Vec2d>
#include <osg/Vec2f>
#include <osg/Vec3d>
#include <osg/Vec3f>
#include <osg/Vec4d>
#include <osg/Vec4f>
#include <osg/Matrixd>

// Needed for conversion: Eigen3 Types
#include <Eigen/Core>
#include <Eigen/LU>  // needed for the inverse() function

#include "../../WDefines.h"
#include "../../WStringUtils.h"
#include "../../WTypeTraits.h"

#include "../../exceptions/WOutOfBounds.h"

/**
 * Macro for handling the value store template.
 */
#define ValueStoreTemplate template< typename, size_t, size_t > class

// forward declaration for the test
class WMatrixFixedTest;

/**
 * A data store with the specified dimensions and type. The possibilities are endless. This way, you can optimize data storage for certain kinds
 * of matrices, like sparse or symmetric ones. It even allows the definition of a whole data block containing many matrices.
 *
 * \note storage is done row-major
 *
 * \tparam ValueT   the integral type
 * \tparam Rows     the number of rows
 * \tparam Cols     the number of cols
 */
template< typename ValueT, size_t Rows, size_t Cols >
class ValueStore
{
    //! the test is a friend
    friend class WMatrixFixedTest;

public:
    /**
     * Returns a reference to the component of a row and column in order to provide access to the component. It does not check for validity of
     * the indices.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     * \return A reference to the component of a row and column
     */
    ValueT& operator()( size_t row, size_t col ) throw()
    {
        return m_values[ row * Cols + col ];
    }

    /**
     * Returns a const reference to the component of an row and column in order to provide access to the component.
     * It does not check for validity of
     * the indices.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     * \return A const reference to the component of an row and column
     */
    const ValueT& operator()( size_t row, size_t col ) const throw()
    {
        return m_values[ row * Cols + col ];
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
                operator()( row, col ) = rhs( row, col );
            }
        }
    }

private:
    /**
     * The value array. Stored row-major. Never access this directly. Always use operator(). This allows us to later-on use another storing
     * order.
     */
    ValueT m_values[ Rows * Cols ];
};

/**
 * A fixed size matrix class. This is the default type in OpenWalnut. You can easily convert this matrix to and from the Eigen3 types and OSG
 * Types.
 *
 * \tparam ValueT The type of the values stored. Most of the operations, if multiple types are involved, use WTypeTraits to guess the better of
 *         both.
 * \tparam Rows Number of Rows
 * \tparam Cols Number of Columns
 * \tparam ValueStoreT The ValueStore handles the values and their access. Use special types here for a fine-grained access control or
 *         data-management
 */
template< typename ValueT, size_t Rows, size_t Cols, ValueStoreTemplate ValueStoreT = ValueStore >
class WMatrixFixed
{
    //! the test is a friend
    friend class WMatrixFixedTest;

    // this is needed for access to the storage object of another matrix
    template< typename ValueTT, size_t Rowss, size_t Colss, ValueStoreTemplate ValueStoreTT >
    friend class WMatrixFixed;

public:
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types defining this matrix
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * The integral type used in this matrix.
     */
    typedef ValueT ValueType;

    /**
     * The storage container.
     */
    typedef ValueStoreT< ValueT, Rows, Cols > ValueStoreType;

    /**
     * The whole matrix as a type for lazy programmers.
     */
    typedef WMatrixFixed< ValueT, Rows, Cols, ValueStoreT > MatrixType;

    /**
     * The number of rows.
     *
     * \return the number of rows.
     */
    size_t getRows() const
    {
        return Rows;
    }

    /**
     * The number of columns.
     *
     * \return the number of columns.
     */
    size_t getColumns() const
    {
        return Cols;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Construction and Initialization
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Default constructor. The values are initialized with 0. Use the static methods \ref zero(), \ref identity() or any of the predefined
     * transformations if an initialized matrix is wished.
     */
    WMatrixFixed()
    {
        // initialize to zero
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                operator()( row, col ) = ValueT( 0 );
            }
        }
    }

    /**
     * Constructor easing the initialization of vectors. This won't compile if Cols != 1 and Rows != 2.
     *
     * \param x x coefficient
     * \param y y coefficient
     */
    WMatrixFixed( const ValueT& x, const ValueT& y )
    {
        BOOST_STATIC_ASSERT( Rows == 2 );
        // NOTE: The static Cols == 1 check is done by operator []
        operator[]( 0 ) = x;
        operator[]( 1 ) = y;
    }

    /**
     * Constructor easing the initialization of vectors. This won't compile if Cols != 1 and Rows != 3.
     *
     * \param x x coefficient
     * \param y y coefficient
     * \param z z coefficient
     */
    WMatrixFixed( const ValueT& x, const ValueT& y, const ValueT& z )
    {
        BOOST_STATIC_ASSERT( Rows == 3 );
        // NOTE: The static Cols == 1 check is done by operator []
        operator[]( 0 ) = x;
        operator[]( 1 ) = y;
        operator[]( 2 ) = z;
    }

    /**
     * Constructor easing the initialization of vectors. This won't compile if Cols != 1 and Rows != 4.
     *
     * \param x x coefficient
     * \param y y coefficient
     * \param z z coefficient
     * \param w w coefficient
     */
    WMatrixFixed( const ValueT& x, const ValueT& y, const ValueT& z, const ValueT& w )
    {
        BOOST_STATIC_ASSERT( Rows == 4 );
        // NOTE: The static Cols == 1 check is done by operator []
        operator[]( 0 ) = x;
        operator[]( 1 ) = y;
        operator[]( 2 ) = z;
        operator[]( 3 ) = w;
    }

    /**
     * Copy construction casting the given value type. This is useful to create matrices with matrices using another value type.
     *
     * \tparam RHSValueT  Value type of the given matrix to copy
     * \tparam RHSValueStoreT Valuestore type of the given matrix to copy
     * \param m the matrix to copy
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& m )     // NOLINT - we do not want it explicit
    {
        setValues( m.m_values );
    }

    /**
     * Returns an identity matrix.
     *
     * \return the identity matrix.
     */
    static MatrixType identity()
    {
        MatrixType m = zero();
        for( size_t i = 0; i < std::min( Rows, Cols ); ++i )
        {
            m( i, i ) = ValueT( 1 );
        }
        return m;
    }

    /**
     * Returns a zero-initialized matrix.
     *
     * \return the matrix.
     */
    static MatrixType zero()
    {
        MatrixType m;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = ValueT( 0 );
            }
        }
        return m;
    }

    /**
     * Copy construction allowing the creation of a WMatrixFixed by another matrix of different size.
     * Please see \ref fromMatrices for more details, since this call is equivalent to fromMatrices( zero(), src, rowOffset, colOffset ).
     *
     * \see fromMatrices
     *
     * \tparam RHSValueT Value type of the given matrix
     * \tparam RHSRows Number of rows of the given matrix.
     * \tparam RHSCols Number of cols of the given matrix.
     * \tparam RHSValueStoreT Value store of the given matrix.
     *
     * \param src the matrix to copy
     * \param rowOffset row offset, defaults to 0
     * \param colOffset col offset, defaults to 0
     *
     * \return The newly created matrix.
     */
    template< typename RHSValueT, size_t RHSRows, size_t RHSCols, ValueStoreTemplate RHSValueStoreT >
    static MatrixType fromMatrix( const WMatrixFixed< RHSValueT, RHSRows, RHSCols, RHSValueStoreT >& src, size_t rowOffset = 0,
                                                                                                          size_t colOffset = 0 )
    {
        return fromMatrices( zero(), src, rowOffset, colOffset );
    }

    /**
     * Copy construction allowing the creation of a WMatrixFixed by another matrix of different size.
     * The specified source matrix gets copied into the area specified by its dimensions and the offset. On all other places, the specified
     * reference matrix is used.
     *
     * \tparam RHSValueT Value type of the given matrix
     * \tparam RHSRows Number of rows of the given matrix.
     * \tparam RHSCols Number of cols of the given matrix.
     * \tparam RHSValueStoreT Value store of the given matrix.
     *
     * \param m the reference matrix to use where src is not defined or used (due to offset)
     * \param src the matrix to copy
     * \param rowOffset row offset, defaults to 0
     * \param colOffset col offset, defaults to 0
     *
     * \return The newly created matrix.
     */
    template< typename RHSValueT, size_t RHSRows, size_t RHSCols, ValueStoreTemplate RHSValueStoreT >
    static MatrixType fromMatrices( const MatrixType& m,
                                    const WMatrixFixed< RHSValueT, RHSRows, RHSCols, RHSValueStoreT >& src, size_t rowOffset = 0,
                                                                                                            size_t colOffset = 0 )
    {
        MatrixType result;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                if( ( row >= rowOffset ) && ( col >= colOffset ) )
                {
                    // find the correct index in the src matrix
                    size_t srcRow = row - rowOffset;
                    size_t srcCol = col - colOffset;

                    // is this a valid index?
                    if( ( srcRow < RHSRows ) && ( srcCol < RHSCols ) )
                    {
                        result( row, col ) = src( srcRow, srcCol );
                    }
                    else
                    {
                        result( row, col ) = m( row, col );
                    }
                }
                else
                {
                    result( row, col ) = m( row, col );
                }
            }
        }
        return result;
    }

    /**
      * Set a row to a specific vector.
      *
      * \tparam RHSValueT Value type of the given matrix
      * \tparam ValueStoreT Value store of the given matrix
      *
      * \param index the index of the row you want to set
      * \param vec the values to set for the row
      *
      */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void  setRowVector( size_t index, const WMatrixFixed< RHSValueT, Rows, 1, RHSValueStoreT >& vec )
    {
        for( size_t col = 0; col < Cols; col++ )
        {
            at( index, col ) = vec( col, 0 );
        }
    }

    /**
      * Get a vector containing a specific row
      *
      * \param index the index of the row
      *
      * \return the row as a vector
      */
    WMatrixFixed< ValueT, Cols, 1, ValueStoreT > getRowVector( size_t index ) const
    {
        WMatrixFixed< ValueT, Cols, 1 > result;
        for( size_t col = 0; col < Cols; col++ )
        {
            result( col, 0 ) = at( index, col );
        }

        return result;
    }

    /**
      * Set a column to a specific vector.
      *
      * \tparam RHSValueT Value type of the given matrix
      * \tparam ValueStoreT Value store of the given matrix
      *
      * \param index the index of the column you want to set
      * \param vec the values to set for the column
      *
      */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void  setColumnVector( size_t index, const WMatrixFixed< RHSValueT, Rows, 1, RHSValueStoreT >& vec )
    {
        for( size_t row = 0; row < Rows; row++ )
        {
            at( row, index ) = vec( row, 0 );
        }
    }

    /**
      * Get a vector containing a specific column
      *
      * \param index the index of the column
      *
      * \return the column as a vector
      */
    WMatrixFixed< ValueT, Rows, 1 > getColumnVector( size_t index ) const
    {
        WMatrixFixed< ValueT, Rows, 1 > result;
        for( size_t row = 0; row < Rows; row++ )
        {
            result( row, 0 ) = at( row, index );
        }

        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Conversion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Conversion to a Eigen3 Matrix of same size and type.
     *
     * \return eigen3 matrix
     */
    operator Eigen::Matrix< ValueT, Rows, Cols >() const
    {
        Eigen::Matrix< ValueT, Rows, Cols > m;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator()( row, col );
            }
        }
        return m;
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 2 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec2d() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( Rows == 2 );
        return osg::Vec2d( operator[]( 0 ), operator[]( 1 ) );
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 3 or 4 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec2f() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( Rows == 2 );
        return osg::Vec2f( operator[]( 0 ), operator[]( 1 ) );
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 3 or 4 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec3d() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( ( Rows == 3 ) || ( Rows == 4 ) );
        return osg::Vec3d( operator[]( 0 ), operator[]( 1 ), operator[]( 2 ) );
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 3 or 4 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec3f() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( ( Rows == 3 ) || ( Rows == 4 ) );
        return osg::Vec3f( operator[]( 0 ), operator[]( 1 ), operator[]( 2 ) );
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 4 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec4d() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( Rows == 4 );
        return osg::Vec4d( operator[]( 0 ), operator[]( 1 ), operator[]( 2 ), operator[]( 3 ) );
    }

    /**
     * Cast to OSG Vector. This will only compile for matrices with only one col and 4 rows.
     *
     * \return OSG vector.
     */
    operator osg::Vec4f() const
    {
        // NOTE: operator[] already manages Cols=0 assert and casting is done implicitly
        BOOST_STATIC_ASSERT( Rows == 4 );
        return osg::Vec4f( operator[]( 0 ), operator[]( 1 ), operator[]( 2 ), operator[]( 3 ) );
    }

    /**
     * Convert this matrix to a OSG Matrix of size 4x4. This compiles only for 4x4 WMatrix types.
     *
     * \return the OSG Matrix
     */
    operator osg::Matrixd() const
    {
        BOOST_STATIC_ASSERT( Rows == 4 );
        BOOST_STATIC_ASSERT( Cols == 4 );

        osg::Matrixd m2;
        for( size_t row = 0; row < 4; ++row )
        {
            for( size_t col = 0; col < 4; ++col )
            {
                m2( row, col ) = operator()( row, col );
            }
        }
        return m2;
    }

    /**
     * A convenience function to cast the WMatrixFixed types to arbitrary other vector/matrix types that are supported by WMatrixFixed. This
     * method is mainly needed for ambiguities during type resolution, if the target methods signature allows several different vec/matrix types.
     * Example: you have void do( osg::Vec3f v ) and void do( osg::Vec3d v ). If you do WVector3d myV; do( myV ); This is ambiguous since
     * WVector3d can be casted to either osg::Vec3d AND Vec3f implicitly.
     *
     * \tparam TargetType the type needed (to cast to)
     *
     * \return the required type
     */
    template< typename TargetType >
    TargetType as() const
    {
        return operator TargetType();
    }

    /**
     * Cast to matrix of same size with different value type.
     *
     * \tparam ResultValueType resulting value type
     * \tparam ResultValueStore resulting value store
     *
     * \return the converted matrix.
     */
    template < typename ResultValueType, ValueStoreTemplate ResultValueStore >
    operator WMatrixFixed< ResultValueType, Rows, Cols, ResultValueStore >() const
    {
        WMatrixFixed< ResultValueType, Rows, Cols, ResultValueStore > result;
        result.setValues( m_values );
        return result;
    }

    /**
     * Creates a WMatrix from a given Eigen3 Matrix
     *
     * \param m the Eigen3 matrix.
     */
    WMatrixFixed( const Eigen::Matrix< ValueT, Rows, Cols >& m )    // NOLINT - we do not want it explicit
    {
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                operator()( row, col ) = m( row, col );
            }
        }
    }

    /**
     * Creates a WMatrix from a given OSG 4x4 Matrix. Will not compile if Rows != 4 or Cols != 4.
     *
     * \param m the OSG matrix.
     */
    WMatrixFixed( const osg::Matrixd& m )     // NOLINT - we do not want it explicit
    {
        BOOST_STATIC_ASSERT( Rows == 4 );
        BOOST_STATIC_ASSERT( Cols == 4 );

        for( size_t row = 0; row < 4; ++row )
        {
            for( size_t col = 0; col < 4; ++col )
            {
                operator()( row, col ) = m( row, col );
            }
        }
    }

    /**
     * Creates a WMatrix from a given OSG Vector. Will not compile if Rows != 3 or Cols != 1.
     *
     * \param m the OSG vector.
     */
    WMatrixFixed( const osg::Vec3f& m )     // NOLINT - we do not want it explicit
    {
        BOOST_STATIC_ASSERT( Rows == 3 );
        BOOST_STATIC_ASSERT( Cols == 1 );

        operator[]( 0 ) = m.x();
        operator[]( 1 ) = m.y();
        operator[]( 2 ) = m.z();
    }

    /**
     * Creates a WMatrix from a given OSG Vector. Will not compile if Rows != 3 or Cols != 1.
     *
     * \param m the OSG vector.
     */
    WMatrixFixed( const osg::Vec3d& m )     // NOLINT - we do not want it explicit
    {
        BOOST_STATIC_ASSERT( Rows == 3 );
        BOOST_STATIC_ASSERT( Cols == 1 );

        operator[]( 0 ) = m.x();
        operator[]( 1 ) = m.y();
        operator[]( 2 ) = m.z();
    }

    /**
     * Creates a WMatrix from a given OSG Vector. Will not compile if Rows != 4 or Cols != 1.
     *
     * \param m the OSG vector.
     */
    WMatrixFixed( const osg::Vec4f& m )     // NOLINT - we do not want it explicit
    {
        BOOST_STATIC_ASSERT( Rows == 4 );
        BOOST_STATIC_ASSERT( Cols == 1 );

        operator[]( 0 ) = m[0];
        operator[]( 1 ) = m[1];
        operator[]( 2 ) = m[2];
        operator[]( 3 ) = m[3];
    }

    /**
     * Creates a WMatrix from a given OSG Vector. Will not compile if Rows != 4 or Cols != 1.
     *
     * \param m the OSG vector.
     */
    WMatrixFixed( const osg::Vec4d& m )     // NOLINT - we do not want it explicit
    {
        BOOST_STATIC_ASSERT( Rows == 4 );
        BOOST_STATIC_ASSERT( Cols == 1 );

        operator[]( 0 ) = m[0];
        operator[]( 1 ) = m[1];
        operator[]( 2 ) = m[2];
        operator[]( 3 ) = m[3];
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Copy and Assignment
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Assigns the given argument matrix to this one. If the types match, a reference is returned.
     *
     * \tparam RHSValueT the value type of the source matrix.
     * \param rhs The right hand side of the assignment
     *
     * \return This matrix.
     */
    template < typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    MatrixType& operator=( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs )
    {
        setValues( rhs.m_values );
        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Operators
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Matrix-Matrix multiplication. The number of columns of this matrix and the rows of the other need to match.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \tparam RHSCols the number of columns of the given matrix. The number if rows must match the number of columns in this matrix
     * \param rhs the matrix
     *
     * \return The product of the matrices
     */
    template< typename RHSValueT, size_t RHSCols, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, RHSCols, ValueStoreT >
        operator*( const WMatrixFixed< RHSValueT, Cols, RHSCols, RHSValueStoreT >& rhs ) const
    {
        typedef typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result ResultValueType;

        // NOTE: this is quite a naive implementation.
        WMatrixFixed< ResultValueType, Rows, RHSCols, ValueStoreT > m;
        for( std::size_t row = 0; row < Rows; ++row )
        {
            for( std::size_t col = 0; col < RHSCols; ++col )
            {
                m( row, col ) = ResultValueType();
                // dot between col and row vector
                for( std::size_t i = 0; i < Cols; ++i )
                {
                    m( row, col ) += operator()( row, i ) * rhs( i, col );
                }
            }
        }
        return m;
    }

    /**
     * Matrix-Matrix multiplication with self-assignment.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void operator*=( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs )
    {
        operator=( *this * rhs );
    }

    /**
     * Matrix-Scalar multiplication.
     *
     * \tparam RHSValueT the integral type of the given scalar
     * \param rhs the scalar
     *
     * \return The product of this matrix with the given scalar value.
     */
    template< typename RHSValueT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
    operator*( const RHSValueT& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator()( row, col ) * rhs;
            }
        }
        return m;
    }

    /**
     * Matrix-Scalar multiplication with self-assignment.
     *
     * \tparam RHSValueT the integral type of the given scalar
     * \param rhs the scalar
     */
    template< typename RHSValueT >
    void operator*=( const RHSValueT& rhs )
    {
        operator=( *this * rhs );
    }

    /**
     * Matrix-Scalar division.
     *
     * \tparam RHSValueT the integral type of the given scalar
     * \param rhs the scalar
     *
     * \return The matrix having all components divided by the scalar.
     */
    template< typename RHSValueT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
    operator/( const RHSValueT& rhs ) const
    {
        typedef typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result ResultT;
        return operator*( ResultT( 1 ) / static_cast< ResultT >( rhs ) );
    }

    /**
     * Matrix-Scalar division with self-assignmnet.
     *
     * \tparam RHSValueT the integral type of the given scalar
     * \param rhs the scalar
     */
    template< typename RHSValueT >
    void operator/=( const RHSValueT& rhs )
    {
        operator=( ( *this ) / rhs );
    }

    /**
     * Matrix addition. The number of columns and rows must be the same.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     *
     * \return The sum of the current and the given matrix
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
        operator+( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator()( row, col ) + rhs( row, col );
            }
        }
        return m;
    }

    /**
     * Matrix addition with self-assignment.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void operator+=( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs )
    {
        operator=( *this + rhs );
    }

    /**
     * Matrix subtraction. The number of columns and rows must be the same.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     *
     * \return The difference of the current and the given matrix.
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
        operator-( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for( size_t row = 0; row < Rows; ++row )
        {
            for( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator()( row, col ) - rhs( row, col );
            }
        }
        return m;
    }

    /**
     * Matrix subtraction with self-assignment.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void operator-=( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) throw()
    {
        operator=( *this - rhs );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Access
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does not check for validity of
     * the indices. Use \ref at for this.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \return A reference to the component of an row and column
     */
    ValueT& operator()( size_t row, size_t col ) throw()
    {
        return m_values( row, col );
    }

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does not check for validity of
     * the indices. Use \ref at for this.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \return A const reference to the component of an row and column
     */
    const ValueT& operator()( size_t row, size_t col ) const throw()
    {
        return m_values( row, col );
    }

    /**
     * Returns a reference to the component of the first column to provide access to the component. It does not check for validity of
     * the indices. Use this for single-column matrices (i.e. vectors). For matrices with cols!=0, this will not compile.
     *
     * \param row the row, staring with 0
     *
     * \return A reference to the component of the first column
     */
    ValueT& operator[]( size_t row ) throw()
    {
        BOOST_STATIC_ASSERT( Cols == 1 );
        return m_values( row, 0 );
    }

    /**
     * Returns a reference to the component of the first column to provide access to the component. It does not check for validity of
     * the indices. Use this for single-column matrices (i.e. vectors). For matrices with cols!=0, this will not compile.
     *
     * \param row the row, staring with 0
     *
     * \return A const reference to the component of the first column
     */
    const ValueT& operator[]( size_t row ) const throw()
    {
        BOOST_STATIC_ASSERT( Cols == 1 );
        return m_values( row, 0 );
    }

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does check for validity of
     * the indices. Use operator() for avoiding this check.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \return A reference to the component of an row and column
     *
     * \throw WOutOfBounds if the specified index is invalid
     */
    ValueT& at( size_t row, size_t col ) throw( WOutOfBounds )
    {
        if( ( row >= Rows ) || ( col >= Cols ) )
        {
            throw WOutOfBounds( "Index pair (" + string_utils::toString( row ) + ", " + string_utils::toString( col ) +
                                ") is invalid for " + string_utils::toString( Rows ) + "x" + string_utils::toString( Cols ) +
                                " matrix." );
        }
        return operator()( row, col );
    }

    /**
     * Returns a const reference to the component of an row and column in order to provide access to the component.
     * It does check for validity of
     * the indices. Use operator() for avoiding this check.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \return A const reference to the component of an row and column.
     *
     * \throw WOutOfBounds if the specified index is invalid
     */
    const ValueT& at( size_t row, size_t col ) const throw( WOutOfBounds )
    {
        if( ( row >= Rows ) || ( col >= Cols ) )
        {
            throw WOutOfBounds( "Index pair (" + string_utils::toString( row ) + ", " + string_utils::toString( col ) +
                                ") is invalid for " + string_utils::toString( Rows ) + "x" + string_utils::toString( Cols ) +
                                " matrix." );
        }
        return operator()( row, col );
    }

    /**
     * Access x element of vector. Works only for matrices with Cols == 1.
     *
     * \return x element
     */
    ValueT& x() throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 1 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 0 );
    }

    /**
     * Access x element of vector. Works only for matrices with Cols == 1.
     *
     * \return x element
     */
    const ValueT& x() const throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 1 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 0 );
    }

    /**
     * Access y element of vector. Works only for matrices with Cols == 1.
     *
     * \return y element
     */
    ValueT& y() throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 2 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 1 );
    }

    /**
     * Access y element of vector. Works only for matrices with Cols == 1.
     *
     * \return y element
     */
    const ValueT& y() const throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 2 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 1 );
    }

    /**
     * Access z element of vector. Works only for matrices with Cols == 1.
     *
     * \return z element
     */
    ValueT& z() throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 3 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 2 );
    }

    /**
     * Access z element of vector. Works only for matrices with Cols == 1.
     *
     * \return z element
     */
    const ValueT& z() const throw()
    {
        BOOST_STATIC_ASSERT( Rows >= 3 );
        BOOST_STATIC_ASSERT( Cols == 1 );
        return operator[]( 2 );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Comparison
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Compares two matrices and returns true if they are equal (component-wise).
     *
     * \tparam RHSValueT the value type of the argument
     * \param rhs The right hand side of the comparison
     *
     * \return true if equal
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    bool operator==( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const throw()
    {
        bool eq = true;
        for( size_t row = 0; eq && ( row < Rows ); ++row )
        {
            for( size_t col = 0; eq && ( col < Cols ); ++col )
            {
                eq = eq && ( operator()( row, col ) == rhs( row, col ) );
            }
        }
        return eq;
    }

    /**
     * Compares two matrices and returns true if this is smaller than the specified one (component-wise).
     *
     * \tparam RHSValueT the value type of the argument
     * \param rhs The right hand side of the comparison
     *
     * \return true if this is less
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    bool operator<( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const throw()
    {
        bool eq = true;
        bool result = true;
        for( size_t row = 0; eq && ( row < Rows ); ++row )
        {
            for( size_t col = 0; eq && ( col < Cols ); ++col )
            {
                eq = eq && ( operator()( row, col ) == rhs( row, col ) );
                result = ( operator()( row, col ) < rhs( row, col ) );
            }
        }
        return result;
    }

    /**
     * Compares two matrices and returns true if they are not equal.
     *
     * \tparam RHSValueT the value type of the argument
     * \param rhs The right hand side of the comparison
     * \return true if not equal.
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    bool operator!=( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const throw()
    {
        return !operator==( rhs );
    }

private:
    /**
     * The value array. Stored row-major. Never access this directly. Always use operator(). This allows us to later-on use another storing
     * order.
     */
    ValueStoreType m_values;

    /**
     * Sets the new values. Always use this method for replacing values in this matrix.
     *
     * \param values
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    void setValues( const RHSValueStoreT< RHSValueT, Rows, Cols >& values )
    {
        for( std::size_t i = 0; i < Rows; ++i )
        {
            for( std::size_t j = 0; j < Cols; ++j )
            {
                m_values( i, j ) = static_cast< ValueT >( values( i, j ) );
            }
        }
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Some standard matrices and vectors
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef WMatrixFixed< double, 3, 3 > WMatrix3d;
typedef WMatrixFixed< double, 4, 4 > WMatrix4d;
typedef WMatrixFixed< float, 3, 3 > WMatrix3f;
typedef WMatrixFixed< float, 4, 4 > WMatrix4f;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Commutative Operators
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Scale the given matrix by the value. This is needed for having * to be commutative. For more details, see  \ref WMatrixFixed::operator*.
 *
 * \tparam ScalarT Integral type of scaler
 * \tparam MatrixValueT Value type of matrix
 * \tparam MatrixRows Rows of matrix
 * \tparam MatrixCols Columns of matrix
 * \tparam MatrixValueStoreT matrix value store type
 * \param n the scalar multiplier
 * \param mat the matrix to scale
 *
 * \return scaled matrix.
 */
template < typename ScalarT,
           typename RHSValueT, size_t RHSRows, size_t RHSCols, ValueStoreTemplate RHSValueStoreT >
WMatrixFixed< typename WTypeTraits::TypePromotion< ScalarT, RHSValueT >::Result, RHSRows, RHSCols, RHSValueStoreT >
    operator*( const ScalarT n, const WMatrixFixed< RHSValueT, RHSRows, RHSCols, RHSValueStoreT >& mat )
{
    return mat * n;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Non-friend Non-Member functions
//  * they implement most of the common algebra
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Calculate dot product between two vectors.
 *
 * \tparam AValueT Value type of the first vector
 * \tparam AValueStoreT ValueStore type of the first vector
 * \tparam BValueT Value type of the second vector
 * \tparam BValueStoreT ValueStore type of the second vector
 * \tparam Rows Number of rows for the vectors
 * \param a the first vector
 * \param b the second vector
 *
 * \return dot product
 */
template< typename AValueT, ValueStoreTemplate AValueStoreT,
          typename BValueT, ValueStoreTemplate BValueStoreT,
          size_t Rows >
typename WTypeTraits::TypePromotion< AValueT, BValueT >::Result dot( const WMatrixFixed< AValueT, Rows, 1, AValueStoreT >& a,
                                                                     const WMatrixFixed< BValueT, Rows, 1, BValueStoreT >& b )
{
    typedef typename WTypeTraits::TypePromotion< AValueT, BValueT >::Result ResultType;
    ResultType r = ResultType();
    for( size_t i = 0; i < Rows; ++i )
    {
        r += a( i, 0 ) * b( i, 0 );
    }
    return r;
}

/**
 * Calculate cross product between two 3D vectors.
 *
 * \tparam AValueT Value type of the first vector
 * \tparam AValueStoreT ValueStore type of the first vector
 * \tparam BValueT Value type of the second vector
 * \tparam BValueStoreT ValueStore type of the second vector
 * \tparam ResultValueStoreT resulting valuestore
 * \param a the first vector
 * \param b the second vector
 *
 * \return cross product
 */
template< typename AValueT, ValueStoreTemplate AValueStoreT,
          typename BValueT, ValueStoreTemplate BValueStoreT >
WMatrixFixed< typename WTypeTraits::TypePromotion< AValueT, BValueT >::Result, 3, 1 >
    cross( const WMatrixFixed< AValueT, 3, 1, AValueStoreT >& a, const WMatrixFixed< BValueT, 3, 1, BValueStoreT >& b )
{
    typedef WMatrixFixed< typename WTypeTraits::TypePromotion< AValueT, BValueT >::Result, 3, 1 > ResultT;

    // NOTE: to implement a general cross product for arbitrary row counts, the implementation is more complex and requires the implementation of
    // the Levi Civita symbol.
    ResultT v;
    v[0] = a[1] * b[2] - a[2] * b[1];
    v[1] = a[2] * b[0] - a[0] * b[2];
    v[2] = a[0] * b[1] - a[1] * b[0];
    return v;
}

/**
 * Calculates the <b>squared</b> length of a specified vector.
 *
 * \tparam ValueT Value type
 * \tparam ValueStoreT Value store to use
 * \tparam Rows number of rows in this colums-vector
 * \param a the vector
 *
 * \return the length of the vector
 */
template< typename ValueT, ValueStoreTemplate ValueStoreT, size_t Rows >
ValueT length2( const WMatrixFixed< ValueT, Rows, 1, ValueStoreT >& a )
{
    ValueT r = ValueT();
    for( size_t i = 0; i < Rows; ++i )
    {
        r += a( i, 0 ) * a( i, 0 );
    }
    return r;
}

/**
 * Calculates the <b>squared</b> length of a specified vector.
 *
 * \tparam ValueT Value type
 * \tparam ValueStoreT Value store to use
 * \tparam Cols number of columns in this row-vector
 * \param a the vector
 *
 * \return length of the vector
 */
template< typename ValueT, ValueStoreTemplate ValueStoreT, size_t Cols >
ValueT length2( const WMatrixFixed< ValueT, 1, Cols, ValueStoreT >& a )
{
    ValueT r = ValueT();
    for( size_t i = 0; i < Cols; ++i )
    {
        r += a( 0, i ) * a( 0, i );
    }
    return r;
}

/**
 * Calculates the length of a specified vector.
 *
 * \tparam ValueT Value type
 * \tparam ValueStoreT Value store to use
 * \tparam Rows number of rows in this colums-vector
 * \param a the vector
 *
 * \return the length of the vector
 */
template< typename ValueT, ValueStoreTemplate ValueStoreT, size_t Rows >
ValueT length( const WMatrixFixed< ValueT, Rows, 1, ValueStoreT >& a )
{
    return sqrt( length2( a ) );
}

/**
 * Calculates the length of a specified vector.
 *
 * \tparam ValueT Value type
 * \tparam ValueStoreT Value store to use
 * \tparam Cols number of columns in this row-vector
 * \param a the vector
 *
 * \return length of the vector
 */
template< typename ValueT, ValueStoreTemplate ValueStoreT, size_t Cols >
ValueT length( const WMatrixFixed< ValueT, 1, Cols, ValueStoreT >& a )
{
    return sqrt( length2( a ) );
}

/**
 * Normalizes the given vector.
 *
 * \tparam RHSValueT given matrix value type
 * \tparam Rows given row number
 * \tparam Cols given col number
 * \tparam RHSValueStoreT given matrix' valuestore
 * \param m the vector
 *
 * \return normalized vector
 */
template< typename RHSValueT, size_t Rows, size_t Cols, ValueStoreTemplate RHSValueStoreT >
WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT > normalize( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& m )
{
    // NOTE: the static cast ensures that the returned matrix value type is the same as the input one.
    return m * static_cast< RHSValueT >( 1.0 / length( m ) );
}

/**
 * Inverts the specified matrix.
 *
 * \tparam ValueT The type of the values stored. Most of the operations, if multiple types are involved, use WTypeTraits to guess the better of
 *         both.
 * \tparam Size Number of rows and columns
 * \tparam ValueStoreT The ValueStore handles the values and their access. Use special types here for a fine-grained access control or
 *         data-management
 * \param m the matrix to invert
 *
 * \return inverted matrix
 */
template< typename ValueT, std::size_t Size, template< typename, std::size_t, std::size_t > class ValueStoreT >
WMatrixFixed< ValueT, Size, Size, ValueStoreT > invert( WMatrixFixed< ValueT, Size, Size, ValueStoreT > const& m )
{
    // this is a standard implementation
    return WMatrixFixed< ValueT, Size, Size, ValueStoreT >( static_cast< Eigen::Matrix< ValueT, Size, Size > >( m ).inverse() );
}

/**
 * Transposes a given matrix.
 *
 * \tparam ValueT The type of the values stored. Most of the operations, if multiple types are involved, use WTypeTraits to guess the better of
 *         both.
 * \tparam Rows Number of Rows
 * \tparam Cols Number of Columns
 * \tparam ValueStoreT The ValueStore handles the values and their access. Use special types here for a fine-grained access control or
 *         data-management
 *
 * \param mat the matrix to transpose
 *
 * \return transposed matrix
 */
template< typename ValueT, std::size_t Rows, std::size_t Cols, template< typename, std::size_t, std::size_t > class ValueStoreT >
WMatrixFixed< ValueT, Cols, Rows, ValueStoreT > transpose( WMatrixFixed< ValueT, Rows, Cols, ValueStoreT > const& mat )
{
    WMatrixFixed< ValueT, Cols, Rows, ValueStoreT > res;
    for( size_t row = 0; row < mat.getRows(); ++row )
    {
        for( size_t col = 0; col < mat.getColumns(); ++col )
        {
            res( col, row ) = mat( row, col );
        }
    }
    return res;
}

/**
 * Write a matrix in string representation to the given output stream.
 *
 * \tparam ValueT the integral type for the matrix coefficients
 * \tparam Rows The number of rows
 * \tparam Cols The number of columns
 * \tparam ValueStoreT the ValueStore type used for storing the values
 *
 * \param out the output stream to print the value to
 * \param m the matrix
 *
 * \return the output stream extended by the trigger value.
 */
template< typename ValueT, size_t Rows, size_t Cols, ValueStoreTemplate ValueStoreT >
std::ostream& operator<<( std::ostream& out, const WMatrixFixed< ValueT, Rows, Cols, ValueStoreT >& m )
{
    // NOTE if you change this operator, also change operator >>
    for( size_t row = 0; row < m.getRows(); ++row )
    {
        for( size_t col = 0; col < m.getColumns(); ++col )
        {
            out << m( row, col ) << ";";
        }
    }
    return out;
}

/**
 * Read a matrix in string representation from the given input stream.
 *
 * \param in the input stream to read the value from
 * \param m  set the values red to this
 *
 * \tparam ValueT the integral type for the matrix coefficients
 * \tparam Rows The number of rows
 * \tparam Cols The number of columns
 * \tparam ValueStoreT the ValueStore type used for storing the values
 *
 * \return the input stream.
 */
template< typename ValueT, size_t Rows, size_t Cols, ValueStoreTemplate ValueStoreT >
std::istream& operator>>( std::istream& in, WMatrixFixed< ValueT, Rows, Cols, ValueStoreT >& m ) throw()
{
    // NOTE if you change this operator, also change operator <<
    typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;

    std::string s;
    in >> s;
    boost::char_separator< char > separators( " ;" );
    Tokenizer t( s, separators );

    Tokenizer::iterator it = t.begin();
    for( std::size_t row = 0; row < Rows; ++row )
    {
        for( std::size_t col = 0; col < Cols; ++col )
        {
            if( it == t.end() )
            {
                return in;
            }
            m( row, col ) = string_utils::fromString< ValueT >( *it );
            ++it;
        }
    }

    return in;
}

#endif  // WMATRIXFIXED_H

