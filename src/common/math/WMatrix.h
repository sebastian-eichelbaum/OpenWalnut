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

#include "WValue.h"
#include "WVector3D.h"

namespace wmath
{
/**
 * Matrix template class with variable number of rows and columns.
 * The access function are row-major, which means that the rows
 * are the first parameter or index.
 */
template< typename T > class WMatrix : public WValue< T >
{
public:
    /**
     * Produces a matrix with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     * \param nbRows number of rows in the matrix
     * \param nbCols number of columns in the matrix
     */
    explicit WMatrix( size_t nbRows, size_t nbCols )
        : WValue< T >( nbRows * nbCols )
    {
        m_nbCols = nbCols;
    }

    /**
     * Produces a matrix as copy of the one given as parameter.
     * \param newMatrix The matrix to be copied.
     */
    WMatrix( const WMatrix& newMatrix )
        : WValue< T >( newMatrix )
    {
        m_nbCols = newMatrix.m_nbCols;
    }

    /**
     * Makes the matrix contain the identity matrix, i.e. 1 on the diagonal.
     */
    WMatrix& makeIdentity()
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
    size_t getNbRows() const
    {
        return this->size() / m_nbCols;
    }

    /**
     * Get number of columns.
     */
    size_t getNbCols() const
    {
        return m_nbCols;
    }

    /**
     * Returns a reference to the component an row i, columns j in order to
     * provide access to the component.
     * \param i row
     * \param j column
     */
    T& operator()( size_t i, size_t j )
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
    const T& operator()( size_t i, size_t j ) const
    {
        WAssert( j < m_nbCols && i * m_nbCols < this->size(), "Index out of bounds." );
        return (*this)[i * m_nbCols + j];
    }

    /**
     * Compares two matrices and returns true if they are equal.
     * \param rhs The right hand side of the comparison
     */
    bool operator==( const WMatrix& rhs ) const
    {
        return WValue< T >::operator==( rhs ) && m_nbCols == rhs.m_nbCols;
    }

    /**
     * Compares two matrices and returns true if they are not equal.
     * \param rhs The right hand side of the comparison
     */
    bool operator!=( const WMatrix& rhs ) const
    {
        return WValue< T >::operator!=( rhs ) || m_nbCols != rhs.m_nbCols;
    }

    /**
     * Assigns the argument WMatrix to this WMatrix.
     * \param rhs The right hand side of the assignment
     */
    WMatrix& operator=( const WMatrix& rhs )
    {
        WValue< T >::operator=( rhs );
        m_nbCols = rhs.m_nbCols;
        return *this;
    }

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
    WMatrix transposed() const
    {
      WMatrix result( m_nbCols, getNbRows() );

      for ( std::size_t i = 0; i < getNbRows(); i++ )
        for ( std::size_t j = 0; j < m_nbCols; j++ )
          result( j, i ) = (*this)( i, j);
      return result;
    }

protected:
private:
    size_t m_nbCols; //!< Number of columns of the matrix. The number of rows will be computed by (size/m_nbCols).
};


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

}  // End of namespace
#endif  // WMATRIX_H
