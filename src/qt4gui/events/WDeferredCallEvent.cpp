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

#include "WDeferredCallEvent.h"

WDeferredCallEvent::WDeferredCallEvent( boost::function< void( void ) > function, WCondition::SPtr notify ):
    QEvent( CUSTOM_TYPE ),
    m_function( function ),
    m_callCondition( notify )
{
}

void WDeferredCallEvent::call()
{
    m_function();
    if( m_callCondition )
    {
        m_callCondition->notify();
    }
}

WConditionOneShot::SPtr WDeferredCallEvent::getDoneCondition() const
{
    return m_callCondition;
}
