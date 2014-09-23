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

#ifndef WDEFERREDCALLEVENT_H
#define WDEFERREDCALLEVENT_H

#include <boost/function.hpp>

#include <QtCore/QEvent>

#include "core/common/WConditionOneShot.h"

#include "WEventTypes.h"

/**
 * A Qt event to call a function from within the GUI thread.
 */
class WDeferredCallEventBase: public QEvent
{
public:
    /**
     * Constructor.
     *
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call.
     */
    explicit WDeferredCallEventBase( WCondition::SPtr notify = WCondition::SPtr() ):
    QEvent( CUSTOM_TYPE ),
    m_callCondition( notify )
    {
    }

    /**
     * Destructor
     */
    virtual ~WDeferredCallEventBase()
    {
    }

    /**
     * Constant which saves the number used to distinguish this event from other
     * custom events.
     */
    static const QEvent::Type CUSTOM_TYPE = static_cast< QEvent::Type >( WQT_DEFERREDCALL );

    /**
     * Call the function.
     */
    void call()
    {
        callImpl();
        if( m_callCondition )
        {
            m_callCondition->notify();
        }
    }

    /**
     * Get the condition that notifies about the finished execution of the specified function.
     *
     * \return the condition. Might be NULL if none was specified.
     */
    WConditionOneShot::SPtr getDoneCondition() const
    {
        return m_callCondition;
    }
protected:
    /**
     * Fired whenever the function was called.
     */
    WCondition::SPtr m_callCondition;

    /**
     * Call the functor.
     */
    virtual void callImpl() = 0;
private:
};

/**
 * Derived WDeferredCallEvent allowing result values in calls.
 *
 * \tparam Result return type
 */
template< typename Result >
class WDeferredCallResultEvent: public WDeferredCallEventBase
{
public:
    /**
     * Constructor.
     *
     * \param function the function to call
     * \param result a pointer to store the result in. Can be NULL if you are not interested in the result. You need to alloc and free it. The
     * reason for using an external pointer is that Qt doc tells us that you loose ownership on QEvents when using QCoreApplication::postEvent.
     * The Event will be deleted after being processed, thus we cannot ensure safe access to members after posting the event.
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call.
     */
    WDeferredCallResultEvent( boost::function< Result( void ) > function, Result* result, WCondition::SPtr notify = WCondition::SPtr() ):
        WDeferredCallEventBase( notify ),
        m_result( result ),
        m_function( function )
    {
    }

    /**
     * Destructor
     */
    virtual ~WDeferredCallResultEvent()
    {
        // nothing to do
    }

protected:
    /**
     * Call the function.
     */
    virtual void callImpl()
    {
        if( m_result )
        {
            *m_result = m_function();
        }
        else
        {
            m_function();
        }
    }
private:
    /**
     * The result.
     */
    Result* m_result;

    /**
     * the title of the widget to create
     */
    boost::function< Result( void ) > m_function;
};

/**
 * Derived WDeferredCallEvent allowing result values in calls. Specialization for VOID
 */
template<>
class WDeferredCallResultEvent< void >: public WDeferredCallEventBase
{
public:
    /**
     * Constructor.
     *
     * \param function the function to call
     * \param notify specify your own condition to wait for. This is needed since the QApplication doc tells us that ownership of an event is
     * handed over to QT and that it is not save to use the event after posting it. This means we cannot utilize an internal condition in the
     * event as it might be deleted already when calling wait() on it. Do not specify this variable to get a fire-and-forget call.
     */
    WDeferredCallResultEvent( boost::function< void( void ) > function, WCondition::SPtr notify = WCondition::SPtr() ):
        WDeferredCallEventBase( notify ),
        m_function( function )
    {
    }

    /**
     * Destructor
     */
    virtual ~WDeferredCallResultEvent()
    {
        // nothing to do
    }

protected:
    /**
     * Call the function.
     */
    virtual void callImpl()
    {
        m_function();
    }
private:
    /**
     * the title of the widget to create
     */
    boost::function< void( void ) > m_function;
};

/**
 * Default deferred call without return type
 */
typedef WDeferredCallResultEvent< void > WDeferredCallEvent;

#endif  // WDEFERREDCALLEVENT_H
