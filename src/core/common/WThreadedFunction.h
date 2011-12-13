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

#ifndef WTHREADEDFUNCTION_H
#define WTHREADEDFUNCTION_H

#include <memory.h>
#include <iostream>

#include <string>
#include <vector>
#include <boost/thread.hpp>

#include "WAssert.h"
#include "WWorkerThread.h"
#include "WSharedObject.h"
#include "WExportCommon.h"

/**
 * An enum indicating the status of a multithreaded computation
 */
enum WThreadedFunctionStatus
{
    W_THREADS_INITIALIZED,      //! the status after constructing the function
    W_THREADS_RUNNING,          //! the threads were started
    W_THREADS_STOP_REQUESTED,   //! a stop was requested and not all threads have stopped yet
    W_THREADS_ABORTED,          //! at least one thread was aborted due to a stop request or an exception
    W_THREADS_FINISHED          //! all threads completed their work successfully
};

/**
 * An enum indicating the number of threads used
 */
enum WThreadedFunctionNbThreads
{
    W_AUTOMATIC_NB_THREADS = 0      //!< Use half the available cores as number of threads
};

/**
 * \class WThreadedFunctionBase
 *
 * A virtual base class for threaded functions (see below).
 */
class OWCOMMON_EXPORT WThreadedFunctionBase // NOLINT
{
    //! a type for exception signals
    typedef boost::signal< void ( WException const& ) > ExceptionSignal;

public:
    //! a type for exception callbacks
    typedef boost::function< void ( WException const& ) > ExceptionFunction;

    /**
     * Standard constructor.
     */
    WThreadedFunctionBase();

    /**
     * Destroys the thread pool and stops all threads, if any one of them is still running.
     *
     * \note Of course, the client has to make sure the threads do not work endlessly on a single job.
     */
    virtual ~WThreadedFunctionBase();

    /**
     * Starts the threads.
     */
    virtual void run() = 0;

    /**
     * Request all threads to stop. Returns immediately, so you might
     * have to wait() for the threads to actually finish.
     */
    virtual void stop() = 0;

    /**
     * Wait for all threads to stop.
     */
    virtual void wait() = 0;

    /**
     * Get the status of the threads.
     *
     * \return The current status.
     */
    WThreadedFunctionStatus status();

    /**
     * Returns a condition that gets fired when all threads have finished.
     *
     * \return The condition indicating all threads are done.
     */
    boost::shared_ptr< WCondition > getThreadsDoneCondition();

    /**
     * Subscribe a function to an exception signal.
     *
     * \param func The function to subscribe.
     */
    void subscribeExceptionSignal( ExceptionFunction func );

protected:
    /**
     * WThreadedFunctionBase is non-copyable, so the copy constructor is not implemented.
     */
    WThreadedFunctionBase( WThreadedFunctionBase const& ); // NOLINT

    /**
     * WThreadedFunctionBase is non-copyable, so the copy operator is not implemented.
     *
     * \return this function
     */
    WThreadedFunctionBase& operator = ( WThreadedFunctionBase const& );

    //! a condition that gets notified when the work is complete
    boost::shared_ptr< WCondition > m_doneCondition;

    //! a signal for exceptions
    ExceptionSignal m_exceptionSignal;

    //! the current status
    WSharedObject< WThreadedFunctionStatus > m_status;
};

/**
 * \class WThreadedFunction
 *
 * Creates threads that computes a function in a multithreaded fashion. The template parameter
 * is an object that provides a function to execute. The following function needs to be implemented:
 *
 * void operator ( std::size_t id, std::size_t mx, WBoolFlag const& s );
 *
 * Here, 'id' is the number of the thread currently executing the function, ranging from
 * 0 to mx - 1, where 'mx' is the number of threads running. 's' is a flag that indicates
 * if the execution should be stopped. Make sure to check the flag often, so that the threads
 * can be stopped when needed.
 *
 * This class itself is NOT thread-safe, do not access it from different threads simultaneously.
 * Also, make sure any resources used by your function are accessed in a threadsafe manner,
 * as all threads share the same function object.
 *
 * Any exception thrown by your function will be caught and forwarded via the exception
 * signal. Beware that the signal function will be called in the executing threads, as opposed
 * to in your module thread. This means that the exception handler bound to the exception
 * signal must be threadsafe too.
 *
 * The status of the execution can be checked via the status() function. Also, when all threads
 * finish (due to throwing exceptions or actually successfully finishing computation ), a condition
 * will be notified.
 *
 * \ingroup common
 */
template< class Function_T >
class WThreadedFunction : public WThreadedFunctionBase
{
    //! a type for exception signals
    typedef boost::signal< void ( WException const& ) > ExceptionSignal;

public:
    //! a type for exception callbacks
    typedef boost::function< void ( WException const& ) > ExceptionFunction;

    /**
     * Creates the thread pool with a given number of threads.
     *
     * \param numThreads The number of threads to create.
     * \param function The function object.
     *
     * \note If the number of threads equals 0, a good number of threads will be determined by the threadpool.
     */
    WThreadedFunction( std::size_t numThreads, boost::shared_ptr< Function_T > function );

    /**
     * Destroys the thread pool and stops all threads, if any one of them is still running.
     *
     * \note Of course, the client has to make sure the threads do not work endlessly on a single job.
     */
    virtual ~WThreadedFunction();

