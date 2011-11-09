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

#ifndef WTENSORBASE_H
#define WTENSORBASE_H

#include <algorithm>
#include <map>
#include <vector>

#include <boost/static_assert.hpp>
#include <boost/array.hpp>

#include "../WAssert.h"
#include "WCompileTimeFunctions.h"
#include "WMatrix.h"
#include "WValue.h"

// TODO(reichenbach): Remove vectors (because of the enum dataSize).

// forward declaration of the test classes
class WTensorFuncTest;
class WTensorBaseTest;
class WTensorBaseSymTest;

// forward declaration
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBaseSym;

// ############################ utility functions ####################################

/**
 * Iterate a position in a multidimensional grid.
 *
 * This essentially creates a linear order on all positions in a tensor, where a position
 * is any valid allocation of indices.
 *
 * Example: for a tensor of order 2 and dimension 3, all possible positions are:
 *
 * ( 0, 0 ), ( 0, 1 ), ( 0, 2 ), ( 1, 0 ), ( 1, 1 ), ( 1, 2 ), ( 2, 0 ), ( 2, 1 ), ( 2, 2 )
 *
 * \param pos The position to be iterated.
 */
template< std::size_t order, std::size_t dim >
inline void positionIterateOneStep( boost::array< std::size_t, order >& pos ) // NOLINT, need a reference here
{
    WAssert( pos.size() >= order, "" );

    for( std::size_t k = order - 1; k > 0; --k )
    {
        if( pos[ k ] == dim - 1)
        {
            pos[ k ] = 0;
        }
        else
        {
            ++( pos[ k ] );
            return;
        }
    }
    ++( pos[ 0 ] );
}

/**
 * Iterate a sorted position in a multidimensional grid.
 *
 * This essentially creates a linear order on all sorted positions in a tensor, where a sorted position
 * is any valid allocation of indices where those indices are in ascending order.
 *
 * Example: for a tensor of order 2 and dimension 3, all possible sorted positions are:
 *
 * ( 0, 0 ), ( 0, 1 ), ( 0, 2 ), ( 1, 1 ), ( 1, 2 ), ( 2, 2 )
 *
 * The number of sorted positions of a tensor matches the number of elements that need to be stored
 * by the symmetric tensor of the same order and dimension.
 *
 * \param pos The sorted(!) position to be iterated.
 */
template< std::size_t order, std::size_t dim >
inline void positionIterateSortedOneStep( boost::array< std::size_t, order >& pos ) // NOLINT, need a reference here
{
    WAssert( pos.size() >= order, "" );

    for( int k = order - 1; k > -1; --k )
    {
        if( pos[ k ] != dim - 1 )
        {
            ++( pos[ k ] );
            for( std::size_t i = k + 1; i < order; ++i )
            {
                pos[ i ] = pos[ k ];
            }
            return;
        }
    }
}

/**
 * Same as the version above, using no template arguments.
 *
 * \param order The order of the tensor.
 * \param dim The dimension of the tensor.
 * \param pos The sorted(!) position to be iterated.
 */
inline void positionIterateSortedOneStep( std::size_t order, std::size_t dim, std::vector< std::size_t >& pos ) // NOLINT, need a reference here
{
    WAssert( pos.size() >= order, "" );

    for( int k = order - 1; k > -1; --k )
    {
        if( pos[ k ] != dim - 1 )
        {
            ++( pos[ k ] );
            for( std::size_t i = k + 1; i < order; ++i )
            {
                pos[ i ] = pos[ k ];
            }
            return;
        }
    }
}

