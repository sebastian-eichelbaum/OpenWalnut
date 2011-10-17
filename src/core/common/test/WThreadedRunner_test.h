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

#ifndef WTHREADEDRUNNER_TEST_H
#define WTHREADEDRUNNER_TEST_H

#include <iostream>

#include <cxxtest/TestSuite.h>

#include "../WThreadedRunner.h"

/**
 * Class implementing a simple worker thread, since proper testing of WThreadedRunner itself is not usable.
 */
class WThreadedRunnerImpl: public WThreadedRunner
{
    friend class WThreadedRunnerTest;
protected:
    /**
     * Function that has to be overwritten for execution. It gets executed in a separate thread after run()
     * has been called.
     */
    virtual void threadMain()
    {
        // Since the modules run in a separate thread: such loops are possible
        while( !m_shutdownFlag() )
        {
            // do fancy stuff
            sleep( 1 );
        }
    }
};

/**
 * Tests the WThreadedRunner class.
 */
class WThreadedRunnerTest : public CxxTest::TestSuite
{
public:
    /**
     * Ensure that nothing is thrown when an instance is created.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WThreadedRunnerImpl() );
    }

    /**
     * Ensure that nothing is thrown when going to sleep.
     */
    void testSleep( void )
    {
        WThreadedRunnerImpl t;

        TS_ASSERT_THROWS_NOTHING( t.sleep( 1 ) );
    }

    /**
     * Ensure that nothing is thrown when running thread.
     */
    void testRun( void )
    {
        WThreadedRunnerImpl t;

        TS_ASSERT_THROWS_NOTHING( t.run() );
        TS_ASSERT_THROWS_NOTHING( t.wait( true ) );
    }
};

#endif  // WTHREADEDRUNNER_TEST_H

