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
#include "../common/WLogger.h"

#include "WThreadedRunner.h"

WThreadedRunner::WThreadedRunner():
#if !USE_BOOST_THREADS
    OpenThreads::Thread(),
#endif
    m_shutdownFlag( new WConditionOneShot, false )
{
    // initialize members
    m_FinishRequested = false;
    m_Thread = NULL;
#if !USE_BOOST_THREADS
    m_firstRun = true;
#endif
}

WThreadedRunner::~WThreadedRunner()
{
    // cleanup
    // XXX is this working if thread already has finished?
    // wait( true ); <-- no
}

void WThreadedRunner::run()
{
#if USE_BOOST_THREADS
    m_Thread = new boost::thread( boost::bind( &WThreadedRunner::threadMain, this ) );
#else
    if ( m_firstRun )
    {
        m_firstRun = false;
        m_Thread = this;
        Init();
        start();
    } else
    {
        threadMain();
    }
#endif
}

void WThreadedRunner::run( THREADFUNCTION f )
{
#if USE_BOOST_THREADS
    m_Thread = new boost::thread( f );
#else
    m_firstRun = false;
    m_Thread = this;
    Init();
    start();
    f();
#endif
}

void WThreadedRunner::wait( bool requestFinish )
{
    if( requestFinish )
    {
        // first notify
        notifyStop();

        // then signal it
        m_FinishRequested = true;
        m_shutdownFlag( true );
    }
    m_Thread->join();
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
#if USE_BOOST_THREADS
    m_Thread->yield();
#else
    m_Thread->YieldCurrentThread();
#endif
}

void WThreadedRunner::sleep( const int t ) const
{
#if USE_BOOST_THREADS
    boost::this_thread::sleep( boost::posix_time::seconds( t ) );
#else
    m_Thread->microSleep( t );
#endif
}

