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

#include "WCondition.h"

WCondition::WCondition()
{
    // initialize members
}

WCondition::~WCondition()
{
    // cleanup
}

void WCondition::wait() const
{
    // since Boost 1.54, we need to explicitly lock the mutex prior to wait.
    boost::unique_lock<boost::shared_mutex> lock( m_mutex );
    m_condition.wait( m_mutex );
}

void WCondition::notify()
{
    signal_ConditionFired();
    m_condition.notify_all();
}

boost::signals2::connection WCondition::subscribeSignal( t_ConditionNotifierType notifier ) const
{
    return signal_ConditionFired.connect( notifier );
}

