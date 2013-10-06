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

#ifndef WWORKERTHREAD_H
#define WWORKERTHREAD_H

#include <string> // because of std::size_t
#include <exception>

#include <boost/shared_ptr.hpp>
#include <boost/signals2/signal.hpp>

#include "WAssert.h"
#include "WException.h"
#include "WThreadedRunner.h"

/**
 * A worker thread that belongs to a \see WThreadedFunction object.
 */
template< class Function_T >
class WWorkerThread : public WThreadedRunner
{
    // typedefs
    //! a type for stop signals
    typedef boost::signals2::signal< void () > StopSignal;

    //! a type for exception signals
    typedef boost::signals2::signal< void ( WException const& ) > ExceptionSignal;

public:
    //typedefs
    //! a type for stop callbacks
    typedef boost::function< void () > StopFunction;

    //! a type for exception callbacks
    typedef boost::function< void ( WException const& ) > ExceptionFunction;

    /**
     * Default constructor.
     *
     * \param func A pointer to the function object.
     * \param id A thread id.
     * \param numThreads The number of threads.
     */
    WWorkerThread( boost::shared_ptr< Function_T > func, std::size_t id, std::size_t numThreads );

    /**
     * Default destructor.
     */
    virtual ~WWorkerThread();

    /**
     * Subscribe a function to the exception signal.
     *
     * \param func The function.
     */
    void subscribeExceptionSignal( ExceptionFunction func );

    /**
     * Subscribe a function to the stop signal.
     *
     * \param func The function.
     */
    void subscribeStopSignal( StopFunction func );

protected:
    /**
     * The thread's main function.
     */
    virtual void threadMain();

private:
    /**
     * WWorkerThread is non-copyable, so the copy constructor is not implemented.
     */
    WWorkerThread( WWorkerThread const& ); // NOLINT

    /**
     * WWorkerThread is non-copyable, so the copy operator is not implemented.
     *
     * \return this worker-thread.
     */
    WWorkerThread& operator = ( WWorkerThread const& );

    //! the functor called in threadMain()
    boost::shared_ptr< Function_T > m_func;

    //! a thread id between 0 and m_numThreads - 1
    std::size_t m_id;

    //! the number of threads
    std::size_t m_numThreads;

    //! the exception signal
    ExceptionSignal m_exceptionSignal;

    //! the stop signal
    StopSignal m_stopSignal;
};

template< class Function_T >
WWorkerThread< Function_T >::WWorkerThread( boost::shared_ptr< Function_T > func, std::size_t id, std::size_t numThreads )
    : m_func( func ),
      m_id( id ),
      m_numThreads( numThreads ),
      m_exceptionSignal(),
      m_stopSignal()
{
    if( id >= numThreads )
    {
        throw WException( std::string( "The id of this thread is not valid." ) );
    }
    if( !m_func )
    {
        throw WException( std::string( "No thread function provided!" ) );
    }
}

template< class Function_T >
WWorkerThread< Function_T >::~WWorkerThread()
{
    m_exceptionSignal.disconnect_all_slots();
    m_stopSignal.disconnect_all_slots();
}

template< class Function_T >
void WWorkerThread< Function_T >::subscribeExceptionSignal( ExceptionFunction func )
{
    if( func )
    {
        m_exceptionSignal.connect( func );
    }
}

template< class Function_T >
void WWorkerThread< Function_T >::subscribeStopSignal( StopFunction func )
{
    if( func )
    {
        m_stopSignal.connect( func );
    }
}

template< class Function_T >
void WWorkerThread< Function_T >::threadMain()
{
    if( m_func )
    {
        try
        {
            m_func->operator() ( m_id, m_numThreads, m_shutdownFlag );
        }
        catch( WException const& e )
        {
            m_exceptionSignal( e );
            return;
        }
        catch( std::exception const& e )
        {
            WException w( std::string( e.what() ) );
            m_exceptionSignal( w );
            return;
        }
        catch( ... )
        {
            WException w( std::string( "An exception was thrown." ) );
            m_exceptionSignal( w );
            return;
        }
    }
    m_stopSignal();
}

#endif  // WWORKERTHREAD_H
