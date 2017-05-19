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

#ifndef WCONDITION_H
#define WCONDITION_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/signals2/signal.hpp>
#include <boost/thread.hpp>



/**
 * Class to encapsulate boost::condition_variable_any. You may use it to efficiently wait for events (a condition comes true). It
 * is a very simple implementation. It might be extended easily. Timed wait functions and so on.
 */
class WCondition // NOLINT
{
    friend class WCondition_test; //!< Access for test class.
public:
    /**
     * Shared pointer type for WCondition.
     */
    typedef boost::shared_ptr< WCondition > SPtr;

    /**
     * Const shared pointer type for WCondition.
     */
    typedef boost::shared_ptr< const WCondition > ConstSPtr;

    /**
     * Default constructor.
     */
    WCondition();

    /**
     * Destructor.
     */
    virtual ~WCondition();

    /**
     * Wait for the condition. Sets the calling thread asleep.
     */
    virtual void wait() const;

    /**
     * Notifies all waiting threads.
     */
    virtual void notify();

    /**
     * Type used for signalling condition changes.
     */
    typedef boost::function0< void > t_ConditionNotifierType;

    /**
     * Subscribes a specified function to be notified on condition change.
     *
     * \param notifier the notifier function
     *
     * \return the connection.
     */
    boost::signals2::connection subscribeSignal( t_ConditionNotifierType notifier ) const;

protected:
    /**
     * Type used for condition notification.
     */
    typedef boost::signals2::signal<void ( void )>  t_ConditionSignalType;

    /**
     * Signal getting fired whenever the condition fires.
     */
    mutable t_ConditionSignalType signal_ConditionFired;

    /**
     * The condition.
     */
    mutable boost::condition_variable_any m_condition;

    /**
     * The mutex used for the condition.
     */
    mutable boost::shared_mutex m_mutex;

private:
};

#endif  // WCONDITION_H

