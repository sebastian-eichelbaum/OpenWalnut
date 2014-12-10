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

#ifndef WCOUNTER_H
#define WCOUNTER_H

#include <boost/thread.hpp>

/**
 * This is a simple but thread-safe counter. Use it to handle counting between multiple threads. When used as static member, you can utilized
 * WCounter for instance counting.
 */
class WCounter
{
public:
    /**
     * Constructor. Counter starts at 0.
     */
    WCounter()
        : m_counterMutex(),
          m_counter( 0 )
    {
    }

    /**
     * Increase count by 1 and return the new counter value. This is threadsafe.
     *
     * \return The new counter value.
     */
    int operator++ ()
    {
        boost::unique_lock< boost::shared_mutex > lock( m_counterMutex );
        return ++m_counter;
    }

    /**
     * Decrease count by 1 and return the new counter value. This is threadsafe.
     *
     * \return The new counter value.
     */
    int operator-- ()
    {
        boost::unique_lock< boost::shared_mutex > lock( m_counterMutex );
        return --m_counter;
    }

    /**
     * Get current count.
     *
     * \return the current count.
     */
    int operator() () const
    {
        boost::shared_lock< boost::shared_mutex > lock( m_counterMutex );
        return m_counter;
    }

    /**
     * Reset the counter to 0.
     *
     * \return The value the counter had before the reset.
     */
    int reset()
    {
        boost::unique_lock< boost::shared_mutex > lock( m_counterMutex );
        int tmp = m_counter;
        m_counter = 0;
        return tmp;
    }

private:
    //! No copy construction.
    WCounter( WCounter& /* count */ );

    /**
     * No copy operator.
     *
     * \return Nothing.
     */
    WCounter operator= ( WCounter& /* count */ );

    //! A mutex to protect the counter from concurrent updates.
    mutable boost::shared_mutex m_counterMutex;

    //! The counter.
    int m_counter;
};

#endif  // WCOUNTER_H

