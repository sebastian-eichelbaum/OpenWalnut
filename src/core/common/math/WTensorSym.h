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

#include <iostream>
#include <vector>

#include "WTensorBase.h"

// ############################# class WTensorSym<> #############################################

/**
 * Implements a symmetric tensor that has the same number of components in every
 * direction. A symmetric tensor has the same value for every permutation of the indices.
 *
 * For example, t(i,j) = t(j,i) for a tensor of order 2, and t(i,j,k) = t(j,i,k) = t(i,k,j)
 * = t(j,k,i) = t(k,i,j) = t(k,j,i) for a tensor of order 3.
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
 * This class optimizes memory usage. For example, for a symmetric tensor of order 2 and
 * dimension 3, only 6 values (instead of 9) need to be stored. However, there is additional
 * memory overhead per class (i.e. per allocation of the template parameters), which is of
 * constant size and thus does not depend on the number of instances.
 *
 * Usage and operators are the same as with WTensor. Note that changes to an
 * element t(i,j,k,...) also change every element whose indices are a permutation of i,j,k... .
 * \see WTensor
 */
template< std::size_t order, std::size_t dim, typename Data_T = double >
class WTensorSym : public WTensorFunc< WTensorBaseSym, order, dim, Data_T >
{
public:
    /**
     * Default constructor of the symmetric tensor.
     */
    WTensorSym();

    /**
     * Constructs and initializes the symmetrical Tensor with a WValue.
     *
     * \note The same ordering as for the data member is required.
     *
     * \param data The components in same ordering as for the data member \c m_data is required, (\see m_data).
     */
    explicit WTensorSym( const WValue< Data_T >& data );

protected:
private:
};

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorSym< order, dim, Data_T >::WTensorSym()
    : WTensorFunc< WTensorBaseSym, order, dim, Data_T >()
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorSym< order, dim, Data_T >::WTensorSym( const WValue< Data_T >& data )
    : WTensorFunc< WTensorBaseSym, order, dim, Data_T >( data )
{
}
// ######################## stream output operators #################################

/**
 * Write a symmetric tensor of order 0 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensorSym.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensorSym< 0, dim, Data_T > const& t )
{
    o << t() << std::endl;
    return o;
}

/**
 * Write a symmetric tensor of order 1 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensorSym.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensorSym< 1, dim, Data_T > const& t )
{
    for( std::size_t k = 0; k < dim; ++k )
    {
        o << t( k ) << " ";
    }
    o << std::endl;
    return o;
}

/**
 * Write a symmetric tensor of order 2 to an output stream.
 *
 * \param o An output stream.
 * \param t A WTensorSym.
 *
 * \return The output stream.
 */
template< std::size_t dim, typename Data_T >
std::ostream& operator << ( std::ostream& o, WTensorSym< 2, dim, Data_T > const& t )
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
// ######################## a utility function #################################

/**
 * This calculates the multiplicity of the elements of a 3-dimensional
 * symmetric tensor. (see Özarslan's paper from 2003)
 * In a (super-)symmetric tensor, all permutations of a given set of Indices
 * i_1, i_2, ... i_order ( i_j in 0,1,2 ) refer to the same data element.
 * This means each data element can be identified by the amount of 0's, 1's
 * and 2's in its index list. Permutations do not change these amounts, thus
 * different data elements must differ in amount of 0's, 1's and 2's. The number of
 * permutations that exist on the index list of a data element is its multiplicity.
 *
 * \param order The order of the tensor.
 * \param numZeros How many elements of the permutation equal 0.
 * \param numOnes How many elements of the permutation equal 1.
 * \param numTwos How many elements of the permutation equal 2.
 */
std::size_t calcSupersymmetricTensorMultiplicity( std::size_t order, std::size_t numZeros, std::size_t numOnes, std::size_t numTwos );

#endif  // WTENSORSYM_H
