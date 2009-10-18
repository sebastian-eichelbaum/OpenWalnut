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

#include "WValue.hpp"

namespace wmath
{
/**
 * Matrix template class with variable number of rows and columns.
 */
template< typename T > class WMatrix : WValue< T >
{
public:
    /**
     * Produces a matrix with the given number of components.
     * The components will be set to zero if T is a type representing numbers.
     */
    explicit WMatrix( size_t nbRows, size_t nbCols )
        : WValue< T >( nbRows * nbCols )
    {
        m_nbCols = nbCols;
    }

    /**
     * Produces a matrix as copy of the one given as parameter.
     */
    WMatrix( const WMatrix& newMatrix )
        : WValue< T >( newMatrix )
    {
        m_nbCols = newMatrix.m_nbCols;
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
     * Returns a reference to the component an row i, colums j in order to
     * provide access to the component.
     */
    T& operator()( size_t i, size_t j )
    {
        assert( j < m_nbCols && i * m_nbCols < this->size() );
        return (*this)[i * m_nbCols + j];
    }

    /**
     * Returns a const reference to the component an row i, colums j in order to
     * provide read-only access to the component.
     */
    const T& operator()( size_t i, size_t j ) const
    {
        assert( j < m_nbCols && i * m_nbCols < this->size() );
        return (*this)[i * m_nbCols + j];
    }

    /**
     * Compares two matrices and returns true if they are equal.
     */
    bool operator==( const WMatrix& rhs ) const
    {
        return WValue< T >::operator==( rhs ) && m_nbCols == rhs.m_nbCols;
    }

    /**
     * Compares two matrices and returns true if they are not equal.
     */
    bool operator!=( const WMatrix& rhs ) const
    {
        return WValue< T >::operator!=( rhs ) || m_nbCols != rhs.m_nbCols;
    }

    /**
     * Assigns the argument WMatrix to this WMatrix.
     */
    WMatrix& operator=( const WMatrix& rhs )
    {
        WValue< T >::operator=( rhs );
        m_nbCols = rhs.m_nbCols;
        return *this;
    }

protected:
private:
    size_t m_nbCols;
};
}  // End of namespace
#endif  // WMATRIX_H
