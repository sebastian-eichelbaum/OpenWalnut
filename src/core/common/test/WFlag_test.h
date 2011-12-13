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

#include "../WFlag.h"
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
     * A temporary holder for some value.
     */
    bool m_testTemporary;

    /**
     * Helper function which simply sets the value above to true. It is used to test some conditions here.
     */
    void setTemporary()
    {
        m_testTemporary = true;
    }

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

    /**
     * Test whether change condition is fired.
     */
    void testChangeCondition()
    {
        m_testTemporary = false;

        // create a condition
        WConditionOneShot* c = new WConditionOneShot();
        c->subscribeSignal( boost::bind( &WFlagTest::setTemporary, this ) );

        // use own condition here
        WFlag< bool >* flag = new WFlag< bool >( c, false );

        // change value
        flag->set( !flag->get( true ) );

        // condition fired?
        // Remember: the condition calls the above member function when fired
        TS_ASSERT( m_testTemporary );

        // setting with the suppression flag enabled should not fire the condition:
        m_testTemporary = false;
        // change value
        flag->set( !flag->get( true ), true );
        TS_ASSERT( !m_testTemporary );

        // setting without a change of value should also not call the condition
        flag->set( flag->get( true ) );
        TS_ASSERT( !m_testTemporary );
    }

    /**
     * Test whether change flag is set and reset.
     */
    void testChangeFlagAndReset()
    {
        // create a flag
        WFlag< bool >* flag = new WFlag< bool >( new WConditionOneShot(), false );

        // after creation, the change flag always is true
        TS_ASSERT( flag->changed() );

        // getting the value does not change the flag
        bool v = flag->get();
        TS_ASSERT( !v );
        TS_ASSERT( flag->changed() );

        // getting the value with the argument "true" should reset the change flag
        v = flag->get( true );
        TS_ASSERT( !flag->changed() );

        delete flag;
    }

    /**
     * Test whether copy construction/cloning is working.
     */
    void testCopyConstruction()
    {
        // create a flag
        WFlag< bool >* flag = new WFlag< bool >( new WConditionOneShot(), false );

        // clone
        WFlag< bool >* flagClone = new WFlag< bool >( *flag );

        // check that value, flag and so on are the same
        TS_ASSERT( flag->get() == flagClone->get() );
        TS_ASSERT( flag->changed() == flagClone->changed() );

        // the first should not influence the clone
        flag->get( true );
        TS_ASSERT( flag->changed() != flagClone->changed() );
        flagClone->set( !flagClone->get( true ) );
        TS_ASSERT( flag->get() != flagClone->get() );

        // the conditions need to be different
        // This is because the flag is another one and you won't expect to wake up if someone changes a Flag you do not know
        TS_ASSERT( flag->getCondition() != flagClone->getCondition() );
        TS_ASSERT( flag->getValueChangeCondition() != flagClone->getValueChangeCondition() );
    }
};

#endif  // WFLAG_TEST_H


