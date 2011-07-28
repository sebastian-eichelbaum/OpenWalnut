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

#include "WConditionSet.h"

WConditionSet::WConditionSet():
    WCondition(),
    m_resetable( false ),
    m_autoReset( false ),
    m_fired( false ),
    m_notifier( boost::bind( &WConditionSet::conditionFired, this ) )
{
}

WConditionSet::~WConditionSet()
{
    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_conditionSetLock );

    // clean conditions list
    // NOTE: we need to disconnect here.
    for( ConditionConnectionMap::iterator it = m_conditionSet.begin(); it != m_conditionSet.end(); ++it )
    {
        ( *it ).second.disconnect();
    }

    m_conditionSet.clear();
    lock.unlock();
}

void WConditionSet::add( boost::shared_ptr< WCondition > condition )
{
    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_conditionSetLock );

    if( !m_conditionSet.count( condition ) )
    {
        // create a new pair, the condition and its connection object.
        // this is needed since remove needs the connection to disconnect the notifier again
        m_conditionSet.insert( ConditionConnectionPair( condition, condition->subscribeSignal( m_notifier ) ) );
    }

    lock.unlock();
}

void WConditionSet::remove( boost::shared_ptr< WCondition > condition )
{
    // get write lock
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_conditionSetLock );

    // get the element
    ConditionConnectionMap::iterator it = m_conditionSet.find( condition );
    if( it != m_conditionSet.end() )
    {
        ( *it ).second.disconnect();
        m_conditionSet.erase( it );
    }

    lock.unlock();
}

void WConditionSet::conditionFired()
{
    m_fired = true;
    notify();
}

void WConditionSet::wait() const
{
    if( !m_resetable || !m_fired )
    {
        WCondition::wait();
    }

    if( m_autoReset )
    {
        reset();
    }
}

void WConditionSet::reset() const
{
    m_fired = false;
}

void WConditionSet::setResetable( bool resetable, bool autoReset )
{
    m_autoReset = autoReset;
    m_resetable = resetable;
}

bool WConditionSet::isResetable()
{
    return m_resetable;
}

