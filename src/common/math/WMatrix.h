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

#ifndef WMATRIX_H
#define WMATRIX_H

#include <iostream>

#include "WMatrix4x4.h"
#include "WValue.h"
#include "WVector3D.h"

#include "../WDefines.h"
#include "../../ext/Eigen/Core"
#include "../../ext/Eigen/LU"

/**
 * A double 3 times 3 matrix. Stack-allocated. Column Major!
 *
 * Column Major indexes:
 * [0 3 6
 *  1 4 7
 *  2 5 8]
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, 3, 3 > WMatrix3x3_2;

/**
 * A double 4 times 4 matrix. Stack-allocated. Column Major!
 *
 * Column Major indexes:
 * [0 4 8  12
 *  1 5 9  13
 *  2 6 10 14
 *  3 7 11 15]
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::Matrix< double, 4, 4 > WMatrix4x4_2;

/**
 * A double matrix of dynamic size. Heap-allocated. Column Major!
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::MatrixXd WMatrix_2;

/**
 * A complex double matrix of dynamic size. Heap-allocated.
 * If you want to access coefficients using the operator( size_t, size_t ), the first parameter is still the row index, starting with 0.
 *
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1Matrix.html
 * \see http://eigen.tuxfamily.org/dox/classEigen_1_1MatrixBase.html
 */
typedef Eigen::MatrixXcd WMatrixComplex_2;

/**
 * Converts a given WMatrix4x4_2 to an osg matrix.
 *
 * \param m the matrix to convert
 *
 * \return the converted matrix
 */
inline osg::Matrixd toOsgMatrixd( WMatrix4x4_2 m )
{
    osg::Matrixd m2;
    for ( size_t row = 0; row < 4; ++row )
    {
        for ( size_t col = 0; col < 4; ++col )
        {
            m2( row, col ) = m( row, col );
        }
    }
    return m2;
}

/**
 * Matrix template class with variable number of rows and columns.
 * The access function are row-major, which means that the rows
 * are the first parameter or index.
 */
template< typename T > class OW_API_DEPRECATED WMatrix : public WValue< T >
{
public:
    /**
     * Produces a square matrix with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     *
     * \param n Number of cols and rows in the matrix
     */
    explicit WMatrix( size_t n );

    /**
     * Produces a matrix with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     *
     * \param nbRows number of rows in the matrix
     * \param nbCols number of columns in the matrix
     */
    WMatrix( size_t nbRows, size_t nbCols );

    /**
     * Produces a matrix as copy of the one given as parameter.
     * \param newMatrix The matrix to be copied.
     */
    WMatrix( const WMatrix& newMatrix );

    /**
     * Copies the specified 4x4 matrix.
     *
     * \param newMatrix the matrix to copy
     */
    WMatrix( const WMatrix4x4& newMatrix ); // NOLINT

    /**
     * Makes the matrix contain the identity matrix, i.e. 1 on the diagonal.
     */
    WMatrix& makeIdentity();

    /**
     * Get number of rows.
     */
    size_t getNbRows() const;

    /**
     * Get number of columns.
     */
    size_t getNbCols() const;

    /**
     * Returns a reference to the component an row i, columns j in order to
     * provide access to the component.
     * \param i row
     * \param j column
     */
    T& operator()( size_t i, size_t j );

    /**
     * Returns a const reference to the component an row i, columns j in order to
     * provide read-only access to the component.
     * \param i row
     * \param j column
     */
    const T& operator()( size_t i, size_t j ) const;

    /**
     * Cast this matrix to an 4x matrix if it is a 4x4 matrix.
     *
     * \return casted matrix
     */
    operator WMatrix4x4() const;

    /**
     * Compares two matrices and returns true if they are equal.
     * \param rhs The right hand side of the comparison
     */
    bool operator==( const WMatrix& rhs ) const;

    /**
     * Compares two matrices and returns true if they are not equal.
     * \param rhs The right hand side of the comparison
     */
    bool operator!=( const WMatrix& rhs ) const;

    /**
     * Assigns the argument WMatrix to this WMatrix.
     * \param rhs The right hand side of the assignment
     */
    WMatrix& operator=( const WMatrix& rhs );

    /**
     * Multiplication of two matrices.
     * \param rhs The right hand side of the multiplication
     */
    WMatrix operator*( const WMatrix& rhs ) const;

    /**
     * Multiplication with a vector.
     * \param rhs The right hand side of the multiplication
     */
    WValue< T > operator*( const WValue< T >& rhs ) const;

    /**
     * Multiplication with a vector.
     * \param rhs The right hand side of the multiplication
     */
    WVector3D operator*( const WVector3D& rhs ) const;

    /**
     * Returns the transposed matrix.
     */
    WMatrix transposed() const;

protected:
private:
    size_t m_nbCols; //!< Number of columns of the matrix. The number of rows will be computed by (size/m_nbCols).
};

