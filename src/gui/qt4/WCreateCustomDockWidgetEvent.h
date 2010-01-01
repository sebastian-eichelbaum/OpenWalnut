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

#ifndef WCREATECUSTOMDOCKWIDGETEVENT_H
#define WCREATECUSTOMDOCKWIDGETEVENT_H

#include <string>
#include <QtCore/QEvent>

#include "../../common/WConditionOneShot.h"

/**
 * A Qt event to create a new custom dock widget if posted to the WMainWindow.
 */
class WCreateCustomDockWidgetEvent : public QEvent
{
public:
    /**
     * constructor
     *
     * \param title The title of the widget to create.
     * \param condition The condition which waits until the widget is created.
     */
    explicit WCreateCustomDockWidgetEvent( std::string title, boost::shared_ptr< WConditionOneShot > condition );

    /**
     * Get the title of the widget to create.
     *
     * \return title of the widget to create
     */
    std::string getTitle() const;

    /**
     * Get the condition which waits until the widget is created.
     *
     * \return a shared pointer to the condition
     */
    boost::shared_ptr< WConditionOneShot > getCondition() const;

    /**
     * Constant which saves the number used to distinguish this event from other
     * custom events.
     */
    static const QEvent::Type CUSTOM_TYPE = static_cast< QEvent::Type >( 51051 );

protected:
private:
    /**
     * the title of the widget to create
     */
    std::string m_title;

    /**
     * Condition which waits until the widget is created.
     */
    boost::shared_ptr< WConditionOneShot > m_condition;
};

#endif  // WCREATECUSTOMDOCKWIDGETEVENT_H
