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

#ifndef WOPENCUSTOMDOCKWIDGETEVENT_H
#define WOPENCUSTOMDOCKWIDGETEVENT_H

#include <string>

#include <QtCore/QEvent>

#include "core/graphicsEngine/WGECamera.h"
#include "core/common/WFlag.h"
#include "core/gui/WCustomWidget.h"

/**
 * A Qt event to open a new custom dock widget if posted to the WMainWindow.
 */
class WOpenCustomDockWidgetEvent : public QEvent
{
public:
    /**
     * Constructor
     *
     * \param title the title of the widget to open
     * \param projectionMode the kind of projection which should be used
     * \param flag The WFlag which contains the widget after its creation.
     */
    explicit WOpenCustomDockWidgetEvent( std::string title, WGECamera::ProjectionMode projectionMode,
        boost::shared_ptr< WFlag< boost::shared_ptr< WCustomWidget > > > flag );

    /**
     * Get the title of the widget to open.
     *
     * \return title of the widget to open
     */
    std::string getTitle() const;

    /**
     * Get the kind of projection which should be used.
     *
     * \return the kind of projection which should be used
     */
    WGECamera::ProjectionMode getProjectionMode() const;

    /**
     * Get the WFlag which contains the widget after its creation.
     *
     * \return a shared pointer to the WFlag
     */
    boost::shared_ptr< WFlag< boost::shared_ptr< WCustomWidget > > > getFlag() const;

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
     * the kind of projection which should be used
     */
    WGECamera::ProjectionMode m_projectionMode;

    /**
     * WFlag which contains the widget after its creation.
     */
    boost::shared_ptr< WFlag< boost::shared_ptr< WCustomWidget > > > m_flag;
};

#endif  // WOPENCUSTOMDOCKWIDGETEVENT_H
