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

#ifndef WCONDITIONONESHOT_TEST_H
#define WCONDITIONONESHOT_TEST_H

#include <iostream>

#include <boost/thread.hpp>
#include <cxxtest/TestSuite.h>

#include "../WConditionOneShot.h"

/**
 * Helper class.
 */
class Callable
{
public:

    /**
     * Flag set true when the thread starts.
     */
    bool flag;

    /**
     * The condition to use.
     */
    WConditionOneShot* c;

    /**
     * The thread.
     */
    void threadMain()
    {
        flag = true;

        // let the test's thread reach its "wait" call first
        boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );
        c->notify();
    };
};

/**
 * Test WConditionOneShot
 */
class WConditionOneShotTest : public CxxTest::TestSuite
{
public:

    /**
     * An instantiation should never throw an exception, as well as tear down.
     */
    void testInstantiation( void )
    {
        WConditionOneShot* c = 0;

        TS_ASSERT_THROWS_NOTHING( c = new WConditionOneShot() );
        TS_ASSERT_THROWS_NOTHING( delete c );
    }

    /**
     * Test whether notification is working.
     */
    void testWaitNotify()
    {
        WConditionOneShot c;
        Callable t;
        t.flag = false;
        t.c = &c;

        // start a thread
        boost::thread thread = boost::thread( boost::bind( &Callable::threadMain, &t ) );

        // wait for condition
        c.wait();

        // since it is a one shot condition -> another wait will not cause a freeze
        c.wait();

        TS_ASSERT( t.flag );

        // notifying twice should not cause exceptions (boost::lock_error)
        TS_ASSERT_THROWS_NOTHING( c.notify() );
    }
};

#endif  // WCONDITIONONESHOT_TEST_H

