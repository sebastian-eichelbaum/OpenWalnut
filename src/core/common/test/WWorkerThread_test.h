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

#ifndef WWORKERTHREAD_TEST_H
#define WWORKERTHREAD_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WWorkerThread.h"
#include "../WSharedObject.h"

/**
 * Tests the WWorkerThread class.
 */
class WWorkerThreadTest : public CxxTest::TestSuite
{
    /**
     * A threaded function.
     */
    class FuncType
    {
    public:
        /**
         * Constructor, initialize some stuff.
         *
         * \param value An int value.
         */
        FuncType( int value ) // NOLINT
            : m_input( new int( value ) ) // NOLINT
        {
            // init stuff here
            m_result.getWriteTicket()->get() = 0;
            m_stopped.getWriteTicket()->get() = false;

            if( value < 0 )
            {
                value = -value;
            }
        }

        /**
         * This is the actual thread function.
         *
         * \param shutdown A flag indicating the thread is supposed to stop.
         */
        void operator() ( std::size_t, std::size_t, WBoolFlag const& shutdown )
        {
            for( int i = 1; i <= *m_input.get() && !shutdown(); ++i )
            {
                m_result.getWriteTicket()->get() += i;
            }
            if( shutdown() )
            {
                m_stopped.getWriteTicket()->get() = true;
            }
            sleep( 1 );
        }

        /**
         * Check if the thread was ordered to stop.
         * \return true, if the thread was ordered to stop
         */
        bool stopped()
        {
            return m_stopped.getReadTicket()->get();
        }

        /**
         * A method to extract the result.
         *
         * \return The result of the threaded computation.
         */
        int getResult()
        {
            return m_result.getReadTicket()->get();
        }

        /**
         * Reset everything.
         */
        void reset()
        {
            m_result.getWriteTicket()->get() = 0;
        }

    private:
        //! the input data
        boost::shared_ptr< int const > m_input;

        //! the result
        WSharedObject< int > m_result;

        //! thread stopped?
        WSharedObject< bool > m_stopped;
    };

    /**
     * A function that throws exceptions.
     */
    class ExceptionalFuncType
    {
    public:
        /**
         * The function.
         */
        void operator() ( std::size_t, std::size_t, WBoolFlag& )
        {
            throw WException( std::string( "Test!" ) );
        }
    };

public:
    /**
     * Test if calculation with a single thread works.
     */
    void testSingleThread( void )
    {
        m_stopped = false;

        boost::shared_ptr< FuncType > func( new FuncType( 6 ) );
        WWorkerThread< FuncType > w( func, 0, 1 );
        w.subscribeStopSignal( boost::bind( &WWorkerThreadTest::stopTestDone, this ) );

        w.run();
        w.wait();

        // the result should be 1 + 2 + .. + 6 = 21
        TS_ASSERT_EQUALS( func->getResult(), 21 );
        TS_ASSERT_EQUALS( m_stopped, true );
    }

    /**
     * Test if the thread gets shutdown correctly.
     */
    void testStopThread()
    {
        m_stopped = false;

        boost::shared_ptr< FuncType > func( new FuncType( 100000000 ) );
        WWorkerThread< FuncType > w( func, 0, 1 );
        w.subscribeStopSignal( boost::bind( &WWorkerThreadTest::stopTestDone, this ) );

        w.run();
        w.requestStop();
        w.wait();

        TS_ASSERT( func->stopped() );
        TS_ASSERT_EQUALS( m_stopped, true );
    }

    /**
     * Test if multiple threads correctly compute the result.
     */
    void testMultipleThreads()
    {
        boost::shared_ptr< FuncType > func( new FuncType( 5 ) );
        WWorkerThread< FuncType > w0( func, 0, 3 );
        WWorkerThread< FuncType > w1( func, 1, 3 );
        WWorkerThread< FuncType > w2( func, 2, 3 );

        w0.run();
        w1.run();
        w2.run();
        w0.wait();
        w1.wait();
        w2.wait();

        TS_ASSERT_EQUALS( func->getResult(), 45 );
    }

// unset the WASSERT_AS_CASSERT flag (just in case), so WAssert throws a WException
#ifdef WASSERT_AS_CASSERT
#define WASSERT_FLAG_CHANGED
#undef WASSERT_AS_CASSERT
#endif
    /**
     * Providing a zero-Pointer as function should cause an exception.
     */
    void testNoFunction()
    {
        boost::shared_ptr< FuncType > func;
        TS_ASSERT_THROWS( WWorkerThread< FuncType > w( func, 0, 1 ), WException );
    }

    /**
     * An invalid thread id should cause an exception.
     */
    void testThreadId()
    {
        boost::shared_ptr< FuncType > func( new FuncType( 5 ) );
        TS_ASSERT_THROWS( WWorkerThread< FuncType > w( func, 1, 0 ), WException );
    }
// restore WASSERT_AS_CASSERT flag
#ifdef WASSERT_FLAG_CHANGED
#define WASSERT_AS_CASSERT
#undef WASSERT_FLAG_CHANGED
#endif

    /**
     * Test if exceptions get handled correctly.
     */
    void testExceptions()
    {
        m_exceptionHandled = false;

        boost::shared_ptr< ExceptionalFuncType > func( new ExceptionalFuncType );
        WWorkerThread< ExceptionalFuncType > w( func, 0, 1 );
        w.subscribeExceptionSignal( boost::bind( &WWorkerThreadTest::handleException, this, _1 ) );

        w.run();
        w.wait();

        TS_ASSERT_EQUALS( m_exceptionHandled, true );
    }

private:
    /**
     * A utility function.
     */
    void stopTestDone()
    {
        m_stopped = true;
    }

    /**
     * Another one.
     *
     * \param e An exception.
     */
    void handleException( WException const& e )
    {
        if( strcmp( e.what(), "Test!" ) == 0 )
        {
            m_exceptionHandled = true;
        }
    }

    //! the thread was stopped?
    bool m_stopped;

    //! the exception was handled?
    bool m_exceptionHandled;
};

#endif  // WWORKERTHREAD_TEST_H
