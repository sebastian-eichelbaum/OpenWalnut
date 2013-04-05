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

WCustomWidget::WCustomWidget( std::string title, EventType subscription ):
    m_title( title ),
    m_eventNotifier( new WCondition() ),
    m_events( new WSharedSequenceContainer< EventDeque >() ),
    m_subscription( subscription )
{
}

WCustomWidget::~WCustomWidget()
{
}

std::string WCustomWidget::getTitle() const
{
    return m_title;
}

WCustomWidget::EventQueueSPtr WCustomWidget::getNextEvents()
{
    EventQueueSPtr unhandledEvents( new EventQueue() );
    WSharedSequenceContainer< EventDeque >::WriteTicket t = m_events->getWriteTicket();

    while( !t->get().empty() )
    {
        unhandledEvents->push( t->get().front() );
        t->get().pop_front();
    }
    return unhandledEvents;
}

WCondition::SPtr WCustomWidget::getEventNotifier() const
{
    return m_eventNotifier;
}

bool WCustomWidget::newEvents() const
{
    WSharedSequenceContainer< EventDeque >::ReadTicket t = m_events->getReadTicket();
    return t->get().size() > 0;
}
