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

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include "../../WDefines.h"
#include "../../WStringUtils.h"
#include "../../WTypeTraits.h"

#include "../../exceptions/WOutOfBounds.h"

#include "../../../ext/Eigen/Core"
#include "../../../ext/Eigen/LU" // needed for the inverse() function

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
     * Returns a reference to the component of an row and column in order to provide access to the component. It does not check for validity of
     * the indices. Use \ref at for this.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     */
    ValueT& operator()( size_t row, size_t col ) throw()
    {
        return m_values[ row * Cols + col ];
    }

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does not check for validity of
     * the indices. Use \ref at for this.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
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
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                ( row, col ) = rhs( row, col );
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
     * Default constructor. The values are un-initialized! Use the static methods \ref zero(), \ref identity() or any of the predefined
     * transformations if an initialized matrix is wished.
     */
    WMatrixFixed()
    {
        // No initialization needed. Matrix values are initialized by ValueStoreT.
    }

    /**
     * Returns an identity matrix.
     *
     * \return the identity matrix.
     */
    static MatrixType identity()
    {
        MatrixType m = zero();
        for ( size_t i = 0; i < std::min( Rows, Cols ); ++i )
        {
            m( i, i ) = ValueT( 1 );
        }
        return m;
    }

    /**
     * Returns an identity matrix.
     * \deprecated just for compatibility with eigen3 types.
     *
     * \return identity
     */
    OW_API_DEPRECATED static MatrixType Identity()
    {
        return identity();
    }

    /**
     * Returns a zero-initialized matrix.
     *
     * \return the matrix.
     */
    static MatrixType zero()
    {
        MatrixType m;
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = ValueT( 0 );
            }
        }
        return m;
    }

    /**
     * Returns an zero-initialized matrix.
     * \deprecated just for compatibility with eigen3 types.
     *
     * \return zero initialized matrix.
     */
    OW_API_DEPRECATED static MatrixType Zero()
    {
        return zero();
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
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator()( row, col );
            }
        }
        return m;
    }

    /**
     * Creates a WMatrix from a given Eigen3 Matrix
     *
     * \param m the Eigen3 matrix.
     */
    WMatrixFixed( const Eigen::Matrix< ValueT, Rows, Cols >& m )
    {
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                operator() ( row, col ) = m( row, col );
            }
        }
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
     */
    template< typename RHSValueT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
    operator*( const RHSValueT& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator() ( row, col ) * rhs;
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
     * Matrix addition. The number of columns and rows must be the same.
     *
     * \tparam RHSValueT the integral type of the given matrix
     * \param rhs the matrix
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
        operator+( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator() ( row, col ) + rhs( row, col );
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
     */
    template< typename RHSValueT, ValueStoreTemplate RHSValueStoreT >
    WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT >
        operator-( const WMatrixFixed< RHSValueT, Rows, Cols, RHSValueStoreT >& rhs ) const
    {
        WMatrixFixed< typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result, Rows, Cols, ValueStoreT > m;
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                m( row, col ) = operator() ( row, col ) - rhs( row, col );
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
     */
    const ValueT& operator()( size_t row, size_t col ) const throw()
    {
        return m_values( row, col );
    }

    /**
     * Returns a reference to the component of the first column to provide access to the component. It does not check for validity of
     * the indices. Use this for single-column matrices (i.e. vectors).
     *
     * \param row the row, staring with 0
     */
    ValueT& operator[]( size_t row ) throw()
    {
        return m_values( row, 0 );
    }

    /**
     * Returns a reference to the component of the first column to provide access to the component. It does not check for validity of
     * the indices. Use this for single-column matrices (i.e. vectors).
     *
     * \param row the row, staring with 0
     */
    const ValueT& operator[]( size_t row ) const throw()
    {
        return m_values( row, 0 );
    }

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does check for validity of
     * the indices. Use \ref operator() for avoiding this check.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \throw WOutOfBounds if the specified index is invalid
     */
    ValueT& at( size_t row, size_t col ) throw( WOutOfBounds )
    {
        if ( ( row >= Rows ) || ( col >= Cols ) )
        {
            throw WOutOfBounds( "Index pair (" + boost::lexical_cast< std::string >( row ) + ", " + boost::lexical_cast< std::string >( col ) +
                                ") is invalid for " + boost::lexical_cast< std::string >( Rows ) + "x" + boost::lexical_cast< std::string >( Cols ) +
                                " matrix." );
        }
        return operator() ( row, col );
    }

    /**
     * Returns a reference to the component of an row and column in order to provide access to the component. It does check for validity of
     * the indices. Use \ref operator() for avoiding this check.
     *
     * \param row the row, staring with 0
     * \param col the column, starting with 0
     *
     * \throw WOutOfBounds if the specified index is invalid
     */
    const ValueT& at( size_t row, size_t col ) const throw( WOutOfBounds )
    {
        if ( ( row >= Rows ) || ( col >= Cols ) )
        {
            throw WOutOfBounds( "Index pair (" + boost::lexical_cast< std::string >( row ) + ", " + boost::lexical_cast< std::string >( col ) +
                                ") is invalid for " + boost::lexical_cast< std::string >( Rows ) + "x" + boost::lexical_cast< std::string >( Cols ) +
                                " matrix." );
        }
        return operator() ( row, col );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Comparison
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Compares two matrices and returns true if they are equal.
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
        typedef typename WTypeTraits::TypePromotion< ValueT, RHSValueT >::Result BetterType;
        for ( size_t row = 0; row < Rows; ++row )
        {
            for ( size_t col = 0; col < Cols; ++col )
            {
                eq &= ( operator() ( row, col ) == rhs( row, col ) );
            }
        }
        return eq;
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

typedef WMatrixFixed< double, 3, 3 > WMatrix3d_2;
typedef WMatrixFixed< double, 4, 4 > WMatrix4d_2;
typedef WMatrixFixed< double, 2, 1 > WVector2d_2;
typedef WMatrixFixed< double, 3, 1 > WVector3d_2;
typedef WMatrixFixed< double, 4, 1 > WVector4d_2;

typedef WMatrixFixed< float, 3, 3 > WMatrix3f_2;
typedef WMatrixFixed< float, 4, 4 > WMatrix4f_2;
typedef WMatrixFixed< float, 2, 1 > WVector2f_2;
typedef WMatrixFixed< float, 3, 1 > WVector3f_2;
typedef WMatrixFixed< float, 4, 1 > WVector4f_2;

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
    ValueT r = ValueT();
    for ( size_t i = 0; i < Rows; ++i )
    {
        r += a( i, 0 ) * a( i, 0 );
    }
    return sqrt( r );
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
    ValueT r = ValueT();
    for ( size_t i = 0; i < Cols; ++i )
    {
        r += a( 0, i ) * a( 0, i );
    }
    return sqrt( r );
}

/**
 * Normalizes the given vector.
 *
 * \tparam MatrixType type of matrix. This type needs to be supported by length().
 * \param m the vector
 *
 * \return normalized vector
 */
template< typename MatrixType >
MatrixType normalize( const MatrixType& m )
{
    return m * ( 1.0 / length( m ) );
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
    for ( size_t row = 0; row < m.getRows(); ++row )
    {
        for ( size_t col = 0; col < m.getColumns(); ++col )
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
            m( row, col ) = boost::lexical_cast< ValueT >( *it );
            ++it;
        }
    }

    return in;
}

#endif  // WMATRIXFIXED_H

