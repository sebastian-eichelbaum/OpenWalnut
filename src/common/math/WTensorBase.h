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

#include <vector>
#include <map>
#include <algorithm>

#include "WCompileTimeFunctions.h"
#include "../WAssert.h"

// TODO(reichenbach): Remove vectors (because of the enum dataSize).
// TODO(reichenbach): Write TensorBase< 0,...> and remove WTensor<0,...> specialization.
// TODO(reichenbach): Cast WTensorBaseSym to WTensorBase
// TODO(reichenbach): Check type safety.
// TODO(reichenbach): Add casts to WVector3D, WValue and WMatrix.
// TODO(reichenbach): Add lots of operators.

namespace wmath
{
// forward declaration
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBaseSym;

// ############################# class WTensorBase<> #################################

/**
 * Normal tensor base class.
 *
 * Template parameters are the order, the dimension of the tensor and
 * the datatype of the elements.
 *
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during any assignment operator.
 *
 * \see WTensor
 */
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBase
{
    // Make the appropriate tensor of order + 1 a friend, so it has access to the getPos() member.
    friend class WTensorBase< order + 1, dim, Data_T >;

public:
    friend class WTensor_test;

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
     * \note No bounds checking is performed.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T const* const indices );

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
    Data_T const& operator[] ( Index_T const* const indices ) const;

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBase const& other );

    /**
     * Compare this WTensorBase to another one.
     *
     * \param other The WBensorBase to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBase const& other );

protected:
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
    explicit WTensorBase( WTensorBase const& t );

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBase const& operator = ( WTensorBase const& t );

    /**
     * Calculate the position of the element in the data vector. The function
     * is build recursively at compile-time.
     *
     * \param pos An array of indices.
     *
     * \return The position of the element.
     */
    template< typename Index_T >
    static inline std::size_t getPos( Index_T const* const pos );

    /**
     * Get the reference to an element in the data vector.
     *
     * \param pos An array of indices.
     *
     * \return The position of the element.
     */
    template< typename Index_T >
    inline Data_T const& getAt( Index_T const* const pos ) const;

private:

    /**
     * Stores all elements.
     */
    std::vector< Data_T > m_data;

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
};

/**
 * Specialization for order = 0. This is only used for recursive
 * definitions of the dataSize member and the getPos() function. There cannot
 * be any instances of this class.
 */
template< std::size_t dim, typename Data_T >
class WTensorBase< 0, dim, Data_T >
{
protected:
    friend class WTensorBase< 1, dim, Data_T >;

    /**
     * Calculate the position of the element in the data vector. This
     * is essentially the standard case of the recursion.
     *
     * \return 0.
     */
    template< typename PIndex_T >
    static inline std::size_t getPos( PIndex_T /* pos */ )
    {
        return 0;
    }

private:

    /**
     * Private constructor.
     */
    WTensorBase();

    /**
     * Private copy contructor.
     */
    WTensorBase( WTensorBase& );

    /**
     * Private copy operator.
     */
    WTensorBase& operator = ( WTensorBase& );

