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

#include <osg/Matrix>

#include "WValue.h"
#include "linearAlgebra/WLinearAlgebra.h"

#include "../WDefines.h"

/**
 * Matrix template class with variable number of rows and columns.
 * The access function are row-major, which means that the rows
 * are the first parameter or index.
 */
template< typename T > class WMatrix : public WValue< T >
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
    WMatrix( const WMatrix4d& newMatrix ); // NOLINT

    /**
     * Copies the specified Eigen::MatrixXd.
     *
     * \param newMatrix the Eigen::MatrixXd matrix to copy
     */
    WMatrix( const Eigen::MatrixXd& newMatrix ); // NOLINT

    /**
     * Makes the matrix contain the identity matrix, i.e. 1 on the diagonal.
     * \return Reference to the current matrix which is identity matrix now.
     */
    WMatrix& makeIdentity();

    /**
     * Get number of rows.
     * \return Number of rows of the matrix.
     */
    size_t getNbRows() const;

    /**
     * Get number of columns.
     * \return Number of columns of the matrix.
     */
    size_t getNbCols() const;

    /**
     * Returns a reference to the component an row i, columns j in order to
     * provide access to the component.
     * \param i row
     * \param j column
     * \return A reference to the component (i,j)
     */
    T& operator()( size_t i, size_t j );

    /**
     * Returns a const reference to the component an row i, columns j in order to
     * provide read-only access to the component.
     * \param i row
     * \param j column
     * \return A const reference to the component (i,j)
     */
    const T& operator()( size_t i, size_t j ) const;

    /**
     * Cast this matrix to an 4x matrix if it is a 4x4 matrix.
     *
     * \return casted matrix
     */
    operator WMatrix4d() const;

    /**
     * Cast this matrix to an 4x4 osg matrix if it is a 4x4 matrix.
     *
     * \return casted matrix.
     */
    operator osg::Matrixd() const;

    /**
     * Cast this matrix to an Eigen::MatrixXd matrix.
     *
     * \return casted matrix.
     */
    operator Eigen::MatrixXd() const;

    /**
     * Compares two matrices and returns true if they are equal.
     * \param rhs The right hand side of the comparison
     * \return Are the matrices equal?
     */
    bool operator==( const WMatrix& rhs ) const;

    /**
     * Compares two matrices and returns true if they are not equal.
     * \param rhs The right hand side of the comparison
     * \return Are the matrices NOT equal?
     */
    bool operator!=( const WMatrix& rhs ) const;

    /**
     * Assigns the argument WMatrix to this WMatrix.
     * \param rhs The right hand side of the assignment
     * \return A reference to the left hand side of the assignment (i.e. the current object).
     */
    WMatrix& operator=( const WMatrix& rhs );

    /**
     * Multiplication of the current matrix with andother matrix.
     * \param rhs The right hand side of the multiplication
     * \return The product of the two matrices.
     */
    WMatrix operator*( const WMatrix& rhs ) const;

    /**
     * Multiplication with a vector.
     * \param rhs The right hand side of the multiplication
     * \return The product of the matrix and the vector.
     */
    WValue< T > operator*( const WValue< T >& rhs ) const;

    /**
     * Multiplication with a vector.
     * \param rhs The right hand side of the multiplication
     * \return The product of the matrix and the 3D vector.
     */
    WVector3d operator*( const WVector3d& rhs ) const;

    /**
     * Returns the transposed matrix.
     * \return Transposed version of the current matrix.
     */
    WMatrix transposed() const;

    /**
     * Resets the matrix components to zero.
     */
    void setZero()
    {
        for( size_t i = 0; i < this->size(); ++i )
        {
            ( *this )[ i ] = 0.0;
        }
    }

    /**
     * Returns true if the matrix is a square matrix.
     * \return true for square matrixes, otherwise false.
     */
    bool isSquare() const;

    /**
     * Returns true if the matrix is a identity matrix.
     * \param delta - tolerance parameter when checking the values.
     * \return true for identity matrixes, otherwise false.
     */
    bool isIdentity( T delta = T( 0.0 ) ) const;

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

