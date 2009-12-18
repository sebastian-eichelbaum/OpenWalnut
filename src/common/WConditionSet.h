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

#ifndef WCONDITIONSET_H
#define WCONDITIONSET_H

#include <set>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "WCondition.h"

/**
 * Class allowing multiple conditions to be used for one waiting cycle. Since wait() can not be used for waiting on multiple
 * conditions, this class can encapsulate multiple conditions and offer a wait() command to wait for one of them to change its
 * state. Please not that this class can also be used as condition.
 */
class WConditionSet: public WCondition
{
public:

    /**
     * Default constructor.
     */
    WConditionSet();

    /**
     * Destructor.
     */
    virtual ~WConditionSet();

    /**
     * Adds another condition to the set of conditions to wait for. Note that, whenever someone is waiting for this WConditionSet,
     * the newly added one is also directly included into the wait() call.
     *
     * \param condition the condition to add.
     */
    virtual void add( boost::shared_ptr< WCondition > condition );

    /**
     * Removes the specified condition. As add() this immediately takes effect on running wait() calls.
     *
     * \param condition the condition to remove
     */
    virtual void remove( boost::shared_ptr< WCondition > condition );

protected:

    /**
     * Set of conditions to be waited for.
     */
    std::set< boost::shared_ptr< WCondition > > m_conditionSet;

    /**
     * Lock used for thread-safe writing to the condition set.
     */
    boost::shared_mutex m_conditionSetLock;

    /**
     * Notifier function getting notified whenever a condition got fired.
     */
    virtual void conditionFired();

private:
};

#endif  // WCONDITIONSET_H

