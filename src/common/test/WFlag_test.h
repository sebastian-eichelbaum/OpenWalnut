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

#ifndef WFLAG_TEST_H
#define WFLAG_TEST_H

#include <iostream>

#include <boost/thread.hpp>
#include <cxxtest/TestSuite.h>

#include "../WFlag.hpp"
#include "../WConditionOneShot.h"

/** 
 * Helper class.
 */
class Callable
{
public:

    /**
     * The flag to be tested
     */
    WFlag<bool>* flag;

    /**
     * True if the thread finishes.
     */
    bool finished;

    /**
     * Constructor. To init the Flag.
     */
    Callable()
    {
        finished = false;
        flag = new WFlag< bool >( new WConditionOneShot(), false );
    }

    /**
     * Thread function.
     */
    void threadMain()
    {
        // just wait
        flag->wait();
        finished = true;
    };
};

/** 
 * Test WFlag
 */
class WFlagTest : public CxxTest::TestSuite
{
public:

    /**
     * An instantiation should never throw an exception, as well as tear down.
     */
    void testInstantiation( void )
    {
        WFlag< bool >* flag = 0;

        TS_ASSERT_THROWS_NOTHING( flag = new WFlag< bool >( new WConditionOneShot(), false ) );
        TS_ASSERT_THROWS_NOTHING( delete flag );
    }

    /** 
     * Test whether notification is working.
     */
    void testWaitNotify()
    {
        Callable t;
        // the flag should be false now
        // NOTE: the syntax to get the value of the flag looks ugly here, but you normally do not use pointers
        TS_ASSERT( !( *t.flag )() );

        // start a thread
        boost::thread thread = boost::thread( boost::bind( &Callable::threadMain, &t ) );

        // set value equal to the one already set
        ( *t.flag )( false );
        // this should NOT notify the thread since the set value is not different to the initial one
        TS_ASSERT( !t.finished );

        // notify
        ( *t.flag )( true );
        thread.join();

        TS_ASSERT( ( *t.flag )() );
    }
};

#endif  // WFLAG_TEST_H