template< typename T > WMatrix< T >::WMatrix( const WMatrix4d& newMatrix )
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

template< typename T > WMatrix< T >::WMatrix( const Eigen::MatrixXd& newMatrix )
    : WValue< T >( newMatrix.cols() * newMatrix.rows() )
{
    m_nbCols = static_cast< size_t >( newMatrix.cols() );
    for( int row = 0; row < newMatrix.rows(); ++row )
    {
        for( int col = 0; col < newMatrix.cols(); ++col )
        {
            ( *this )( row, col ) = static_cast< T >( newMatrix( row, col ) );
        }
    }
}


template< typename T > WMatrix< T >::operator WMatrix4d() const
{
    size_t nbRows = this->size() / m_nbCols;
    WAssert( m_nbCols == 4 && nbRows == 4, "This is no 4x4 matrix." );
    WMatrix4d m;
    for( size_t i = 0; i < nbRows; ++i )
    {
        for( size_t j = 0; j < m_nbCols; ++j )
        {
            m( i, j ) = ( *this )( i, j );
        }
    }
    return m;
}

template< typename T > WMatrix< T >::operator osg::Matrixd() const
{
    WAssert( ( getNbRows() == 3 || getNbRows() == 4 ) && ( getNbCols() == 3 || getNbCols() == 4 ),
             "Only 3x3 or 4x4 matrices allowed." );

    // handle 4x4 and 3x3 separately
    if( getNbRows() == 4 )
    {
        return osg::Matrixd( ( *this )[ 0 ], ( *this )[ 4 ], ( *this )[ 8 ], ( *this )[ 12 ],
                             ( *this )[ 1 ], ( *this )[ 5 ], ( *this )[ 9 ], ( *this )[ 13 ],
                             ( *this )[ 2 ], ( *this )[ 6 ], ( *this )[ 10 ], ( *this )[ 14 ],
                             ( *this )[ 3 ], ( *this )[ 7 ], ( *this )[ 11 ], ( *this )[ 15 ]
                           );
    }
    else
    {
        return osg::Matrixd( ( *this )[ 0 ], ( *this )[ 1 ], ( *this )[ 2 ], 0.0,
                             ( *this )[ 3 ], ( *this )[ 4 ], ( *this )[ 5 ], 0.0,
                             ( *this )[ 6 ], ( *this )[ 7 ], ( *this )[ 8 ], 0.0,
                             ( *this )[ 9 ], ( *this )[ 10 ], ( *this )[ 11 ], 1.0
                           );
    }
}

template< typename T > WMatrix< T >::operator Eigen::MatrixXd() const
{
    Eigen::MatrixXd matrix( this->getNbRows(), this->getNbCols() );
    for( int row = 0; row < matrix.rows(); ++row )
    {
        for( int col = 0; col < matrix.cols(); ++col )
        {
            matrix( row, col ) = ( *this )( row, col );
        }
    }
    return matrix;
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

  for( std::size_t i = 0; i < getNbRows(); i++ )
    for( std::size_t j = 0; j < m_nbCols; j++ )
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

template< typename T > WVector3d WMatrix< T >::operator*( const WVector3d& rhs ) const
{
    WAssert( rhs.getRows() == getNbCols(), "Incompatible number of rows of rhs and columns of lhs." );
    WVector3d result;

    for( size_t r = 0; r < getNbRows(); ++r)
    {
        for( size_t i = 0; i < getNbCols(); ++i )
        {
            result[r] += ( *this )( r, i ) * rhs[i];
        }
    }
    return result;
}

template< typename T > bool WMatrix< T >::isSquare() const
{
    return getNbRows() == getNbCols();
}

template< typename T > bool WMatrix< T >::isIdentity( T delta ) const
{
    if( !isSquare() )
    {
        return false;
    }

    for( size_t row = 0; row < getNbRows(); row++ )
    {
        for( size_t col = 0; col < getNbCols(); col++ )
        {
            T val = ( *this )( row, col );
            T expected = ( row == col ? T( 1.0 ) : T( 0.0 ) );
            if( std::fabs( val - expected ) > delta )
            {
                return false;
            }
        }
    }
    return true;
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