// ############################# class WTensorBase<> #################################
/**
 * Normal tensor base class.
 *
 * \tparam order The order of the tensor.
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 *
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during any assignment operator.
 * \note The dimension may not be 0.
 *
 * \see WTensor
 */
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBase
{
    /**
     * For dim == 0, create an artificial compiler error.
     */
    BOOST_STATIC_ASSERT( dim != 0 );

    // Make the appropriate tensor of order + 1 a friend, so it has access to the getPos() member.
    friend class WTensorBase< order + 1, dim, Data_T >;

    // make the test class a friend
    friend class ::WTensorBaseTest;

    // make the func test class a friend
    friend class ::WTensorFuncTest;

public:
    /**
     * Standard constructor.
     *
     * All elements are set to Data_T().
     */
    WTensorBase();

    /**
     * Copy constructor.
     *
     * \param t The tensor to copy from.
     */
    WTensorBase( WTensorBase const& t ); // NOLINT

    /**
     * Copy construct a WTensorBase from a WTensorBaseSym.
     *
     * \param t The symmetric tensor to copy from.
     */
    explicit WTensorBase( WTensorBaseSym< order, dim, Data_T > const& t );

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBase const& operator = ( WTensorBase const& t );

    /**
     * Copy operator.
     *
     * \param t The symmetric tensor to copy from.
     *
     * \return *this.
     */
    WTensorBase const& operator = ( WTensorBaseSym< order, dim, Data_T > const& t );

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
     * Get the element at a specific position.
     *
     * \param indices A std::vector of indices that has a size of at least order.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( std::vector< Index_T > const& indices );

    /**
     * Get the element at a specific position.
     *
     * \param indices A std::vector of indices that has a size of at least order.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( std::vector< Index_T > const& indices ) const;

    /**
     * Get the element at a specific position.
     *
     * \param indices An array of indices that has a size of at least order.
     *
     * \return A reference to the element.
     *
     * \note The array must have a length of at least order.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T indices[] );

    /**
     * Get the element at a specific position.
     *
     * \param indices An array of indices that has a size of at least order.
     *
     * \return A reference to the element.
     *
     * \note The array must have a length of at least order.
     */
    template< typename Index_T >
    Data_T const& operator[] ( Index_T indices[] ) const;

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBase const& other ) const;

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBase const& other ) const;

