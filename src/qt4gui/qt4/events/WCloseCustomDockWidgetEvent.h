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

#ifndef WCLOSECUSTOMDOCKWIDGETEVENT_H
#define WCLOSECUSTOMDOCKWIDGETEVENT_H

#include <string>

#include <QtCore/QEvent>

#include "core/common/WCondition.h"

#include "WEventTypes.h"

/**
 * A Qt event to close an existing custom dock widget if posted to the WMainWindow.
 */
class WCloseCustomDockWidgetEvent : public QEvent
{
public:
    /**
     * Constructor. Use the doneCondition to wait for the event to be processed.
     *
     * \param title the title of the widget to open
     */
    explicit WCloseCustomDockWidgetEvent( std::string title );

    /**
     * Get the title of the widget to open.
     *
     * \return title of the widget to open
     */
    std::string getTitle() const;

    /**
     * Constant which saves the number used to distinguish this event from other
     * custom events.
     */
    static const QEvent::Type CUSTOM_TYPE = static_cast< QEvent::Type >( WQT_CLOSECUSTOMDOCKWIDGET );

protected:
private:
    /**
     * the title of the widget to create
     */
    std::string m_title;
};

#endif  // WCLOSECUSTOMDOCKWIDGETEVENT_H
