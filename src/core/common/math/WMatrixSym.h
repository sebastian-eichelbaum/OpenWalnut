//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#ifndef WMATRIXSYM_H
#define WMATRIXSYM_H

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../exceptions/WOutOfBounds.h"

/**
 * Symmetric square matrix, storing only the elements of the triangular matrix without the main
 * diagonal. So in case of a NxN matrix there are only (N^2-N)/2 elements stored.
 *
 * \note There exists also a WWriter and WReader for storing/reading the matrix in VTK file format.
 */
template< typename T >
class WMatrixSym
{
friend class WMatrixSymTest; //!< Access for test class.
public:
    /**
     * Type of stored elements.
     */
    typedef T value_type;

    /**
     * Shorthand for shared pointers.
     */
    typedef boost::shared_ptr< WMatrixSym < T > > SPtr;

    /**
     * Generates new symmetric matrix.
     *
     * \param n number of rows and cols
     */
    explicit WMatrixSym( size_t n );

    /**
     * Default constructor leaving all empty.
     */
    WMatrixSym();

    /**
     * Element acces operator as if the elements where stored as a normal matrix.
     *
     * \warning Acessing elements of the main diagonal is forbidden!
     *
     * \param i The i'th row
     * \param j The j'th column
     *
     * \return reference to the (i,j) element of the table
     */
    T& operator()( size_t i, size_t j );

    /**
     * Const version of the element access.
     *
     * \warning Acessing elements of the main diagonal is forbidden!
     *
     * \param i The i'th row
     * \param j The j'th column
     *
     * \return Const reference to the (i,j) element of the table
     */
    const T& operator()( size_t i, size_t j ) const;

    /**
     * Returns the number of elements stored in this matrix.
     * \return the number of elements stored in this matrix.
     */
    size_t numElements() const;

    /**
     * Returns the number of rows and cols of the matrix.
     * \return The number of rows and cols of the matrix.
     */
    size_t size() const;

    /**
     * Returns the elements stored inside of this container.
     *
     * \return Read-only reference to the elements stored inside this container.
     */
    const std::vector< T >& getData() const;

    /**
     * Resets the internal data to the given vector of elements.
     *
     * \param data new data in row major arrangement
     */
    void setData( const std::vector< T > &data );

    /**
     * Renders the matrix to a full nxn matrix, where the main diagonal is set to 0.0 and the m(i,j) == m(j,i).
     * Each column is separated by exactly one space and each row is separated by a newline.
     *
     * @return Multiline string containing the nxn symmetric matrix.
     */
    std::string toString( void ) const;

private:
    /**
     * Internal data structure to store the elements. The order is row major.
     */
    std::vector< T > m_data;

    /**
     * Number of rows and cols.
     */
    size_t m_n;
};

template< typename T >
inline WMatrixSym< T >::WMatrixSym( size_t n )
    : m_data( ( n * ( n - 1 ) ) / 2, 0.0 ),
      m_n( n )
{
}

template< typename T >
inline WMatrixSym< T >::WMatrixSym()
    : m_n( 0 )
{
}

template< typename T >
inline const T& WMatrixSym< T >::operator()( size_t i, size_t j ) const
{
    if( i == j || i >= m_n || j >= m_n )
    {
        std::stringstream ss;
        ss << "Invalid Element Access ( " << i << ", " << j << " ). No diagonal elements or indices bigger than " << m_n << " are allowed.";
        throw WOutOfBounds( ss.str() );
    }
    if( i > j )
    {
        std::swap( i, j );
    }
    return m_data[( i * m_n + j - ( i + 1 ) * ( i + 2 ) / 2 )];
}


template< typename T >
inline T& WMatrixSym< T >::operator()( size_t i, size_t j )
{
    if( i == j || i >= m_n || j >= m_n )
    {
        std::stringstream ss;
        ss << "Invalid Element Access ( " << i << ", " << j << " ). No diagonal elements or indices bigger than " << m_n << " are allowed.";
        throw WOutOfBounds( ss.str() );
    }
    if( i > j )
    {
        std::swap( i, j );
    }
    return m_data[( i * m_n + j - ( i + 1 ) * ( i + 2 ) / 2 )];
}


