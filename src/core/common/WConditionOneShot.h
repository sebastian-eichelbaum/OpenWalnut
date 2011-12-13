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

#ifndef WCONDITIONONESHOT_H
#define WCONDITIONONESHOT_H

#include <boost/thread.hpp>

#include "WCondition.h"
#include "WExportCommon.h"

/**
 * Implements a WCondition, but can be fired only ONCE. This is useful if you want to have a thread waiting for a condition but
 * you can not assure that the thread already waits when you set the condition. This would cause the thread to wait endlessly
 * because he does not know that you already fired it. Implementation is simple. The constructor uses a unique lock (write lock)
 * on a mutex. All waiting threads try to get a read lock which is not possible as long it is write-locked. The notify method
 * releases the write lock and all waiting threads can continue.
 */
class OWCOMMON_EXPORT WConditionOneShot: public WCondition
{
    friend class WConditionOneShot_test;
public:
    /**
     * Default constructor.
     */
    WConditionOneShot();

    /**
     * Destructor.
     */
    virtual ~WConditionOneShot();

    /**
     * Wait for the condition. Sets the calling thread asleep.
     */
    virtual void wait() const;

    /**
     * Notifies all waiting threads.
     */
    virtual void notify();

protected:
    /**
     * Locked as long the condition was not fired.
     */
    boost::unique_lock<boost::shared_mutex> m_lock;

private:
};

#endif  // WCONDITIONONESHOT_H

