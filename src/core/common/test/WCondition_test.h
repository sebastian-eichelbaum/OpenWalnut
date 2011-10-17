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

#ifndef WCONDITION_TEST_H
#define WCONDITION_TEST_H

#include <iostream>

#include <boost/thread.hpp>
#include <cxxtest/TestSuite.h>

#include "../WCondition.h"

/**
 * Helper class.
 */
class Callable
{
public:

    /**
     * Flag set to true when thread starts
     */
    bool flag;

    /**
     * The condition to be used for signalling.
     */
    WCondition* c;

    /**
     * Thread main method.
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
 * Test WCondition
 */
class WConditionTest : public CxxTest::TestSuite
{
public:

    /**
     * An instantiation should never throw an exception, as well as tear down.
     */
    void testInstantiation( void )
    {
        WCondition* c = 0;

        TS_ASSERT_THROWS_NOTHING( c = new WCondition() );
        TS_ASSERT_THROWS_NOTHING( delete c );
    }

    /**
     * Test whether notification is working.
     */
    void testWaitNotify()
    {
        WCondition c;
        Callable t;
        t.flag = false;
        t.c = &c;

        // start a thread
        boost::thread thread = boost::thread( boost::bind( &Callable::threadMain, &t ) );

        // wait for condition
        c.wait();

        TS_ASSERT( t.flag );
    }
};

#endif  // WCONDITION_TEST_H

