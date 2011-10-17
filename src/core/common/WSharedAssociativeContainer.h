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

#ifndef WSHAREDASSOCIATIVECONTAINER_H
#define WSHAREDASSOCIATIVECONTAINER_H

#include <utility>

#include <boost/thread.hpp>

#include "WSharedObject.h"

/**
 * This class provides a common interface for thread-safe access to associative containers (set, multiset, map, multimap).
 */
template < typename T >
class WSharedAssociativeContainer: public WSharedObject< T >
{
public:
    // Some helpful typedefs

    /**
     * A typedef for the correct const iterator useful to traverse this sequence container.
     */
    typedef typename T::const_iterator   ConstIterator;

    /**
     * A typedef for the correct iterator to traverse this sequence container.
     */
    typedef typename T::iterator         Iterator;

    /**
     * The type of the elements
     */
    typedef typename T::value_type value_type;

    /**
     * The type of the key used in this associative container
     */
    typedef typename T::key_type key_type;

    /**
     * Default constructor.
     */
    WSharedAssociativeContainer();

    /**
     * Destructor.
     */
    virtual ~WSharedAssociativeContainer();

    /**
     * Clears the container.
     */
    void clear();

    /**
     * Return true if the container is empty. The sense and non-sense of this method in a multi threaded environment is questionable.
     *
     * \return true if empty
     */
    bool empty() const;

    /**
     * The current size of the container. 0 if empty. The sense and non-sense of this method in a multi threaded environment is questionable.
     *
     * \return the size.
     */
    size_t size() const;

    /**
     * The maximum size of a container.
     *
     * \return the maximum size
     */
    size_t max_size() const;

    /**
     * Count elements with a specific key. The sense and non-sense of this method in a multi threaded environment is questionable.
     *
     * \param x the key
     *
     * \return the count, 0 if none found.
     */
    size_t count( const key_type& x ) const;

    /**
     * Erases the element with the specified key.
     *
     * \param x the key
     *
     * \return the number of elements erased
     */
    size_t erase( const key_type& x );

    /**
     * Inserts the specified element.
     *
     * \param x the element to add
     *
     * \return a pair containing the Iterator pointing to the inserted element and the bool is true if the element not existed before.
     */
    std::pair< Iterator, bool > insert( const value_type& x );

protected:
private:
};

template < typename T >
WSharedAssociativeContainer< T >::WSharedAssociativeContainer():
    WSharedObject< T >()
{
    // init members
}

template < typename T >
WSharedAssociativeContainer< T >::~WSharedAssociativeContainer()
{
    // clean up
}

template < typename T >
void WSharedAssociativeContainer< T >::clear()
{
    typename WSharedAssociativeContainer< T >::WriteTicket w = WSharedObject< T >::getWriteTicket();
    w->get().clear();
}

template < typename T >
bool WSharedAssociativeContainer< T >::empty() const
{
    typename WSharedAssociativeContainer< T >::ReadTicket r = WSharedObject< T >::getReadTicket();
    return r->get().empty();
}

template < typename T >
size_t WSharedAssociativeContainer< T >::size() const
{
    typename WSharedAssociativeContainer< T >::ReadTicket r = WSharedObject< T >::getReadTicket();
    return r->get().size();
}

template < typename T >
size_t WSharedAssociativeContainer< T >::max_size() const
{
    typename WSharedAssociativeContainer< T >::ReadTicket r = WSharedObject< T >::getReadTicket();
    return r->get().max_size();
}

template < typename T >
size_t WSharedAssociativeContainer< T >::count( const key_type& x ) const
{
    typename WSharedAssociativeContainer< T >::ReadTicket r = WSharedObject< T >::getReadTicket();
    return r->get().count( x );
}

template < typename T >
size_t WSharedAssociativeContainer< T >::erase( const key_type& x )
{
    typename WSharedAssociativeContainer< T >::WriteTicket w = WSharedObject< T >::getWriteTicket();
    return w->get().erase( x );
}

template < typename T >
std::pair< typename WSharedAssociativeContainer< T >::Iterator, bool > WSharedAssociativeContainer< T >::insert( const value_type& x )
{
    typename WSharedAssociativeContainer< T >::WriteTicket w = WSharedObject< T >::getWriteTicket();
    return w->get().insert( x );
}

#endif  // WSHAREDASSOCIATIVECONTAINER_H

