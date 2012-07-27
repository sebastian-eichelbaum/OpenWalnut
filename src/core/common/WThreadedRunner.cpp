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

#if ( defined( __linux__ ) && !defined( __ANDROID__ ) )
#include <sys/prctl.h>
#endif

#include <iostream>
#include <string>

#include "exceptions/WSignalSubscriptionFailed.h"
#include "WCondition.h"
#include "WConditionOneShot.h"
#include "WException.h"
#include "WLogger.h"
#include "WThreadedRunner.h"

WThreadedRunner::WThreadedRunner():
    m_shutdownFlag( new WConditionOneShot(), false ),
    m_isCrashed( new WConditionOneShot(), false ),
    m_crashMessage( "" ),
    m_threadName( "" )
{
    // initialize members
}

WThreadedRunner::~WThreadedRunner()
{
    // cleanup
}

const WBoolFlag& WThreadedRunner::isCrashed() const
{
    return m_isCrashed;
}

void WThreadedRunner::onThreadException( const WException& e )
{
    handleDeadlyException( e );
}

void WThreadedRunner::handleDeadlyException( const WException& e, std::string sender )
{
    wlog::error( sender ) << "WException. Notifying. Message: " << e.what();

    // ensure proper exception propagation
    signal_thread_error( e );

    // notify waiting threads
    m_isCrashed( true );
    m_crashMessage = e.what();
}

void WThreadedRunner::run()
{
    run( boost::bind( &WThreadedRunner::threadMainSave, this ) );
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

void WThreadedRunner::threadMainSave()
{
    WThreadedRunner::setThisThreadName( getThreadName() );

    try
    {
        threadMain();
    }
    catch( const WException& e )
    {
        onThreadException( e );
    }
    catch( const std::exception& e )
    {
        onThreadException( WException( e ) );
    }
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

boost::signals2::connection WThreadedRunner::subscribeSignal( THREAD_SIGNAL signal, t_ThreadErrorSignalHandlerType notifier )
{
    switch( signal)
    {
        case WTHREAD_ERROR:
            return signal_thread_error.connect( notifier );
        default:
            throw WSignalSubscriptionFailed( "Could not subscribe to unknown signal." );
            break;
    }
}

void WThreadedRunner::setThreadName( std::string name )
{
    m_threadName = name;
}

std::string WThreadedRunner::getThreadName() const
{
    return m_threadName;
}

const std::string& WThreadedRunner::getCrashMessage() const
{
    return m_crashMessage;
}

void WThreadedRunner::setThisThreadName( std::string name )
{
#ifdef __linux__
    // set the name of the thread. This name is shown by the "top", for example.
    prctl( PR_SET_NAME, ( "openwalnut (" + name + ")" ).c_str() );
#endif
}
