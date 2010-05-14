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

#ifndef WSHAREDOBJECTTICKETREAD_H
#define WSHAREDOBJECTTICKETREAD_H

#include <boost/shared_ptr.hpp>

#include "WCondition.h"
#include "WSharedObjectTicket.h"

/**
 * Class which represents granted access to a locked object. It contains a reference to the object and a lock. The lock is freed after the ticket
 * has been destroyed.  This class especially implements the shared (read) lock.
 */
template < typename Data >
class WSharedObjectTicketRead: public WSharedObjectTicket< Data >
{
// the shared object class needs protected access to create new instances
friend class WSharedObject< Data >;
public:

    /**
     * Destroys the ticket and releases the lock.
     */
    virtual ~WSharedObjectTicketRead()
    {
        // explicitly unlock to ensure the WSharedObjectTicket destructor can call the update callback AFTER the lock has been released
        unlock();
    };

    /**
     * Returns the protected data. As long as you own the ticket, you are allowed to use it.
     *
     * \note making it const enforces const correctness for contained types!
     *
     * \return the data (const!)
     */
    const Data& get() const
    {
        return WSharedObjectTicket< Data >::m_data;
    };

protected:

    /**
     * Create a new instance. It is protected to avoid someone to create them. It locks the mutex for read.
     *
     * \param data the data to protect
     * \param mutex the mutex used to lock
     * \param condition a condition that should be fired upon unlock. Can be NULL.
     */
    WSharedObjectTicketRead( Data& data, boost::shared_ptr< boost::shared_mutex > mutex, boost::shared_ptr< WCondition > condition ): // NOLINT
        WSharedObjectTicket< Data >( data, mutex, condition ),
        m_lock( boost::shared_lock< boost::shared_mutex >( *mutex ) )
    {
    };

    /**
     * The lock.
     */
    boost::shared_lock< boost::shared_mutex > m_lock;

    /**
     * Unlocks the mutex.
     */
    virtual void unlock()
    {
        m_lock.unlock();
    }

private:
};

#endif  // WSHAREDOBJECTTICKETREAD_H