template< typename T > WMatrix< T >::WMatrix( size_t n )
    : WValue< T >( n * n )
{
    m_nbCols = n;
}

template< typename T > WMatrix< T >::WMatrix( size_t nbRows, size_t nbCols )
    : WValue< T >( nbRows * nbCols )
{
    m_nbCols = nbCols;
}

/**
 * Produces a matrix as copy of the one given as parameter.
 * \param newMatrix The matrix to be copied.
 */
template< typename T > WMatrix< T >::WMatrix( const WMatrix& newMatrix )
    : WValue< T >( newMatrix )
{
    m_nbCols = newMatrix.m_nbCols;
}

template< typename T > WMatrix< T >::WMatrix( const WMatrix4x4& newMatrix )
    : WValue< T >( 4 * 4 )
{
    m_nbCols = 4;
    for( size_t i = 0; i < 4; ++i )
    {
        for( size_t j = 0; j < 4; ++j )
        {
            ( *this )( i, j ) = newMatrix( i, j );
        }
    }
}

template< typename T > WMatrix< T >::operator WMatrix4x4() const
{
    size_t nbRows = this->size() / m_nbCols;
    WAssert( m_nbCols == 4 && nbRows == 4, "This is no 4x4 matrix." );
    WMatrix4x4 m;
    for( size_t i = 0; i < nbRows; ++i )
    {
        for( size_t j = 0; j < m_nbCols; ++j )
        {
            m( i, j ) = ( *this )( i, j );
        }
    }
    return m;
}

/**
 * Makes the matrix contain the identity matrix, i.e. 1 on the diagonal.
 */
template< typename T > WMatrix< T >& WMatrix< T >::makeIdentity()
{
    size_t nbRows = this->size() / m_nbCols;
    for( size_t i = 0; i < nbRows; ++i )
    {
        for( size_t j = 0; j < m_nbCols; ++j )
        {
            if( i == j )
            {
                (*this)( i, j ) = 1;
            }
            else
            {
                (*this)( i, j ) = 0;
            }
        }
    }
    return *this;
}

/**
 * Get number of rows.
 */
template< typename T > size_t WMatrix< T >::getNbRows() const
{
    return this->size() / m_nbCols;
}

/**
 * Get number of columns.
 */
template< typename T > size_t WMatrix< T >::getNbCols() const
{
    return m_nbCols;
}

/**
 * Returns a reference to the component an row i, columns j in order to
 * provide access to the component.
 * \param i row
 * \param j column
 */
template< typename T > T& WMatrix< T >::operator()( size_t i, size_t j )
{
    WAssert( j < m_nbCols && i * m_nbCols < this->size(), "Index out of bounds." );
    return (*this)[i * m_nbCols + j];
}

/**
 * Returns a const reference to the component an row i, columns j in order to
 * provide read-only access to the component.
 * \param i row
 * \param j column
 */
