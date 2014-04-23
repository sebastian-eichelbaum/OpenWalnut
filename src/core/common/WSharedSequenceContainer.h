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

#ifndef WSHAREDSEQUENCECONTAINER_H
#define WSHAREDSEQUENCECONTAINER_H

#include <algorithm>

#include <boost/thread.hpp>

#include "WSharedObject.h"

/**
 * This class provides a common interface for thread-safe access to sequence containers (list, vector, dequeue ).
 * \param S the sequence container to use. Everything is allowed here which provides push_back and pop_back as well as size functionality.
 */
template < typename S >
class WSharedSequenceContainer: public WSharedObject< S >
{
public:
    // Some helpful typedefs

    /**
     * A typedef for the correct const iterator useful to traverse this sequence container.
     */
    typedef typename S::const_iterator   ConstIterator;

    /**
     * A typedef for the correct iterator to traverse this sequence container.
     */
    typedef typename S::iterator         Iterator;

    /**
     * The type of the elements
     */
    typedef typename S::value_type value_type;

    /**
     * Default constructor.
     */
    WSharedSequenceContainer();

    /**
     * Destructor.
     */
    virtual ~WSharedSequenceContainer();

    //////////////////////////////////////////////////////////////////////////////////////////
    // These methods implement common methods of all sequence containers. The list is not
    // complete but should be enough for now.
    // \NOTE: all methods using or returning iterators are NOT implemented here. Use the access
    // Object (getAccessObject) to iterate.
    //////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Adds a new element at the end of the container.
     *
     * \param x the new element.
     */
    void push_back( const typename S::value_type& x );

    /**
     * Adds a new element at the beginning of the container.
     *
     * \param x the new element.
     */
    void push_front( const typename S::value_type& x );

    /**
     * Removes an element from the end.
     */
    void pop_back();

    /**
     * Add the element only if it is not inside the container until now. This is a shortcut to a combined used of find and push_back.
     *
     * \param x the element to push back.
     */
    void unique_push_back( const typename S::value_type& x );

    /**
     * Add the element only if it is not inside the container until now. This is a shortcut to a combined used of find and push_front.
     *
     * \param x the element to push front.
     */
    void unique_push_front( const typename S::value_type& x );

    /**
     * Clears the container.
     */
    void clear();

    /**
     * The size of the container.
     *
     * \return the size.
     *
     * \note: be aware that the size can change at every moment after getting the size, since the read lock got freed. Better use
     * access objects to lock the container and use size() on the container directly.
     */
    size_t size() const;

    /**
     * Get item at position n. Uses the [] operator of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    typename S::value_type& operator[]( size_t n );

    /**
     * Get item at position n. Uses the [] operator of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    const typename S::value_type& operator[]( size_t n ) const;

    /**
     * Get item at position n. Uses the at-method of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    typename S::value_type& at( size_t n );

    /**
     * Get item at position n. Uses the at-method of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    const typename S::value_type& at( size_t n ) const;

    /**
     * Searches and removes the specified element. If it is not found, nothing happens. It mainly is a comfortable forwarder for std::remove and
     * S::erase.
     *
     * \param element the element to remove
     */
    void remove( const typename S::value_type& element );

    /**
     * Erase the element at the specified position. Read your STL reference for more details.
     *
     * \param position where to erase
     *
     * \return A random access iterator pointing to the new location of the element that followed the last element erased by the function call.
     */
    typename WSharedSequenceContainer< S >::Iterator erase( typename WSharedSequenceContainer< S >::Iterator position );

    /**
     * Erase the specified range of elements. Read your STL reference for more details.
     *
     * \param first Iterators specifying a range within the vector to be removed: [first,last).
     * \param last Iterators specifying a range within the vector to be removed: [first,last).
     *
     * \return A random access iterator pointing to the new location of the element that followed the last element erased by the function call.
     */
    typename WSharedSequenceContainer< S >::Iterator erase( typename WSharedSequenceContainer< S >::Iterator first,
                                                            typename WSharedSequenceContainer< S >::Iterator last );

    /**
     * Replaces the specified old value by a new one. If the old one does not exist, nothing happens. This is a comfortable forwarder for
     * std::replace.
     *
     * \param oldValue the old value to replace
     * \param newValue the new value
     */
    void replace( const typename S::value_type& oldValue, const typename S::value_type& newValue );

    /**
     * Counts the number of occurrences of the specified value inside the container. This is a comfortable forwarder for std::count.
     *
     * \param value the value to count
     *
     * \return the number of items found.
     */
    size_t count( const value_type& value );

    /**
     * Resorts the container using the specified comparator from its begin to its end.
     *
     * \tparam Comparator the comparator type. Usually a boost::function or class providing the operator().
     *
     * \param comp the comparator
     */
    template < typename Comparator >
    void sort( Comparator comp );

    /**
     * Resorts the container using the specified comparator between [first,last) in ascending order.
     *
     * \param first the first element
     * \param last the last element
     * \param comp the comparator
     */
    template < typename Comparator >
    void sort( typename WSharedSequenceContainer< S >::Iterator first, typename WSharedSequenceContainer< S >::Iterator last, Comparator comp );

    /**
     * Resorts the container using the specified comparator from its begin to its end. Uses stable sort algorithm.
     *
     * \tparam Comparator the comparator type. Usually a boost::function or class providing the operator().
     *
     * \param comp the comparator
     */
    template < typename Comparator >
    void stableSort( Comparator comp );

