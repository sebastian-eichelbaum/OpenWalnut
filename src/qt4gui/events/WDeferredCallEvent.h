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
class WDeferredCallEvent : public QEvent
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
    WDeferredCallEvent( boost::function< void( void ) >  function, WCondition::SPtr notify = WCondition::SPtr() );

    /**
     * Constant which saves the number used to distinguish this event from other
     * custom events.
     */
    static const QEvent::Type CUSTOM_TYPE = static_cast< QEvent::Type >( WQT_DEFERREDCALL );

    /**
     * Call the function.
     */
    void call();

    /**
     * Get the condition that notifies about the finished execution of the specified function.
     *
     * \return the condition. Might be NULL if none was specified.
     */
    WConditionOneShot::SPtr getDoneCondition() const;
protected:
private:
    /**
     * the title of the widget to create
     */
    boost::function< void( void ) > m_function;

    /**
     * Fired whenever the function was called.
     */
    WCondition::SPtr m_callCondition;
};

#endif  // WDEFERREDCALLEVENT_H
