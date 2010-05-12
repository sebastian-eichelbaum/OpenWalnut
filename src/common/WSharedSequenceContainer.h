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

#include <boost/thread.hpp>

#include "WSharedObject.h"

/**
 * This class provides a common interface for thread-safe access to sequence containers (list, vector, dequeue ).
 */
template < typename T, typename S >
class WSharedSequenceContainer: public WSharedObject< S >
{
public:

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
    void push_back( const T& x );

    /**
     * Removes an element from the end.
     */
    void pop_back();

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
    size_t size();

protected:

private:
};

template < typename T, typename S >
WSharedSequenceContainer< T, S >::WSharedSequenceContainer():
    WSharedObject< S >()
{
    // init members
}

template < typename T, typename S >
WSharedSequenceContainer< T, S >::~WSharedSequenceContainer()
{
    // clean up
}

template < typename T, typename S >
void WSharedSequenceContainer< T, S >::push_back( const T& x )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().push_back( x );
}

template < typename T, typename S >
void WSharedSequenceContainer< T, S >::pop_back()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().pop_back();
}

template < typename T, typename S >
void WSharedSequenceContainer< T, S >::clear()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().clear();
}

template < typename T, typename S >
size_t WSharedSequenceContainer< T, S >::size()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    size_t size = a->get().size();
    return size;
}

#endif  // WSHAREDSEQUENCECONTAINER_H

