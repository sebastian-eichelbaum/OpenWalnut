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

#ifndef WSHAREDOBJECT_H
#define WSHAREDOBJECT_H

#include <boost/thread.hpp>

#include "WCondition.h"
#include "WSharedObjectTicket.h"
#include "WSharedObjectTicketRead.h"
#include "WSharedObjectTicketWrite.h"

/**
 * Wrapper around an object/type for thread safe sharing of objects among multiple threads. The advantage of this class over WFlag
 * is, that WFlag just protects simple get/set operations, while this class can protect a whole bunch of operations on the
 * encapsulated object.
 */
template < typename T >
class WSharedObject
{
public:

    /**
     * Default constructor.
     */
    WSharedObject();

    /**
     * Destructor.
     */
    virtual ~WSharedObject();

    /**
     * Type for read tickets.
     */
    typedef boost::shared_ptr< WSharedObjectTicketRead< T > > ReadTicket;

    /**
     * Type for write tickets.
     */
    typedef boost::shared_ptr< WSharedObjectTicketWrite< T > > WriteTicket;

    /**
     * Returns a ticket to get read access to the contained data. After the ticket is freed, the read lock vanishes.
     *
     * \return the read ticket
     */
    ReadTicket getReadTicket() const;

    /**
     * Returns a ticket to get write access to the contained data. After the ticket is freed, the write lock vanishes.
     *
     * \param suppressNotify true if no notification should be send after unlocking.
     *
     * \return the ticket
     */
    WriteTicket getWriteTicket( bool suppressNotify = false ) const;

    /**
     * This condition fires whenever the encapsulated object changed. This is fired automatically by endWrite().
     *
     * \return the condition
     */
    boost::shared_ptr< WCondition > getChangeCondition() const;

protected:

    /**
     * The object wrapped by this class. This member is mutable as the \ref getReadTicket and \ref getWriteTicket functions are const but need a
     * non-const reference to m_object.
     */
    mutable T m_object;

    /**
     * The lock to ensure thread safe access. This member is mutable as the \ref getReadTicket and \ref getWriteTicket functions are const but need a
     * non-const reference to m_lock.
     */
    mutable boost::shared_ptr< boost::shared_mutex > m_lock;

    /**
     * This condition set fires whenever the contained object changes. This corresponds to the Observable pattern.
     */
    boost::shared_ptr< WCondition > m_changeCondition;

private:
};

template < typename T >
WSharedObject< T >::WSharedObject():
    m_lock( new boost::shared_mutex ),
    m_changeCondition( new WCondition() )
{
    // init members
}

template < typename T >
WSharedObject< T >::~WSharedObject()
{
    // clean up
}

template < typename T >
boost::shared_ptr< WCondition > WSharedObject< T >::getChangeCondition() const
{
    return m_changeCondition;
}

template < typename T >
typename WSharedObject< T >::ReadTicket WSharedObject< T >::getReadTicket() const
{
    return boost::shared_ptr< WSharedObjectTicketRead< T > >(
            new WSharedObjectTicketRead< T >( m_object, m_lock, boost::shared_ptr< WCondition >() )
    );
}

template < typename T >
typename WSharedObject< T >::WriteTicket WSharedObject< T >::getWriteTicket( bool suppressNotify ) const
{
    if( suppressNotify )
    {
        return boost::shared_ptr< WSharedObjectTicketWrite< T > >(
                new WSharedObjectTicketWrite< T >( m_object, m_lock, boost::shared_ptr< WCondition >() )
        );
    }
    else
    {
        return boost::shared_ptr< WSharedObjectTicketWrite< T > >(
                new WSharedObjectTicketWrite< T >( m_object, m_lock, m_changeCondition )
        );
    }
}

#endif  // WSHAREDOBJECT_H