    /**
     * Starts the threads.
     */
    virtual void run();

    /**
     * Request all threads to stop. Returns immediately, so you might
     * have to wait() for the threads to actually finish.
     */
    virtual void stop();

    /**
     * Wait for all threads to stop.
     */
    virtual void wait();

private:
    /**
     * WThreadedFunction is non-copyable, so the copy constructor is not implemented.
     */
    WThreadedFunction( WThreadedFunction const& ); // NOLINT

    /**
     * WThreadedFunction is non-copyable, so the copy operator is not implemented.
     *
     * \return this function
     */
    WThreadedFunction& operator = ( WThreadedFunction const& );

    /**
     * This function gets subscribed to the threads' stop signals.
     */
    void handleThreadDone();

    /**
     * This function handles exceptions thrown in the worker threads.
     *
     * \param e The exception that was thrown.
     */
    void handleThreadException( WException const& e );

    //! the number of threads to manage
    std::size_t m_numThreads;

    //! the threads
    // use shared_ptr here, because WWorkerThread is non-copyable
    std::vector< boost::shared_ptr< WWorkerThread< Function_T > > > m_threads;

    //! the function object
    boost::shared_ptr< Function_T > m_func;

    //! a counter that keeps track of how many threads have finished
    WSharedObject< std::size_t > m_threadsDone;
};

template< class Function_T >
WThreadedFunction< Function_T >::WThreadedFunction( std::size_t numThreads, boost::shared_ptr< Function_T > function )
    : WThreadedFunctionBase(),
      m_numThreads( numThreads ),
      m_threads(),
      m_func( function ),
      m_threadsDone()
{
    if( !m_func )
    {
        throw WException( std::string( "No valid thread function pointer." ) );
    }

    // find a suitable number of threads
    if( m_numThreads == W_AUTOMATIC_NB_THREADS )
    {
        m_numThreads = 1;
        while( m_numThreads < boost::thread::hardware_concurrency() / 2 && m_numThreads < 1024 )
        {
            m_numThreads *= 2;
        }
    }

    // set number of finished threads to 0
    m_threadsDone.getWriteTicket()->get() = 0;

    // create threads
    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        boost::shared_ptr< WWorkerThread< Function_T > > t( new WWorkerThread< Function_T >( m_func, k, m_numThreads ) );
        t->subscribeStopSignal( boost::bind( &WThreadedFunction::handleThreadDone, this ) );
        t->subscribeExceptionSignal( boost::bind( &WThreadedFunction::handleThreadException, this, _1 ) );
        m_threads.push_back( t );
    }
}

template< class Function_T >
WThreadedFunction< Function_T >::~WThreadedFunction()
{
    stop();
}

template< class Function_T >
void WThreadedFunction< Function_T >::run()
{
    // set the number of finished threads to 0
    m_threadsDone.getWriteTicket()->get() = 0;
    // change status
    m_status.getWriteTicket()->get() = W_THREADS_RUNNING;
    // start threads
    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        m_threads[ k ]->run();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::stop()
{
    // change status
    m_status.getWriteTicket()->get() = W_THREADS_STOP_REQUESTED;

    typename std::vector< boost::shared_ptr< WWorkerThread< Function_T > > >::iterator it;
    // tell the threads to stop
    for( it = m_threads.begin(); it != m_threads.end(); ++it )
    {
        ( *it )->requestStop();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::wait()
{
    typename std::vector< boost::shared_ptr< WWorkerThread< Function_T > > >::iterator it;
    // wait for the threads to stop
    for( it = m_threads.begin(); it != m_threads.end(); ++it )
    {
        ( *it )->wait();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::handleThreadDone()
{
    typedef typename WSharedObject< std::size_t >::WriteTicket WT;

    WT t = m_threadsDone.getWriteTicket();
    WAssert( t->get() < m_numThreads, "" );
    ++t->get();
    std::size_t k = t->get();
    t = WT();

    if( m_numThreads == k )
    {
        typedef typename WSharedObject< WThreadedFunctionStatus >::WriteTicket ST;
        ST s = m_status.getWriteTicket();
        if( s->get() == W_THREADS_RUNNING )
        {
            s->get() = W_THREADS_FINISHED;
        }
        else if( s->get() == W_THREADS_STOP_REQUESTED )
        {
            s->get() = W_THREADS_ABORTED;
        }
        else
        {
            throw WException( std::string( "Invalid status change." ) );
        }
        m_doneCondition->notify();
    }
}

template< class Function_T >
void WThreadedFunction< Function_T >::handleThreadException( WException const& e )
{
    // change status
    typedef typename WSharedObject< WThreadedFunctionStatus >::WriteTicket WT;
    WT w = m_status.getWriteTicket();
    WAssert( w->get() != W_THREADS_FINISHED &&
             w->get() != W_THREADS_ABORTED, "" );
    if( w->get() == W_THREADS_RUNNING )
    {
        w->get() = W_THREADS_STOP_REQUESTED;
    }
    // force destruction of the write ticket
    w = WT();
    // update the number of finished threads
    handleThreadDone();

    m_exceptionSignal( e );
}

#endif  // WTHREADEDFUNCTION_H
