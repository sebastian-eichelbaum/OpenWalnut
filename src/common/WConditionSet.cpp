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

#include "iostream"

#include "WConditionSet.h"

WConditionSet::WConditionSet():
    WCondition()
{
}

WConditionSet::~WConditionSet()
{
    // clean conditions list
    m_conditionSet.clear();
}

void WConditionSet::add( boost::shared_ptr< WCondition > condition )
{
    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_conditionSetLock );
    m_conditionSet.insert( condition );
    lock.unlock();

    condition->subscribeSignal( boost::bind( &WConditionSet::conditionFired, this ) );
}

void WConditionSet::remove( boost::shared_ptr< WCondition > condition )
{
    condition->unsubscribeSignal( boost::bind( &WConditionSet::conditionFired, this ) );

    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_conditionSetLock );
    m_conditionSet.erase( condition );
    lock.unlock();
}

void WConditionSet::conditionFired()
{
    notify();
}