template< typename T >
inline std::string WMatrixSym< T >::toString( void ) const
{
  std::stringstream ss;
  ss << std::setprecision( 9 ) << std::fixed;
  ss << *this;
  return ss.str();
}

/**
 * Compares two matrix elementwise. First tested their size, if equal elementwisely further tests.
 *
 * \tparam T Element type
 * \param lhs Left symmetric matrix operand
 * \param rhs Right symmetric matrix operand
 *
 * \return True if and only if all elements are equal.
 */
template< typename T >
inline bool operator==( WMatrixSym< T > const& lhs, WMatrixSym< T > const& rhs )
{
    std::vector< T > l = lhs.getData();
    std::vector< T > r = rhs.getData();
    return l == r;
}

/**
 * Output operator for symmetric Matrix producing full square matrix. Each row in separate line.
 *
 * \tparam T Element type
 * \param os Output stream
 * \param m Matrix to put to output stream
 *
 * \return Output stream
 */
template< typename T >
inline std::ostream& operator<<( std::ostream& os, const WMatrixSym< T >& m )
{
    size_t n = m.size();
    for( size_t i = 0; i < n; ++i )
    {
        for( size_t j = 0; j < n; ++j )
        {
            if( i != j )
            {
                os << m( i, j );
            }
            else
            {
                os << 0.0;
            }
            if( ( j + 1 ) < n )
            {
                os << " ";
            }
        }
        if( ( i + 1 ) < n )
        {
            os << std::endl;
        }
    }
    return os;
}

/**
 * Read elemnts of full square matrix into symmetric matrix. Only elements of the upper triangle matrix will be used.
 * First all elements separated by white space are read and then dimension check is performed, to ensure all
 * elements fit into sym Matrix then.
 *
 * \throw WOutOfBounds exception if element number missmatch occours.
 *
 * \tparam T Element type
 * \param is Input Stream
 * \param m Sym. Matrix to populate
 *
 * \return Input Stream
 */
template< typename T >
inline std::istream& operator>>( std::istream& is, WMatrixSym< T >& m )
{
    std::vector< T > elements;
    T elm;
    while( is >> elm )
    {
        elements.push_back( elm );
    }
    if( m.size() * m.size() != elements.size() )
    {
        std::stringstream ss;
        ss << "Error: Input stream has: " << elements.size() << " elements, while matrix given to accommodate expected: ";
        ss << m.size() * m.size() << " elements.";
        throw WOutOfBounds( ss.str() );
    }
    typename std::vector< T >::const_iterator it = elements.begin();
    for( size_t i = 0; i < m.size(); ++i )
    {
        for( size_t j = 0; j < m.size(); ++j )
        {
            if( j > i )
            {
                m( i, j ) = *it;
            }
            ++it;
        }
    }
    return is;
}

template< typename T >
inline size_t WMatrixSym< T >::numElements() const
{
    return m_data.size();
}

template< typename T >
inline size_t WMatrixSym< T >::size() const
{
    return m_n;
}

template< typename T >
inline const typename std::vector< T >& WMatrixSym< T >::getData() const
{
    return m_data;
}

template< typename T >
inline void WMatrixSym< T >::setData( const std::vector< T > &data )
{
    if( m_n * ( m_n - 1 ) / 2 != data.size() )
    {
        std::stringstream ss;
        ss << "Data vector length: " << data.size() << " doesn't fit to number of rows and cols: " << m_n;
        throw WOutOfBounds( ss.str() );
    }
    m_data = std::vector< T >( data ); // copy content
}

typedef WMatrixSym< double > WMatrixSymDBL;
typedef WMatrixSym< float > WMatrixSymFLT;

#endif  // WMATRIXSYM_H
