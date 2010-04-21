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

#include <iostream>

#include "WConditionOneShot.h"
#include "WCondition.h"
#include "WLogger.h"

#include "WThreadedRunner.h"

WThreadedRunner::WThreadedRunner():
    m_shutdownFlag( new WConditionOneShot(), false )
{
    // initialize members
}

WThreadedRunner::~WThreadedRunner()
{
    // cleanup
    // XXX is this working if thread already has finished?
    // wait( true ); <-- no
}

void WThreadedRunner::run()
{
    run( boost::bind( &WThreadedRunner::threadMain, this ) );
}

void WThreadedRunner::run( THREADFUNCTION f )
{
    m_thread = boost::thread( f );
}

void WThreadedRunner::wait( bool requestFinish )
{
    if( requestFinish )
    {
        requestStop();
    }
    m_thread.join();
}

void WThreadedRunner::requestStop()
{
    // first notify
    notifyStop();

    // then signal it
    m_shutdownFlag( true );
}

void WThreadedRunner::waitForStop()
{
    m_shutdownFlag.wait();
}

void WThreadedRunner::threadMain()
{
    WLogger::getLogger()->addLogMessage( "This should never be called. Implement a thread function here.", "WThreadedRunner", LL_WARNING );
}

void WThreadedRunner::notifyStop()
{
}

void WThreadedRunner::yield() const
{
    m_thread.yield();
}

void WThreadedRunner::sleep( const int32_t t ) const
{
    boost::this_thread::sleep( boost::posix_time::seconds( t ) );
}

void WThreadedRunner::msleep( const int32_t t ) const
{
    boost::this_thread::sleep( boost::posix_time::microseconds( t ) );
}