template< typename T > const T& WMatrix< T >::operator()( size_t i, size_t j ) const
{
    WAssert( j < m_nbCols && i * m_nbCols < this->size(), "Index out of bounds." );
    return (*this)[i * m_nbCols + j];
}

/**
 * Compares two matrices and returns true if they are equal.
 * \param rhs The right hand side of the comparison
 */
template< typename T > bool WMatrix< T >::operator==( const WMatrix& rhs ) const
{
    return WValue< T >::operator==( rhs ) && m_nbCols == rhs.m_nbCols;
}

/**
 * Compares two matrices and returns true if they are not equal.
 * \param rhs The right hand side of the comparison
 */
template< typename T > bool WMatrix< T >::operator!=( const WMatrix& rhs ) const
{
    return WValue< T >::operator!=( rhs ) || m_nbCols != rhs.m_nbCols;
}

/**
 * Assigns the argument WMatrix to this WMatrix.
 * \param rhs The right hand side of the assignment
 */
template< typename T > WMatrix< T >& WMatrix< T >::operator=( const WMatrix& rhs )
{
    WValue< T >::operator=( rhs );
    m_nbCols = rhs.m_nbCols;
    return *this;
}

/**
 * Returns the transposed matrix.
 */
template< typename T > WMatrix< T > WMatrix< T >::transposed() const
{
  WMatrix result( m_nbCols, getNbRows() );

  for ( std::size_t i = 0; i < getNbRows(); i++ )
    for ( std::size_t j = 0; j < m_nbCols; j++ )
      result( j, i ) = (*this)( i, j);
  return result;
}

template< typename T > WMatrix< T > WMatrix< T >::operator*( const WMatrix< T >& rhs ) const
{
    WAssert( rhs.getNbRows() == getNbCols(), "Incompatible number of rows of rhs and columns of lhs." );
    WMatrix< T > result( getNbRows(), rhs.getNbCols() );

    for( size_t r = 0; r < getNbRows(); ++r)
    {
        for( size_t c = 0; c < rhs.getNbCols(); ++c )
        {
            for( size_t i = 0; i < getNbCols(); ++i )
            {
                result( r, c ) += ( *this )( r, i ) * rhs( i, c );
            }
        }
    }
    return result;
}

template< typename T > WValue< T > WMatrix< T >::operator*( const WValue< T >& rhs ) const
{
    WAssert( rhs.size() == getNbCols(), "Incompatible number of rows of rhs and columns of lhs." );
    WValue< T > result( getNbRows() );

    for( size_t r = 0; r < getNbRows(); ++r)
    {
        for( size_t i = 0; i < getNbCols(); ++i )
        {
            result[r] += ( *this )( r, i ) * rhs[i];
        }
    }
    return result;
}

template< typename T > WVector3D WMatrix< T >::operator*( const WVector3D& rhs ) const
{
    WAssert( rhs.num_components == getNbCols(), "Incompatible number of rows of rhs and columns of lhs." );
    WVector3D result;

    for( size_t r = 0; r < getNbRows(); ++r)
    {
        for( size_t i = 0; i < getNbCols(); ++i )
        {
            result[r] += ( *this )( r, i ) * rhs[i];
        }
    }
    return result;
}

template< typename T >
inline std::ostream& operator<<( std::ostream& os, const WMatrix< T >& m )
{
    os << std::setprecision( 5 ) << std::fixed;
    for( size_t i = 0; i < m.getNbRows(); ++i )
    {
        if( i == 0 )
        {
            os << "[ ";
        }
        else
        {
            os << "  ";
        }
        for( size_t j = 0; j < m.getNbCols(); ++j )
        {
            os << std::setw( 12 ) << m( i, j );
            if( j < m.getNbCols() - 1 )
            {
                os << ", ";
            }
            else if( i < m.getNbRows() - 1 )
            {
                os << "  ";
            }
            else
            {
                os << " ]";
            }
        }
        os << std::endl;
    }
    return os;
}

#endif  // WMATRIX_H
