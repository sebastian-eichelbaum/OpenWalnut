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

#include <string>

#include "../../common/WConditionOneShot.h"
#include "WCreateCustomDockWidgetEvent.h"

WCreateCustomDockWidgetEvent::WCreateCustomDockWidgetEvent(
    std::string title, WGECamera::ProjectionMode projectionMode, boost::shared_ptr< WConditionOneShot > condition )
    : QEvent( CUSTOM_TYPE ),
      m_title( title ),
      m_projectionMode( projectionMode ),
      m_condition( condition )
{
}

std::string WCreateCustomDockWidgetEvent::getTitle() const
{
    return m_title;
}

WGECamera::ProjectionMode WCreateCustomDockWidgetEvent::getProjectionMode() const
{
    return m_projectionMode;
}

boost::shared_ptr< WConditionOneShot > WCreateCustomDockWidgetEvent::getCondition() const
{
    return m_condition;
}
