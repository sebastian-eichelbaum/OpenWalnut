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

#include "WCustomWidget.h"

WCustomWidget::WCustomWidget( std::string title ):
    m_title( title ),
    m_eventOccured( new WBoolFlag( WCondition::SPtr( new WCondition() ), false ) )
{
}

WCustomWidget::~WCustomWidget()
{
}

std::string WCustomWidget::getTitle() const
{
    return m_title;
}

const osgGA::GUIEventAdapter& WCustomWidget::getEvent( bool reset )
{
    if( reset )
    {
        m_eventOccured->set( false, true ); // reset and suppress condition notification
    }
    return *m_event;
}

WCondition::SPtr WCustomWidget::getCondition() const
{
    return m_eventOccured->getCondition();
}

bool WCustomWidget::eventOccured() const
{
    return m_eventOccured->get( false ); // don't eat change
}