private:
    /**
     * Calculate the position of the element in the data vector. The function
     * is build recursively at compile-time.
     *
     * \param pos An array of indices.
     *
     * \return The position of the element.
     */
    template< typename Index_T >
    static inline std::size_t getPos( Index_T pos[] );

    /**
     * Declare a compile-time constant as enum and not as static constant.
     */
    enum
    {
        /**
         * The number of elements to store.
         */
        dataSize = WPower< dim, order >::value
    };

    //! Stores all elements.
    //std::vector< Data_T > m_data;
    boost::array< Data_T, dataSize > m_data;
};

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T >::WTensorBase()
{
    m_data.assign( Data_T() );
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T >::WTensorBase( WTensorBase const& t )
    : m_data( t.m_data )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T >::WTensorBase( WTensorBaseSym< order, dim, Data_T > const& t )
{
    *this = t;
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T > const& WTensorBase< order, dim, Data_T >::operator = ( WTensorBase const& t )
{
    m_data = t.m_data;
    return *this;
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T > const& WTensorBase< order, dim, Data_T >::operator = ( WTensorBaseSym< order, dim, Data_T > const& t )
{
    boost::array< std::size_t, order > pos;
    pos.assign( 0 );

    for( std::size_t k = 0; k < dataSize; ++k )
    {
        ( *this )[ &pos[ 0 ] ] = t[ &pos[ 0 ] ];
        positionIterateOneStep< order, dim >( pos );
    }

    return *this;
}

template< std::size_t order, std::size_t dim, typename Data_T >
std::size_t WTensorBase< order, dim, Data_T >::getDimension() const
{
    return dim;
}

template< std::size_t order, std::size_t dim, typename Data_T >
std::size_t WTensorBase< order, dim, Data_T >::getOrder() const
{
    return order;
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
std::size_t WTensorBase< order, dim, Data_T >::getPos( Index_T pos[] )
{
    return WTensorBase< order - 1, dim, Data_T >::getPos( pos ) * dim + static_cast< std::size_t >( pos[ order - 1 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBase< order, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices )
{
    return const_cast< Data_T& >( static_cast< WTensorBase const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBase< order, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices ) const
{
    WAssert( indices.size() >= order, "" );
    return operator[] ( &indices[ 0 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBase< order, dim, Data_T >::operator[] ( Index_T indices[] )
{
    return const_cast< Data_T& >( static_cast< WTensorBase const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBase< order, dim, Data_T >::operator[] ( Index_T indices[] ) const
{
    for( std::size_t k = 0; k < order; ++k )
    {
        WAssert( static_cast< std::size_t >( indices[ k ] ) < dim, "" );
    }
    std::size_t p = getPos( indices );
    return m_data[ p ];
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBase< order, dim, Data_T >::operator == ( WTensorBase const& other ) const
{
    return m_data == other.m_data;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBase< order, dim, Data_T >::operator != ( WTensorBase const& other ) const
{
    return m_data != other.m_data;
}

// ######################### WTensorBase for order == 0 ##########################

/**
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.

 * Specialization for order = 0. This essentially encapsulates a scalar. The purpose of
 * this specialization is compatibility for generic tensor functions.
 */
template< std::size_t dim, typename Data_T >
class WTensorBase< 0, dim, Data_T >
{
    // Make the appropriate tensor of order + 1 a friend, so it has access to the getPos() member.
    friend class WTensorBase< 1, dim, Data_T >;

    // make the test class a friend
    friend class ::WTensorBaseTest;

    // make the func test class a friend
    friend class ::WTensorFuncTest;

public:
    // implementations are inline as they are trivial
    /**
     * Standard constructor.
     *
     * All elements are set to Data_T().
     */
    WTensorBase()
        : m_data( Data_T() )
    {
    }

    /**
     * Copy constructor.
     *
     * \param t The tensor to copy from.
     */
    WTensorBase( WTensorBase const& t ) // NOLINT
        : m_data( t.m_data )
    {
    }

    /**
     * Copy construct a WTensorBase from a WTensorBaseSym.
     *
     * \param t The symmetric tensor to copy from.
     */
    explicit WTensorBase( WTensorBaseSym< 0, dim, Data_T > const& t )
        : m_data()
    {
        m_data = t.operator[]< std::size_t >( NULL );
    }

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBase const& operator = ( WTensorBase const& t )
    {
        m_data = t.m_data;
        return *this;
    }

    /**
     * Copy operator.
     *
     * \param t The symmetric tensor to copy from.
     *
     * \return *this.
     */
    WTensorBase const& operator = ( WTensorBaseSym< 0, dim, Data_T > const& t )
    {
        m_data = t.operator[]< std::size_t >( NULL );
        return *this;
    }

    /**
     * Get the dimension of this tensor.
     *
     * \return The dimension of this tensor.
     */
    std::size_t getDimension() const
    {
        return dim;
    }

    /**
     * Get the order of this tensor.
     *
     * \return The order of this tensor.
     */
    std::size_t getOrder() const
    {
        return 0;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( std::vector< Index_T > const& /* indices */ )
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( std::vector< Index_T > const& /* indices */ ) const
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T[] /* indices */ )
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( Index_T[] /* indices */ ) const
    {
        return m_data;
    }

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBase const& other ) const
    {
        return m_data == other.m_data;
    }

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBase const& other ) const
    {
        return m_data != other.m_data;
    }

private:
    /**
     * Calculate the position of the element in the data vector. This
     * is essentially the standard case of the recursion.
     *
     * \return 0.
     */
    template< typename Index_T >
    static inline std::size_t getPos( Index_T[] /* pos */ )
    {
        return 0;
    }

    /**
     * Stores the value.
     */
    Data_T m_data;

    /**
     * Declare a compile-time constant as enum and not as static constant.
     */
    enum
    {
        /**
         * The number of elements to store.
         */
        dataSize = 1
    };
};

// ################################# class WTensorBaseSym<> #####################################

/**
 * Symmetric tensor base class.
 *
 * \tparam order The order of the tensor.
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 *
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during any assignment operator.
 * \note The dimension may not be 0.
 *
 * \see WTensorSym
 */
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBaseSym
{
    /**
     * Declare a compile-time constant as enum and not as static constant.
     */
    enum
    {
        /**
         * The number of elements to store.
         */
        dataSize = WBinom< order + dim - 1, order >::value
    };

    /**
     * For dim == 0, create an artificial compiler error.
     */
    BOOST_STATIC_ASSERT( dim != 0 );

    // make the test class a friend
    friend class ::WTensorBaseSymTest;

    // make the func test class a friend
    friend class ::WTensorFuncTest;

public:
    /**
     * Standard constructor.
     *
     * All elements are set to Data_T().
     */
    WTensorBaseSym();

    /**
     * Constructs the symmetrical tensor and initialize with the given data.
     *
     * \param data The components of the symmetrical tensor: Take care of the
     * ordering of the components to match the ordering in \see m_data.
     */
    explicit WTensorBaseSym( const WValue< Data_T >& data );

    /**
     * Copy constructor.
     *
     * \param t The tensor to copy from.
     */
    WTensorBaseSym( WTensorBaseSym const& t ); // NOLINT

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBaseSym const& operator = ( WTensorBaseSym const& t );

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
     * Set internal data from a WValue.
     *
     * \param values The input values.
     */
    void setValues( WValue< Data_T > const values );

    /**
     * Get the element at a specific position.
     *
     * \param indices A std::vector of indices that has a size of at least order.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( std::vector< Index_T > const& indices );

    /**
     * Get the element at a specific position.
     *
     * \param indices A std::vector of indices that has a size of at least order.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( std::vector< Index_T > const& indices ) const;

    /**
     * Get the element at a specific position.
     *
     * \param indices An array of indices that has a size of at least order.
     *
     * \return A reference to the element.
     *
     * \note No bounds checking is performed.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T indices[] );

    /**
     * Get the element at a specific position.
     *
     * \param indices An array of indices that has a size of at least order.
     *
     * \return A reference to the element.
     *
     * \note No bounds checking is performed.
     */
    template< typename Index_T >
    Data_T const& operator[] ( Index_T indices[] ) const;

    /**
     * Compare this WTensorBaseSym to another one.
     *
     * \param other The WTensorBaseSym to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBaseSym const& other ) const;

    /**
     * Compare this WTensorBaseSym to another one.
     *
     * \param other The WTensorBaseSym to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBaseSym const& other ) const;

protected:
    /**
     * Stores the elements of this tensor lexicographical ordered on their
     * indices, where for each set of permutations the lexicographical lowest
     * index is used.
     */
    // std::vector< Data_T > m_data;
    boost::array< Data_T, dataSize > m_data;

private:
    /**
     * A class that maps symmetric tensor indices to vector positions.
     */
    class PositionIndexer
    {
    public:
        /**
         * Standard constructor. The mapping is calculated here.
         */
        PositionIndexer();

        /**
         * Get the mapped position.
         *
         * \param pos An array of indices.
         *
         * \return The position that corresponds to the indices.
         */
        template< typename Index_T >
        inline std::size_t operator[] ( Index_T pos[] ) const;

        /**
         * Declare a compile-time constant as enum and not as static constant.
         */
        enum
        {
            /**
             * The number of data elements.
             */
            dataSize = WBinom< order + dim - 1, order >::value
        };

    private:
        /**
         * Maps the indices to a vector element position.
         */
        WTensorBase< order, dim, std::size_t > m_positions;
    };

    /**
     * A static PositionIndexer that maps tensor indices to vector positions.
     */
    static PositionIndexer const m_indexer;
};

// initialize the indexer object as a static object
template< std::size_t order, std::size_t dim, typename Data_T >
typename WTensorBaseSym< order, dim, Data_T >::PositionIndexer const WTensorBaseSym< order, dim, Data_T >::m_indexer;

// ######################## impl of WTensorBaseSym::PositionIndexer #####################################

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::PositionIndexer::PositionIndexer()
{
    // the map uses lexical ordering of vectors
    std::map< boost::array< std::size_t, order >, std::size_t > m;

    // fill the map with all possible combinations of indices, where
    // every combination of indices appears in ascending order of indices
    boost::array< std::size_t, order > pos;
    pos.assign( 0 );

    for( std::size_t k = 0; k < dataSize; ++k )
    {
        // enumerate the position
        // m[ pos ] = k;
        m.insert( std::make_pair( pos, k ) );

        // get the next sorted combination
        positionIterateSortedOneStep< order, dim >( pos );
    }

    // now iterate all possible sets of indices
    pos.assign( 0 );

    boost::array< std::size_t, order > _p;
    _p.assign( 0 );

    for( std::size_t k = 0; k < WPower< dim, order >::value; ++k )
    {
        _p = pos;

        // sort the indices in _p
        std::sort( _p.begin(), _p.end() );

        // now map the arbitrary ordered indices to the position of the ordered set in m (and thus in m_data)
        m_positions[ &pos[ 0 ] ] = m[ _p ];

        // the map should already knows the sorted position,
        // it should never be added by std::map::operator [] at this point
        WAssert( m.size() == dataSize, "" );

        // get the next position
        positionIterateOneStep< order, dim >( pos );
    }
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
std::size_t WTensorBaseSym< order, dim, Data_T >::PositionIndexer::operator[] ( Index_T pos[] ) const
{
    return m_positions[ pos ];
}

// ######################## impl of WTensorBaseSym #####################################

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::WTensorBaseSym()
{
    m_data.assign( Data_T() );
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::WTensorBaseSym( const WValue< Data_T >& data )
{
    WAssert( dataSize == m_data.size(), "Number of given components does not match the order and dimension of this symmetric tensor" );
    std::copy( &data[ 0 ], &data[ 0 ] + data.size(), &m_data[ 0 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::WTensorBaseSym( WTensorBaseSym const& t )
    : m_data( t.m_data )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T > const& WTensorBaseSym< order, dim, Data_T >::operator = ( WTensorBaseSym const& t )
{
    m_data = t.m_data;
    return *this;
}

template< std::size_t order, std::size_t dim, typename Data_T >
std::size_t WTensorBaseSym< order, dim, Data_T >::getDimension() const
{
    return dim;
}

template< std::size_t order, std::size_t dim, typename Data_T >
std::size_t WTensorBaseSym< order, dim, Data_T >::getOrder() const
{
    return order;
}

template< std::size_t order, std::size_t dim, typename Data_T >
void WTensorBaseSym< order, dim, Data_T >::setValues( WValue< Data_T > const values )
{
    if( m_data.size() == values.size() )
    {
        std::copy( &values[ 0 ], &values[ 0 ] + values.size(), &m_data[ 0 ] );
    }
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBaseSym< order, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices )
{
    return const_cast< Data_T& >( static_cast< WTensorBaseSym const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBaseSym< order, dim, Data_T >::operator[] ( std::vector< Index_T > const& indices ) const
{
    WAssert( indices.size() >= order, "" );
    return operator[] ( &indices[ 0 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBaseSym< order, dim, Data_T >::operator[] ( Index_T indices[] )
{
    return const_cast< Data_T& >( static_cast< WTensorBaseSym const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBaseSym< order, dim, Data_T >::operator[] ( Index_T indices[] ) const
{
    std::size_t p = m_indexer[ indices ];
    WAssert( p < m_data.size(), "" );
    return m_data[ p ];
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::operator == ( WTensorBaseSym const& other ) const
{
    return m_data == other.m_data;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::operator != ( WTensorBaseSym const& other ) const
{
    return m_data != other.m_data;
}

// ######################### WTensorBaseSym for order == 0 ##########################

/**
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.

 * Specialization for order = 0. This essentially encapsulates a scalar. The purpose of
 * this specialization is compatibility for generic tensor functions.
 */
template< std::size_t dim, typename Data_T >
class WTensorBaseSym< 0, dim, Data_T >
{
    friend class ::WTensorBaseSymTest;
    friend class ::WTensorFuncTest;

public:
    // implementations are inline as they are trivial
    /**
     * Standard constructor.
     *
     * All elements are set to Data_T().
     */
    WTensorBaseSym()
        : m_data( Data_T() )
    {
    }

    /**
     * Copy constructor.
     *
     * \param t The tensor to copy from.
     */
    WTensorBaseSym( WTensorBaseSym const& t ) // NOLINT
        : m_data( t.m_data )
    {
    }

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBaseSym const& operator = ( WTensorBaseSym const& t )
    {
        m_data = t.m_data;
        return *this;
    }

    /**
     * Get the dimension of this tensor.
     *
     * \return The dimension of this tensor.
     */
    std::size_t getDimension() const
    {
        return dim;
    }

    /**
     * Get the order of this tensor.
     *
     * \return The order of this tensor.
     */
    std::size_t getOrder() const
    {
        return 0;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( std::vector< Index_T > const& /* indices */ )
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( std::vector< Index_T > const& /* indices */ ) const
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T[] /* indices */ )
    {
        return m_data;
    }

    /**
     * Get the value of this scalar.
     *
     * \return A reference to the element.
     */
    template< typename Index_T >
    Data_T const& operator[] ( Index_T[] /* indices */ ) const
    {
        return m_data;
    }

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBaseSym const& other ) const
    {
        return m_data == other.m_data;
    }

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBaseSym const& other ) const
    {
        return m_data != other.m_data;
    }

protected:
    /**
     * Stores the value.
     */
    Data_T m_data;

private:
    /**
     * Declare a compile-time constant as enum and not as static constant.
     */
    enum
    {
        /**
         * The number of elements to store.
         */
        dataSize = 1
    };
};

// ################################### class WTensorFunc<> ######################################

/**
 * Implements functions that should only be defined for certain values of order.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam order The order of the tensor.
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
//next line is nolint because brainlint cannot find the declaration of TensorBase_T
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t order, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc : public TensorBase_T< order, dim, Data_T >
{
};

/**
 * Implements the operator () for an order of 6.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 6, dim, Data_T > : public TensorBase_T< 6, dim, Data_T >
{
public:
    /**
     * Default constructor.
     */
    WTensorFunc();

    /**
     * Initializes the tensor with the given data.
     *
     * \param data Components in same ordering as the components of the TensorBase class.
     */
    explicit WTensorFunc( const WValue< Data_T >& data );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     * \param i4 An index.
     * \param i5 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3, std::size_t i4, std::size_t i5 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     * \param i4 An index.
     * \param i5 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3, std::size_t i4, std::size_t i5 ) const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T >
WTensorFunc< TensorBase_T, 6, dim, Data_T >::WTensorFunc()
    : TensorBase_T< 6, dim, Data_T >()
{
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T >
WTensorFunc< TensorBase_T, 6, dim, Data_T >::WTensorFunc( const WValue< Data_T >& data )
    : TensorBase_T< 6, dim, Data_T >( data )
{
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 6, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2,
                                                                  std::size_t i3, std::size_t i4, std::size_t i5 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1, i2, i3, i4, i5 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 6, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2,
                                                                        std::size_t i3, std::size_t i4, std::size_t i5 ) const
{
    std::size_t p[] = { i0, i1, i2, i3, i4, i5 };
    return TensorBase_T< 6, dim, Data_T >::operator[] ( p );
}




/**
 * Implements the operator () for an order of 5.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 5, dim, Data_T > : public TensorBase_T< 5, dim, Data_T >
{
public:
    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     * \param i4 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3, std::size_t i4 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     * \param i4 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3, std::size_t i4 ) const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 5, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2,
                                                                  std::size_t i3, std::size_t i4 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1, i2, i3, i4 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 5, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2,
                                                                        std::size_t i3, std::size_t i4 ) const
{
    std::size_t p[] = { i0, i1, i2, i3, i4 };
    return TensorBase_T< 5, dim, Data_T >::operator[] ( p );
}




/**
 * Implements the operator () for an order of 4.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 4, dim, Data_T > : public TensorBase_T< 4, dim, Data_T >
{
public:
    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     * \param i3 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3 ) const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 4, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1, i2, i3 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 4, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2, std::size_t i3 ) const
{
    std::size_t p[] = { i0, i1, i2, i3 };
    return TensorBase_T< 4, dim, Data_T >::operator[] ( p );
}



/**
 * Implements the operator () for an order of 3.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 3, dim, Data_T > : public TensorBase_T< 3, dim, Data_T >
{
public:
    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0, std::size_t i1, std::size_t i2 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     * \param i2 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0, std::size_t i1, std::size_t i2 ) const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 3, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1, i2 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 3, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1, std::size_t i2 ) const
{
    std::size_t p[] = { i0, i1, i2 };
    return TensorBase_T< 3, dim, Data_T >::operator[] ( p );
}

/**
 * Implements the operator () for an order of 2 as well as casts to WMatrix.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 2, dim, Data_T > : public TensorBase_T< 2, dim, Data_T >
{
public:
    /**
     * Default constructor.
     */
    WTensorFunc();

    /**
     * Initializes the tensor with the given data.
     *
     * \param data Components in same ordering as the components of the TensorBase class.
     */
    explicit WTensorFunc( const WValue< Data_T >& data );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0, std::size_t i1 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     * \param i1 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0, std::size_t i1 ) const;

    /**
     * Cast this 2nd-order tensor into a WMatrix.
     */
    operator WMatrix< Data_T >() const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T >
WTensorFunc< TensorBase_T, 2, dim, Data_T >::WTensorFunc()
    : TensorBase_T< 2, dim, Data_T >()
{
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T >
WTensorFunc< TensorBase_T, 2, dim, Data_T >::WTensorFunc( const WValue< Data_T >& data )
    : TensorBase_T< 2, dim, Data_T >( data )
{
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1 ) const
{
    std::size_t p[] = { i0, i1 };
    return TensorBase_T< 2, dim, Data_T >::operator[] ( p );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator WMatrix< Data_T > () const
{
    WMatrix< Data_T > m( dim, dim );
    for( std::size_t i = 0; i < dim; ++i )
    {
        for( std::size_t j = 0; j < dim; ++j )
        {
            m( i, j ) = this->operator() ( i, j );
        }
    }
    return m;
}

/**
 * Implements the operator () for an order of 1 as well as a cast to WValue.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 1, dim, Data_T > : public TensorBase_T< 1, dim, Data_T >
{
public:
    /**
     * Access operator.
     *
     * \param i0 An index.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ( std::size_t i0 );

    /**
     * Access operator.
     *
     * \param i0 An index.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() ( std::size_t i0 ) const;

    /**
     * Cast this tensor to a WValue.
     */
    operator WValue< Data_T > () const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 1, dim, Data_T >::operator() ( std::size_t i0 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 1, dim, Data_T >::operator() ( std::size_t i0 ) const
{
    std::size_t p[] = { i0 };
    return TensorBase_T< 1, dim, Data_T >::operator[] ( p );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
WTensorFunc< TensorBase_T, 1, dim, Data_T >::operator WValue< Data_T > () const
{
    WValue< Data_T > v( dim );

    for( std::size_t k = 0; k < dim; ++k )
    {
        v[ k ] = this->operator() ( k );
    }

    return v;
}

/**
 * Implements the operator () for an order of 0 and a cast to Data_T.
 *
 * \tparam TensorBase_T Either WTensorBase<> or WTensorBaseSym<>
 * \tparam dim The dimension of the tensor, i.e. the number of components
 * in each direction.
 * \tparam Data_T The datatype of the components, double by default.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 0, dim, Data_T > : public TensorBase_T< 0, dim, Data_T >
{
public:
    /**
     * Access operator.
     *
     * \return A reference to the element.
     */
    Data_T& operator() ();

    /**
     * Access operator.
     *
     * \return A reference to the element.
     */
    Data_T const& operator() () const;

    /**
     * Cast this tensor to a Data_T.
     */
    operator Data_T() const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 0, dim, Data_T >::operator() ()
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() () );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 0, dim, Data_T >::operator() () const
{
#ifdef _MSC_VER
    return TensorBase_T< 0, dim, Data_T >::operator[]< std::size_t >( NULL );
#else
    return TensorBase_T< 0, dim, Data_T >::template operator[]< std::size_t >( NULL );
#endif
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
WTensorFunc< TensorBase_T, 0, dim, Data_T >::operator Data_T() const
{
#ifdef _MSC_VER
    return TensorBase_T< 0, dim, Data_T >::operator[]< std::size_t >( NULL );
#else
    return TensorBase_T< 0, dim, Data_T >::template operator[]< std::size_t >( NULL );
#endif
}

#endif  // WTENSORBASE_H
