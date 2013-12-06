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

#ifndef WCONDITIONSET_TEST_H
#define WCONDITIONSET_TEST_H

#include <iostream>

#include <boost/thread.hpp>
#include <cxxtest/TestSuite.h>

#include "../WConditionSet.h"
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
 * Test WConditionSet
 */
class WConditionSetTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception, as well as tear down.
     */
    void testInstantiation( void )
    {
        WConditionSet* c = NULL;

        TS_ASSERT_THROWS_NOTHING( c = new WConditionSet() );
        TS_ASSERT_THROWS_NOTHING( delete c );
    }

    /**
     * Tests add and remove methods. Also check double remove/add.
     */
    void testAddRemove( void )
    {
        WConditionSet* cs = new WConditionSet();

        // create some conditions
        boost::shared_ptr< WCondition > c1( new WCondition() );
        boost::shared_ptr< WCondition > c2( new WCondition() );
        boost::shared_ptr< WCondition > c3( new WCondition() );

        // add them
        TS_ASSERT_THROWS_NOTHING( cs->add( c1 ) );
        // adding it a second time should not cause any exception
        TS_ASSERT_THROWS_NOTHING( cs->add( c1 ) );
        TS_ASSERT_THROWS_NOTHING( cs->add( c2 ) );

        // the size should now be 2
        TS_ASSERT( cs->m_conditionSet.size() == 2 );

        // try to remove a condition which is NOT inside the condition set
        TS_ASSERT_THROWS_NOTHING( cs->remove( c3 ) );
        TS_ASSERT( cs->m_conditionSet.size() == 2 );

        // remove a condition inside the set
        TS_ASSERT_THROWS_NOTHING( cs->remove( c1 ) );
        TS_ASSERT( cs->m_conditionSet.size() == 1 );

        // remove a condition inside the set
        TS_ASSERT_THROWS_NOTHING( cs->remove( c2 ) );
        TS_ASSERT( cs->m_conditionSet.size() == 0 );

        delete cs;
    }

    /**
     * Tests whether the condition set really reacts on fired conditions.
     */
    void testWait( void )
    {
        WConditionSet* cs = new WConditionSet();

        // create some conditions
        boost::shared_ptr< WCondition > c1( new WCondition() );
        boost::shared_ptr< WCondition > c2( new WCondition() );

        // disable resetable feature
        cs->setResetable( false, false );
        cs->add( c1 );
        cs->add( c2 );

        // create a thread which fires a condition in the set for us
        Callable t;
        t.flag = false;
        t.c = c1.get();

        // start a thread
        boost::thread thread = boost::thread( boost::bind( &Callable::threadMain, &t ) );

        // wait for condition
        cs->wait();

        // ugly but this is the only way to test it.
        TS_ASSERT( true );

        // reset it
        cs->reset();
        TS_ASSERT( !cs->m_fired );

        // ensure that a condition which has been removed does NOT fire the condition set
        cs->remove( c2 );
        c2->notify();
        TS_ASSERT( !cs->m_fired );

        // the other one should fire the set
        c1->notify();
        TS_ASSERT( cs->m_fired );

        delete cs;
    }

    /**
     * Tests the resetable feature.
     */
    void testWaitResetable( void )
    {
        WConditionSet* cs = new WConditionSet();

        // create some conditions
        boost::shared_ptr< WCondition > c1( new WCondition() );
        boost::shared_ptr< WCondition > c2( new WCondition() );

        // disable resetable feature
        cs->setResetable( true, true );
        cs->add( c1 );
        cs->add( c2 );

        // notify a condition
        c2->notify();

        // after the notification, m_fired should be true
        TS_ASSERT( cs->m_fired );

        // wait should return instantly
        cs->wait();     // how to test it? it just freezes the test if it does not work ;-) also some kind of error notification :-)

        // as the condition set has auto-reset enabled: m_fired should be false again
        TS_ASSERT( !cs->m_fired );

        // if resetable without auto-reset:
        cs->setResetable( true, false );

        // notify again
        c2->notify();

        // m_fired should be true
        TS_ASSERT( cs->m_fired );

        // wait should return instantly
        cs->wait();     // how to test it? it just freezes the test if it does not work ;-) also some kind of error notification :-)

        // m_fired should stay true
        TS_ASSERT( cs->m_fired );

        delete cs;
    }

    /**
     * Ensures reset() never throws something.
     */
    void testResetOnNotResetable( void )
    {
        WConditionSet* cs = new WConditionSet();

        cs->setResetable( false, false );

        // just ensure it does not throw something
        TS_ASSERT_THROWS_NOTHING( cs->reset() );

        delete cs;
    }
};

#endif  // WCONDITIONSET_TEST_H

