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

#ifndef WTENSOR_H
#define WTENSOR_H

#include <iostream>
#include <vector>

#include "WTensorSym.h"

// ############################# class WTensor<> #############################################
/**
 * Implements a tensor that has the same number of components in every
 * direction.
 *
 * \tparam order The order of the tensor.
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 *
 * \note The dimension may never be 0.
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during any assignment operator.
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
 * Examples:
 *
 * - Construct a tensor of order 2 and dimension 3 (i.e. a 3x3-Matrix):
 *
 *     WTensor< 2, 3 > w;
 *
 * - Change Element (2,0) to 4.0:
 *
 *     w( 2, 0 ) = 4.0;
 *
 * - Construct a 4D-vector:
 *
 *     WTensor< 1, 4 > v;
 *
 * - Access v at position 2:
 *
 *     v( 2 ) = ...;
 *
 *     std::vector< int > i( 1, 2 );
 *     v[ i ] = ...;
 */
template< std::size_t order, std::size_t dim, typename Data_T = double >
class WTensor : public WTensorFunc< WTensorBase, order, dim, Data_T >
{
public:
    /**
     * Standard constructor.
     */
    WTensor();

    /**
     * Construct a Tensor from a symmetric tensor.
     *
     * \param t A symmetric tensor.
     */
    WTensor( WTensorSym< order, dim, Data_T > const& t );

    /**
     * Copy from a symmetric tensor.
     *
     * \param t A symmetric tensor.
     *
     * \return new tensor
     */
    WTensor const& operator = ( WTensorSym< order, dim, Data_T > const& t );
};

template< std::size_t order, std::size_t dim, typename Data_T >
WTensor< order, dim, Data_T >::WTensor()
    : WTensorFunc< WTensorBase, order, dim, Data_T >()
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensor< order, dim, Data_T >::WTensor( WTensorSym< order, dim, Data_T > const& t )
    : WTensorFunc< WTensorBase, order, dim, Data_T >()
{
    WTensorBase< order, dim, Data_T >::operator = ( t );
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensor< order, dim, Data_T > const& WTensor< order, dim, Data_T >::operator = ( WTensorSym< order, dim, Data_T > const& t )
{
    WTensorBase< order, dim, Data_T >::operator = ( t );
    return *this;
}

// ######################## stream output operators #################################

/**
 * Write a tensor of order 0 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensor.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensor< 0, dim, Data_T > const& t )
{
    o << t() << std::endl;
    return o;
}

/**
 * Write a tensor of order 1 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensor.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensor< 1, dim, Data_T > const& t )
{
    for( std::size_t k = 0; k < dim; ++k )
    {
        o << t( k ) << " ";
    }
    o << std::endl;
    return o;
}

/**
 * Write a tensor of order 2 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensor.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensor< 2, dim, Data_T > const& t )
{
    for( std::size_t k = 0; k < dim; ++k )
    {
        for( std::size_t l = 0; l < dim; ++l )
        {
            o << t( k, l ) << " ";
        }
        o << std::endl;
    }
    return o;
}

#endif  // WTENSOR_H
