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

#include <map>
#include <utility>

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
friend class WConditionSetTest;
public:
    /**
     * Shared pointer to instance of this class.
     */
    typedef boost::shared_ptr< WConditionSet > SPtr;

    /**
     * Shared pointer to const instance of this class.
     */
    typedef boost::shared_ptr< const WConditionSet > ConstSPtr;

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

    /**
     * Wait for the condition. Sets the calling thread asleep. If the condition set is resetable, this will return immediately
     * when a condition in the set fired in the past and there has been no reset() call until now.
     */
    virtual void wait() const;

    /**
     * Resets the internal fire state. This does nothing if !isResetable().
     */
    virtual void reset() const;

    /**
     * Sets the resetable flag. This causes the condition set to act like a WConditionOneShot. There are several implications to
     * this you should consider when using the condition set as a resetable. If one condition in the condition set fires, a
     * subsequent call to wait() will immediately return until a reset() call has been done. If you share one condition set among
     * several threads, you should consider, that one thread can reset the condition set before the other thread had a chance to
     * call wait() which causes the other thread to wait until the next condition in the set fires.
     *
     * \param resetable true if the fire state should be delayed and can be reseted.
     * \param autoReset true if the state should be reset whenever a wait call is called and continues.This is especially useful if a
     * condition set is used only by one thread, so there is no need to call reset() explicitly.
     */
    void setResetable( bool resetable = true, bool autoReset = true );

    /**
     * Returns whether the condition set acts like a one shot condition.
     *
     * \return true if the fire state is delayed and can be reseted.
     */
    bool isResetable();

protected:
    /**
     * Flag denoting whether the condition set should act like a one shot condition.
     */
    bool m_resetable;

    /**
     * Flag which shows whether the wait() call should reset the state m_fired when it returns.
     */
    bool m_autoReset;

    /**
     * We need to keep track of the connections a condition has made since boost::function objects do not provide a == operator and can therefore
     * not easily be removed from a signals by signal.desconnect( functor ).
     */
    typedef std::map< boost::shared_ptr< WCondition >, boost::signals2::connection > ConditionConnectionMap;

    /**
     * Set of conditions to be waited for.
     */
    ConditionConnectionMap m_conditionSet;

    /**
     * Each condition has a connection.
     */
    typedef std::pair< boost::shared_ptr< WCondition >, boost::signals2::connection > ConditionConnectionPair;

    /**
     * Lock used for thread-safe writing to the condition set.
     */
    boost::shared_mutex m_conditionSetLock;

    /**
     * Notifier function getting notified whenever a condition got fired.
     */
    virtual void conditionFired();

    /**
     * Flag denoting whether one condition fired in the past. Just useful when m_resetable is true.
     */
    mutable bool m_fired;

    /**
     * The notifier which gets called by all conditions if they fire
     */
    WCondition::t_ConditionNotifierType m_notifier;

private:
};

#endif  // WCONDITIONSET_H

