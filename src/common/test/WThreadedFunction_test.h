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

#ifndef WTHREADEDFUNCTION_TEST_H
#define WTHREADEDFUNCTION_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WThreadedFunction.h"
#include "../WSharedObject.h"

/**
 * Tests the WThreadedFunction class.
 */
class WThreadedFunctionTest : public CxxTest::TestSuite
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
                m_result.getWriteTicket()->get() = -1;
            }
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
            throw WException( "Test!" );
        }
    };

public:
    /**
     * Test function computation with multiple threads.
     */
    void testMultipleThreads()
    {
        boost::shared_ptr< FuncType > func( new FuncType( 5 ) );
        // test 1 thread
        {
            WThreadedFunction< FuncType > f( 1, func );

            TS_ASSERT( !f.done() );
            f.run();
            TS_ASSERT( !f.done() );
            f.wait();
            TS_ASSERT( f.done() );

            TS_ASSERT_EQUALS( func->getResult(), 15 );
            func->reset();

            f.run();
            f.wait();

            TS_ASSERT_EQUALS( func->getResult(), 15 );

            f.run();
            f.wait();

            TS_ASSERT_EQUALS( func->getResult(), 30 );
            func->reset();
        }
        // test 2 threads
        {
            WThreadedFunction< FuncType > f( 2, func );

            TS_ASSERT( !f.done() );
            f.run();
            TS_ASSERT( !f.done() );
            f.wait();
            TS_ASSERT( f.done() );

            TS_ASSERT_EQUALS( func->getResult(), 30 );
            func->reset();
        }
        // test 5 threads
        {
            WThreadedFunction< FuncType > f( 5, func );

            TS_ASSERT( !f.done() );
            f.run();
            TS_ASSERT( !f.done() );
            f.wait();
            TS_ASSERT( f.done() );

            TS_ASSERT_EQUALS( func->getResult(), 75 );
            func->reset();
        }
    }

    /**
     * Test stopping threads.
     */
    void testStopThreads()
    {
        boost::shared_ptr< FuncType > func( new FuncType( 100 ) );
        WThreadedFunction< FuncType > f( 6, func );

        TS_ASSERT( !f.done() );
        f.run();
        TS_ASSERT( !f.done() );
        f.stop();
        TS_ASSERT( !f.done() );
        f.wait();
        TS_ASSERT( f.done() );

        TS_ASSERT_EQUALS( func->getResult(), -1 );
        func->reset();
    }

    /**
     * Test stop condition.
     */
    void testStopCondition()
    {
        boost::shared_ptr< FuncType > func( new FuncType( 5 ) );
        WThreadedFunction< FuncType > f( 6, func );

        TS_ASSERT( !f.done() );
        f.run();
        TS_ASSERT( !f.done() );
        f.getThreadsDoneCondition()->wait();
        TS_ASSERT( f.done() );

        TS_ASSERT_EQUALS( func->getResult(), 90 );
        func->reset();
    }

    /**
     * Test exception handling.
     */
    void testExceptionHandling()
    {
        boost::shared_ptr< ExceptionalFuncType > func( new ExceptionalFuncType );
        WThreadedFunction< ExceptionalFuncType > f( 7, func );
        f.subscribeExceptionSignal( boost::bind( &WThreadedFunctionTest::handleException, this, _1 ) );

        m_exceptionCounter.getWriteTicket()->get() = 0;

        f.run();
        f.wait();

        TS_ASSERT( f.done() );
        TS_ASSERT_EQUALS( m_exceptionCounter.getReadTicket()->get(), 7 );
    }

private:
    /**
     * Exception callback.
     */
    void handleException( WException const& )
    {
        ++m_exceptionCounter.getWriteTicket()->get();
    }

    //! a counter
    WSharedObject< int > m_exceptionCounter;
};

#endif  // WTHREADEDFUNCTION_TEST_H