    /**
     * Resorts the container using the specified comparator between [first,last) in ascending order. Uses stable sort algorithm.
     *
     * \param first the first element
     * \param last the last element
     * \param comp the comparator
     */
    template < typename Comparator >
    void stableSort( typename WSharedSequenceContainer< S >::Iterator first, typename WSharedSequenceContainer< S >::Iterator last, Comparator comp );

    /**
     * Searches the specified value in the range [first,last).
     *
     * \param first the first element
     * \param last the last element
     * \param value the value to search.
     *
     * \return the iterator pointing to the found element.
     */
    typename WSharedSequenceContainer< S >::Iterator find( typename WSharedSequenceContainer< S >::Iterator first,
                                                           typename WSharedSequenceContainer< S >::Iterator last,
                                                           const typename S::value_type& value );

    /**
     * Searches the specified value in the range [begin,end).
     *
     * \param value the value to search.
     *
     * \return the iterator pointing to the found element.
     */
    typename WSharedSequenceContainer< S >::ConstIterator find( const typename S::value_type& value );

protected:
private:
};

template < typename S >
WSharedSequenceContainer< S >::WSharedSequenceContainer():
    WSharedObject< S >()
{
    // init members
}

template < typename S >
WSharedSequenceContainer< S >::~WSharedSequenceContainer()
{
    // clean up
}

template < typename S >
void WSharedSequenceContainer< S >::push_back( const typename S::value_type& x )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().push_back( x );
}

template < typename S >
void WSharedSequenceContainer< S >::push_front( const typename S::value_type& x )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().insert( a->get().begin(), x );
}

template < typename S >
void WSharedSequenceContainer< S >::unique_push_back( const typename S::value_type& x )
{
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    WSharedSequenceContainer< S >::Iterator it = std::find( a->get().begin(), a->get().end(), x );
    if( it == a->get().end() )
    {
        // not found -> add
        a->get().push_back( x );
    }
}

template < typename S >
void WSharedSequenceContainer< S >::unique_push_front( const typename S::value_type& x )
{
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    WSharedSequenceContainer< S >::Iterator it = std::find( a->get().begin(), a->get().end(), x );
    if( it == a->get().end() )
    {
        // not found -> add
        a->get().insert( a->get().begin(), x );
    }
}

template < typename S >
void WSharedSequenceContainer< S >::pop_back()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().pop_back();
}

template < typename S >
void WSharedSequenceContainer< S >::clear()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().clear();
}

template < typename S >
size_t WSharedSequenceContainer< S >::size() const
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    size_t size = a->get().size();
    return size;
}

template < typename S >
typename S::value_type& WSharedSequenceContainer< S >::operator[]( size_t n )
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return const_cast< S& >( a->get() ).operator[]( n );    // read tickets return the handled object const. This is bad here although in most cases
    // it is useful and needed.
}

template < typename S >
const typename S::value_type& WSharedSequenceContainer< S >::operator[]( size_t n ) const
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return a->get().operator[]( n );
}

template < typename S >
typename S::value_type& WSharedSequenceContainer< S >::at( size_t n )
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return const_cast< S& >( a->get() ).at( n );    // read tickets return the handled object const. This is bad here although in most cases it
    // is useful and needed.
}

template < typename S >
const typename S::value_type& WSharedSequenceContainer< S >::at( size_t n ) const
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return a->get().at( n );
}

template < typename S >
void WSharedSequenceContainer< S >::remove( const typename S::value_type& element )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().erase( std::remove( a->get().begin(), a->get().end(), element ), a->get().end() );
}

template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::erase( typename WSharedSequenceContainer< S >::Iterator position )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return a->get().erase( position );
}

template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::erase(
        typename WSharedSequenceContainer< S >::Iterator first,
        typename WSharedSequenceContainer< S >::Iterator last )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return a->get().erase( first, last );
}

template < typename S >
void WSharedSequenceContainer< S >::replace( const typename S::value_type& oldValue, const typename S::value_type& newValue )
{
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    std::replace( a->get().begin(), a->get().end(), oldValue, newValue );
}

template < typename S >
size_t WSharedSequenceContainer< S >::count( const value_type& value )
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return std::count( a->get().begin(), a->get().end(), value );
}

template < typename S >
template < typename Comparator >
void WSharedSequenceContainer< S >::sort( Comparator comp )
{
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return std::sort( a->get().begin(), a->get().end(), comp );
}

template < typename S >
template < typename Comparator >
void WSharedSequenceContainer< S >::sort( typename WSharedSequenceContainer< S >::Iterator first,
                                          typename WSharedSequenceContainer< S >::Iterator last,
                                          Comparator comp )
{
    return std::sort( first, last, comp );
}

template < typename S >
template < typename Comparator >
void WSharedSequenceContainer< S >::stableSort( Comparator comp )
{
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return std::stable_sort( a->get().begin(), a->get().end(), comp );
}

template < typename S >
template < typename Comparator >
void WSharedSequenceContainer< S >::stableSort( typename WSharedSequenceContainer< S >::Iterator first,
                                                typename WSharedSequenceContainer< S >::Iterator last,
                                                Comparator comp )
{
    return std::stable_sort( first, last, comp );
}

template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::find(
        typename WSharedSequenceContainer< S >::Iterator first,
        typename WSharedSequenceContainer< S >::Iterator last,
        const typename S::value_type& value )
{
    return std::find( first, last, value );
}

template < typename S >
typename WSharedSequenceContainer< S >::ConstIterator WSharedSequenceContainer< S >::find( const typename S::value_type& value )
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return std::find( a->get().begin(), a->get().end(), value );
}

#endif  // WSHAREDSEQUENCECONTAINER_H

