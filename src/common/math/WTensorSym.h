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

#ifndef WTENSORSYM_H
#define WTENSORSYM_H

#include <vector>
#include <iostream>

#include "WTensor.h"

namespace wmath
{
// ############################# class WTensorSym<> #############################################

/**
 * Implements a symmetric tensor that has the same number of components in every
 * direction. A symmetric tensor has the same value for every permutation of the indices.
 *
 * For example, t(i,j) = t(j,i) for a tensor of order 2, and t(i,j,k) = t(j,i,k) = t(i,k,j)
 * = t(j,k,i) = t(k,i,j) = t(k,j,i) for a tensor of order 3.
 *
 * The first template parameter is the order of the tensor. 
 * The second template parameter is the dimension of the tensor, i.e. the number of components
 * in each direction.
 * The third template parameter is the datatype of the components, which is double by default.
 * 
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during operator =.
 *
 * Access to specific elements of the tensor can be achieved in 2 ways:
 *
 * - operator (), whose number of parameters matches the template parameter order.
 * - operator [], whose parameter is either an array or a std::vector of appropriate size.
 *
 * \note The datatype of the array or std::vector can be any type castable to std::size_t.
 * \note There is no bounds checking for the array version of operator [].
 * \note Operator () is not supported for orders larger than 6.
 *
 * This class optimizes memory usage. For example, for a symmetric tensor of order 2 and
 * dimension 3, only 6 values (instead of 9) need to be stored. However, there is additional
 * memory overhead per class (i.e. per allocation of the template parameters), which is of
 * constant size and thus does not depend on the number of instances.
 * 
 * Usage and operators are the same as with WTensor. Note, that changes to an
 * element t(i,j,k,...) also change every element whose indices are a permutation of i,j,k... .
 * \see WTensor 
 */
template< std::size_t order, std::size_t dim, typename Data_T = double >
class WTensorSym : public WTensorFunc< WTensorBaseSym, order, dim, Data_T >
{
};

//template< std::size_t order, typename Data_T >
//WTensorSym< order, Data_T >::operator WTensor< order, T > () const
//{
//    return WTensor< order, T >( *this );
//}

// ###################### special case WTensorSym< T, 0 > ##############################

/**
 * The order 0 version of WTensorSym. This essentialy encapsulates a scalar.
 */
template< std::size_t dim, typename Data_T >
class WTensorSym< 0, dim, Data_T >
{
public:
    /**
     * Standard constructor.
     */
    WTensorSym();

    /**
     * Get the dimension of this tensor.
     *
     * \return The dimension of this tensor.
     */
    std::size_t getDimension() const;

    /**
     * Get the order of this tensor.
     *
     * \return The order of this tensor.
     */
    std::size_t getOrder() const;

    /**
     * Access via std::vector of indices. Only for compatibility.
     *
     * \param indices A std::vector of indices.
     *
     * \return A reference to the value of this tensor.
     */
    template< typename Index_T >
    Data_T& operator[] ( std::vector< Index_T > const& indices );

    /**
     * Access via std::vector of indices. Only for compatibility.
     *
     * \param indices A std::vector of indices.
     *
     * \return A reference to the value of this tensor.
     */
    template< typename Index_T >
    Data_T const& operator[] ( std::vector< Index_T > const& indices ) const;

    /**
     * Access operator.
     *
     * \return A reference to the value of this tensor.
     */
    Data_T& operator() ();

    /**
     * Access operator.
     *
     * \return A reference to the value of this tensor.
     */
    Data_T const& operator() () const;

//    operator WTensor< 0, dim, Data_T > () const;

private:
    /**
     * The data element.
     */
    Data_T m_data;
};

template< std::size_t dim, typename Data_T >
WTensorSym< 0, dim, Data_T >::WTensorSym()
    : m_data( Data_T() )
{
}

template< std::size_t dim, typename Data_T >
std::size_t WTensorSym< 0, dim, Data_T >::getDimension() const
{
    return 0;
}

template< std::size_t dim, typename Data_T >
std::size_t WTensorSym< 0, dim, Data_T >::getOrder() const
{
    return 0;
}

template< std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorSym< 0, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices )
{
    return m_data;
}

template< std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorSym< 0, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices ) const
{
    return m_data;
}

template< std::size_t dim, typename Data_T >
Data_T& WTensorSym< 0, dim, Data_T >::operator() ()
{
    return m_data;
}

template< std::size_t dim, typename Data_T >
Data_T const& WTensorSym< 0, dim, Data_T >::operator() () const
{
    return m_data;
}

//template< std::size_t dim, typename Data_T >
//WTensorSym< 0, dim, Data_T >::operator WTensor< 0, dim, Data_T > () const
//{
//    WTensor< 0, dim, Data_T > t;
//    t() = m_data;
//    return t;
//}

}  // namespace wmath

#endif  // WTENSORSYM_H
