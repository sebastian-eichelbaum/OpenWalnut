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

#ifndef WPROGRESS_TEST_H
#define WPROGRESS_TEST_H

#include <iostream>

#include <cxxtest/TestSuite.h>

#include "../WProgress.h"

/**
 * Test Class for the base progress class.
 */
class WProgressTest : public CxxTest::TestSuite
{
public:
    /**
     * Test whether WProgress is instantiatable.
     */
    void testInstantiation()
    {
        TS_ASSERT_THROWS_NOTHING( WProgress p( "Test", 1 ) );
    }

    /**
     * Test whether isDetermined returns the right value, depending on construction parameters of WProgress.
     */
    void testDeterminedFlag()
    {
        WProgress p1( "Test1", 0 );
        WProgress p2( "Test2", 1 );

        TS_ASSERT( !p1.isDetermined() );
        TS_ASSERT(  p2.isDetermined() );
    }

    /**
     * Test whether finish() sets pending to false.
     */
    void testFinish()
    {
        // this instance should be pending
        WProgress p1( "Test1", 1 );
        TS_ASSERT( p1.isPending() );

        // finishing it should set isPending to false
        p1.finish();
        TS_ASSERT( !p1.isPending() );
    }

    /**
     * Test whether the state is updated properly.
     */
    void testInternalState()
    {
        WProgress p( "Test", 11 );

        // update
        TS_ASSERT_THROWS_NOTHING( p.update() );

        // get progress
        TS_ASSERT( p.getProgress() == 0.0 );

        // increment it a bit
        ++++++++++p;
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( p.m_count == 5 );
        TS_ASSERT( p.getProgress() == 50.0 );
        ++++++++++p;
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( p.m_count == 10 );
        TS_ASSERT( p.getProgress() == 100.0 );

        // does another step increase the count! It shouldn't
        ++p;
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( p.m_count == 10 );
        TS_ASSERT( p.getProgress() == 100.0 );

        // reaching the max counter should not finish the progress.
        // update
        TS_ASSERT( p.isPending() );
    }

    /**
     * Test whether the state is updated properly if the instance is a indetermined one.
     */
    void testInternalStateOfIndetermined()
    {
        WProgress p( "Test", 0 );

        // update
        TS_ASSERT_THROWS_NOTHING( p.update() );

        // get progress
        TS_ASSERT( p.getProgress() == 0.0 );
        // increment it a bit
        ++++++++++p;
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( p.m_count == 0 );
        TS_ASSERT( p.getProgress() == 0.0 );
    }
};

#endif  // WPROGRESS_TEST_H