    /**
     * Private destructor.
     */
    ~WTensorBase();
};

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T >::WTensorBase()
    : m_data( dataSize, Data_T() )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T >::WTensorBase( WTensorBase const& t )
    : m_data( t.m_data )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBase< order, dim, Data_T > const& WTensorBase< order, dim, Data_T >::operator = ( WTensorBase const& t )
{
    m_data = t.m_data;
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
std::size_t WTensorBase< order, dim, Data_T >::getPos( Index_T const* const pos )
{
    return WTensorBase< order - 1, dim, Data_T >::getPos( pos ) * dim + pos[ order - 1 ];
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBase< order, dim, Data_T >::getAt( Index_T const* const pos ) const
{
    for( std::size_t k = 0; k < order; ++k )
    {
        WAssert( static_cast< std::size_t >( pos[ k ] ) < dim, "" );
    }
    std::size_t p = getPos( pos );
    return m_data[ p ];
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
    return getAt( &indices[ 0 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBase< order, dim, Data_T >::operator[] ( Index_T const* const indices )
{
    return const_cast< Data_T& >( static_cast< WTensorBase const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBase< order, dim, Data_T >::operator[] ( Index_T const* const indices ) const
{
    return getAt( indices );
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBase< order, dim, Data_T >::operator == ( WTensorBase const& other )
{
    return m_data == other.m_data;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBase< order, dim, Data_T >::operator != ( WTensorBase const& other )
{
    return m_data != other.m_data;
}

// ################################# class WTensorBaseSym<> #####################################

/**
 * Symmetric tensor base class.
 *
 * Template parameters are the order, the dimension of the tensor and
 * the datatype of the elements.
 *
 * \note The type Data_T may not throw exceptions on construction, destruction or
 * during any assignment operator.
 *
 * \see WTensorSym
 */
template< std::size_t order, std::size_t dim, typename Data_T >
class WTensorBaseSym
{
public:
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
     * \note No bounds checking is performed.
     */
    template< typename Index_T >
    Data_T& operator[] ( Index_T const* const indices );

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
    Data_T const& operator[] ( Index_T const* const indices ) const;

    /**
     * Compare this WTensorBaseSym to another one.
     *
     * \param other The WBensorBaseSym to compare to.
     *
     * \return True, iff this tensors' elements are equal to another tensors' elements.
     */
    bool operator == ( WTensorBaseSym const& other );

    /**
     * Compare this WTensorBaseSym to another one.
     *
     * \param other The WBensorBaseSym to compare to.
     *
     * \return True, iff this tensors' elements are not equal to another tensors' elements.
     */
    bool operator != ( WTensorBaseSym const& other );

protected:

    /**
     * Standard constructor.
     *
     * All elements are set to Data_T().
     */
    WTensorBaseSym();

    /**
     * Copy constructor.
     *
     * \param t The tensor to copy from.
     */
    explicit WTensorBaseSym( WTensorBaseSym const& t );

    /**
     * Copy operator.
     *
     * \param t The tensor to copy from.
     *
     * \return *this.
     */
    WTensorBaseSym const& operator = ( WTensorBaseSym const& t );

    /**
     * Get the reference to an element in the data vector.
     *
     * \param pos An array of indices.
     *
     * \return The position of the element.
     */
    template< typename Index_T >
    inline Data_T const& getAt( Index_T const* const pos ) const;

    /**
     * Cast this WTensorBaseSym to WTensorBase, so one can cast
     * a WTensorSym to WTensor.
     *
     * \return A WTensor.
     */
    //operator WTensorBase< order, dim, Data_T > () const;

private:
    /**
     * Stores the elements of this tensor.
     */
    std::vector< Data_T > m_data;

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
     * A class that maps symmetric tensor indices to vector positions.
     */
    class PositionIndexer : public WTensorBase< order, dim, std::size_t >
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
        inline std::size_t getAt( Index_T const* const pos ) const;

        using WTensorBase< order, dim, std::size_t >::getPos;

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
         * A utility class that encapsulates a position (a set of indices of a tensor).
         */
        class Position : public std::vector< std::size_t >
        {
        public:
            /**
             * Initialize as first position (0,0,0,...0).
             */
            Position();

            /**
             * Copy operator.
             * 
             * \param p The position to copy.
             */
            Position( Position const& p ); // NOLINT not explicit because of std::map

            /**
             * Increment position.
             * 
             * \return *this.
             */
            Position& operator++ ();

            /**
             * Sorted increment position.
             * 
             * \return *this.
             */
            Position& incrSym();

            /**
             * Get sorted position.
             *
             * \return A vector of sorted indices. 
             */
            Position operator() ();

            /**
             * Determine if two Positions are equal.
             *
             * \param other The position to compare to.
             *
             * \return True, iff the two positions are equal.
             */
            bool operator == ( Position const& other );

            /**
             * Strict weak ordering.
             *
             * \param other The position to compare to.
             *
             * \return True, iff this position is smaller than the other.
             */
            bool operator< ( Position const& other );

            /**
             * Set all indices to 0.
             */
            void clear();
        };
    };

    /**
     * A static tensor that maps tensor indices to vector positions.
     */
    static PositionIndexer const m_indexer;
};


template< std::size_t order, std::size_t dim, typename Data_T >
typename WTensorBaseSym< order, dim, Data_T >::PositionIndexer const WTensorBaseSym< order, dim, Data_T >::m_indexer;

// ######################## impl of WTensorBaseSym::PositionIndexer::Position #####################################

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::Position()
    : std::vector< std::size_t >( order, 0 )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::Position( Position const& p )
    : std::vector< std::size_t >( p )
{
}

template< std::size_t order, std::size_t dim, typename Data_T >
typename WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position& WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::incrSym()
{
    for( int k = order - 1; k > -1; --k )
    {
        if( at( k ) != dim - 1 )
        {
            ++( at( k ) );
            for( std::size_t i = k + 1; i < order; ++i )
            {
                at( i ) = at( k );
            }
            return *this;
        }
    }
    return *this;
}

template< std::size_t order, std::size_t dim, typename Data_T >
typename WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position&
                        WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::operator++ ()
{
    for( std::size_t k = order - 1; k > 0; --k )
    {
        if( at( k ) == dim - 1)
        {
            at( k ) = 0;
        }
        else
        {
            ++( at( k ) );
            return *this;
        }
    }
    ++( at( 0 ) );
    return *this;
}

template< std::size_t order, std::size_t dim, typename Data_T >
typename WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position
                        WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::operator() ()
{
    Position res( *this );
    std::sort( res.begin(), res.end() );
    return res;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::operator == ( Position const& other )
{
    for( std::size_t k = 0; k < order; ++k )
    {
        if( at( k ) != other[ k ] )
        {
            return false;
        }
    }
    return true;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::operator < ( Position const& other )
{
    for( std::size_t k = 0; k < order; ++k )
    {
        if( at( k ) > other[ k ] )
        {
            return false;
        }
    }
    return true;
}

template< std::size_t order, std::size_t dim, typename Data_T >
void WTensorBaseSym< order, dim, Data_T >::PositionIndexer::Position::clear()
{
    for( std::size_t k = 0; k < order; ++k )
    {
        at( k ) = 0;
    }
}

// ######################## impl of WTensorBaseSym::PositionIndexer #####################################

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::PositionIndexer::PositionIndexer()
{
    std::size_t n = 0;
    std::map< Position, std::size_t > m;

    Position p;
    for( std::size_t k = 0; k < dataSize; ++k )
    {
        m[ p ] = n;

        p.incrSym();
        ++n;
    }

    p.clear();
    for( std::size_t k = 0; k < WPower< dim, order >::value; ++k )
    {
        Position _p( p() );

        ( WTensorBase< order, dim, std::size_t >::operator[] ( p ) ) = m[ _p ];
        WAssert( m.size() == dataSize, "" );

        ++p;
    }
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
std::size_t WTensorBaseSym< order, dim, Data_T >::PositionIndexer::getAt( Index_T const* const pos ) const
{
    return this->operator[] ( pos );
}

// ######################## impl of WTensorBaseSym #####################################

template< std::size_t order, std::size_t dim, typename Data_T >
WTensorBaseSym< order, dim, Data_T >::WTensorBaseSym()
    : m_data( dataSize, Data_T() )
{
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
template< typename Index_T >
Data_T const& WTensorBaseSym< order, dim, Data_T >::getAt( Index_T const* const pos ) const
{
    std::size_t p = m_indexer.getAt( pos );
    WAssert( p < m_data.size(), "" );
    return m_data[ p ];
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
    return getAt( &indices[ 0 ] );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T& WTensorBaseSym< order, dim, Data_T >::operator[] ( Index_T const* const indices )
{
    return const_cast< Data_T& >( static_cast< WTensorBaseSym const& >( *this ).operator[] ( indices ) );
}

template< std::size_t order, std::size_t dim, typename Data_T >
template< typename Index_T >
Data_T const& WTensorBaseSym< order, dim, Data_T >::operator[] ( Index_T const* const indices ) const
{
    return getAt( indices );
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::operator == ( WTensorBaseSym const& other )
{
    return m_data == other.m_data;
}

template< std::size_t order, std::size_t dim, typename Data_T >
bool WTensorBaseSym< order, dim, Data_T >::operator != ( WTensorBaseSym const& other )
{
    return m_data != other.m_data;
}

//template< std::size_t order, std::size_t dim, typename Data_T >
//WTensorBaseSym< order, dim, Data_T >::operator WTensorBase< order, dim, Data_T > () const
//{
//    WTensorBase< order, dim, Data_T > t;
//
//    std::size_t pos[ order ]; // NOLINT because we want a constant size array for efficiency
//    for( std::size_t k = 0; k < order; ++k )
//    {
//        pos[ k ] = 0;
//    }
//
//    for( std::size_t k = 0; k < t.m_data.size(); ++k )
//    {
//        const_cast< Data_T& >( t.getAt( pos ) ) = getAt( pos );
//        ++pos[ 0 ];
//        for( std::size_t i = 0; i < order - 1; ++i )
//        {
//            if( pos[ i ] < dim )
//            {
//                break;
//            }
//            else
//            {
//                pos[ i ] = 0;
//                ++pos[ i + 1 ];
//            }
//        }
//    }
//}

// ################################### class WTensorFunc<> ######################################

/**
 * Implements the correct operator () depending on the order of the Tensor (up to 6)
 * or no operator () for orders larger than 6.
 */
//next line is nolint because brainlint cannot find the declaration of TensorBase_T
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t order, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc : public TensorBase_T< order, dim, Data_T >
{
};




/**
 * Implements the operator () for an order of 6.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 6, dim, Data_T > : public TensorBase_T< 6, dim, Data_T >
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
    return TensorBase_T< 6, dim, Data_T >::getAt( p );
}




/**
 * Implements the operator () for an order of 5.
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
    return TensorBase_T< 5, dim, Data_T >::getAt( p );
}




/**
 * Implements the operator () for an order of 4.
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
    return TensorBase_T< 4, dim, Data_T >::getAt( p );
}



/**
 * Implements the operator () for an order of 3.
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
    return TensorBase_T< 3, dim, Data_T >::getAt( p );
}



/**
 * Implements the operator () for an order of 2 as well as matrix multiplication.
 */
template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
class WTensorFunc< TensorBase_T, 2, dim, Data_T > : public TensorBase_T< 2, dim, Data_T >
{
public:
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
     * Multiply with another matrix.
     * 
     * \param other The matrix to multiply with.
     * 
     * \return The product.
     */
    WTensorFunc operator * ( WTensorFunc const& other ) const;

    /**
     * Multiply with a vector.
     * 
     * \param other The vector to multiply with.
     * 
     * \return The product.
     */
    WTensorFunc< TensorBase_T, 1, dim, Data_T > operator * ( WTensorFunc< TensorBase_T, 1, dim, Data_T > const& other ) const;
};

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T& WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1 )
{
    return const_cast< Data_T& >( static_cast< WTensorFunc const& >( *this ).operator() ( i0, i1 ) );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
Data_T const& WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator() ( std::size_t i0, std::size_t i1 ) const
{
    std::size_t p[] = { i0, i1 };
    return TensorBase_T< 2, dim, Data_T >::getAt( p );
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
WTensorFunc< TensorBase_T, 2, dim, Data_T > WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator * ( WTensorFunc const& other ) const
{
    WTensorFunc res;
    for( std::size_t i = 0; i < dim; ++i )
    {
        for( std::size_t j = 0; j < dim; ++j )
        {
            for( std::size_t k = 0; k < dim; ++k )
            {
                res( i, j ) += this->operator() ( i, k ) * other( k, j );
            }
        }
    }
    return res;
}

template< template< std::size_t, std::size_t, typename > class TensorBase_T, std::size_t dim, typename Data_T > //NOLINT
WTensorFunc< TensorBase_T, 1, dim, Data_T > WTensorFunc< TensorBase_T, 2, dim, Data_T >::operator *
                                                          ( WTensorFunc< TensorBase_T, 1, dim, Data_T > const& other ) const
{
    WTensorFunc< TensorBase_T, 1, dim, Data_T > res;
    for( std::size_t i = 0; i < dim; ++i )
    {
        for( std::size_t j = 0; j < dim; ++j )
        {
            res( i ) += this->operator() ( i, j ) * other( j );
        }
    }
    return res;
}

/**
 * Implements the operator () for an order of 1.
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
    return TensorBase_T< 1, dim, Data_T >::getAt( p );
}

} // namespace wmath

#endif  // WTENSORBASE_H
