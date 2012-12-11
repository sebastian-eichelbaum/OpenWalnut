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

#ifndef WINTERVAL_H
#define WINTERVAL_H

#include <utility>
#include <algorithm>

#include <boost/shared_ptr.hpp>

#include "../WTypeTraits.h"

/**
 * Basic class for encapsulating a std::pair to be interpreted as interval. This class intentionally does not include a parameter telling whether
 * the interval is open or not (mathematically: [],][,[[,]])
 *
 * \tparam T the type used for this interval
 */
template< typename T >
class WInterval
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WInterval >.
     */
    typedef boost::shared_ptr< WInterval< T > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WInterval >.
     */
    typedef boost::shared_ptr< const WInterval< T > > ConstSPtr;

    /**
     * Type used to store the information
     */
    typedef std::pair< T, T > StoreType;

    /**
     * My own type.
     */
    typedef WInterval< T > Type;

    /**
     * Copy constructor to create a WInterval using a std::pair
     *
     * \param c the pair to use
     */
    explicit WInterval( const StoreType& c );

    /**
     * Copy constructor.
     *
     * \param c the interval to copy
     */
    WInterval( const Type& c );  // NOLINT

    /**
     * Create a new interval instance using the given values.
     *
     * \param l the lower border
     * \param u the upper border
     */
    WInterval( const T& l, const T& u );

    /**
     * Destructor.
     */
    virtual ~WInterval();

    /**
     * Convert the WInterval instance to a std::pair again.
     *
     * \return the pair
     */
    operator const StoreType& () const;

    /**
     * Get the lower value of the interval.
     *
     * \return the lower value
     */
    const T& getLower() const;

    /**
     * Return the upper value of the interval
     *
     * \return the upper value
     */
    const T& getUpper() const;

    /**
     * Compare this interval with another one
     *
     * \param interval the other one
     *
     * \return true if lower and upper bounds are equal
     */
    bool operator==( Type interval ) const;

protected:
private:
    /**
     * The interval itself.
     */
    StoreType m_interval;
};

/**
 * Abbreviation for an double interval.
 */
typedef WInterval< double > WIntervalDouble;

/**
 * Abbreviation for an integer interval
 */
typedef WInterval< int > WIntervalInt;

/**
 * Create an interval instance similar to make_pair.
 *
 * \tparam T1 the lower bound type
 * \tparam T2 the upper bound type
 * \param l lower bound
 * \param u upper bound
 *
 * \return the interval
 */
template < typename T1, typename T2 >
WInterval< typename WTypeTraits::TypePromotion< T1, T2 >::Result > make_interval( T1 l, T2 u )
{
    return WInterval< typename WTypeTraits::TypePromotion< T1, T2 >::Result >( l, u );
}

/**
 * Check whether a value is in the interval or not. This function interprets the interval as closed at both bounds.
 *
 * \tparam IntervalType type if the interval
 * \tparam T type of the value to use for checking
 * \param interval the interval to check against
 * \param value the value
 *
 * \return true if inside
 */
template < typename IntervalType, typename T >
bool isInClosed( const IntervalType& interval, const T& value )
{
    return ( ( interval.getLower() <= value ) && ( interval.getUpper() >= value ) );
}

/**
 * Check whether a value is in the interval or not. This function interprets the interval as open at both bounds.
 *
 * \tparam IntervalType type if the interval
 * \tparam T type of the value to use for checking
 * \param interval the interval to check against
 * \param value the value
 *
 * \return true if inside
 */
template < typename IntervalType, typename T >
bool isInOpen( const IntervalType& interval, const T& value )
{
    return ( ( interval.getLower() < value ) && ( interval.getUpper() > value ) );
}

/**
 * Check whether a value is in the interval or not. This function interprets the interval as open at the lower bound and closed at the upper one.
 *
 * \tparam IntervalType type if the interval
 * \tparam T type of the value to use for checking
 * \param interval the interval to check against
 * \param value the value
 *
 * \return true if inside
 */
template < typename IntervalType, typename T >
bool isInOpenClosed( const IntervalType& interval, const T& value )
{
    return ( ( interval.getLower() < value ) && ( interval.getUpper() >= value ) );
}

/**
 * Check whether a value is in the interval or not. This function interprets the interval as closed at the lower bound and open at the upper one.
 *
 * \tparam IntervalType type if the interval
 * \tparam T type of the value to use for checking
 * \param interval the interval to check against
 * \param value the value
 *
 * \return true if inside
 */
template < typename IntervalType, typename T >
bool isInClosedOpen( const IntervalType& interval, const T& value )
{
    return ( ( interval.getLower() <= value ) && ( interval.getUpper() > value ) );
}

template < typename T >
WInterval< T >::WInterval( const StoreType& c )
{
    // ensure order
    m_interval.first = std::min( c.first, c.second );
    m_interval.second = std::min( c.first, c.second );
}

template < typename T >
WInterval< T >::WInterval( const Type& c ):
    m_interval( c.m_interval )
{
    // nothing else to do
}

template < typename T >
WInterval< T >::WInterval( const T& l, const T& u ):
    m_interval( std::min( l, u ), std::max( l, u ) )
{
    // nothing else to do
}

template < typename T >
WInterval< T >::~WInterval()
{
    // nothing else to do
}

template < typename T >
WInterval< T >::operator const StoreType& () const
{
    return m_interval;
}

template < typename T >
const T& WInterval< T >::getLower() const
{
    return m_interval.first;
}

template < typename T >
const T& WInterval< T >::getUpper() const
{
    return m_interval.second;
}

template < typename T >
bool WInterval< T >::operator==( Type interval ) const
{
    return ( ( interval.getLower() == getLower() ) && ( interval.getUpper() == getUpper() ) );
}

#endif  // WINTERVAL_H

