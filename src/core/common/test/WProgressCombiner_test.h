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

#ifndef WPROGRESSCOMBINER_TEST_H
#define WPROGRESSCOMBINER_TEST_H

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <cxxtest/TestSuite.h>

#include "../WProgress.h"
#include "../WProgressCombiner.h"

/**
 * Class testing the functionality of progress combiners.
 */
class WProgressCombinerTest : public CxxTest::TestSuite
{
public:

   /**
    * Test whether WProgress is instantiatable.
    */
    void testInstantiation()
    {
        TS_ASSERT_THROWS_NOTHING( WProgressCombiner p( "Test" ) );
    }

   /**
    * Test whether the combiner ignores manual increments.
    */
    void testInternalStateIgnoresIncrementAndFinish()
    {
        WProgressCombiner p( "Test" );

        // try increment
        ++++++p;
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( p.getProgress() == 0.0 );

        // should ignore finish()
        p.finish();
        TS_ASSERT_THROWS_NOTHING( p.update() );
        TS_ASSERT( !p.isPending() );
    }

    /**
     * Test the combiner when some childs got added to it.
     */
    void testWithChilds()
    {
        WProgressCombiner p( "Test" );

        // create some children
        boost::shared_ptr< WProgress> p1 = boost::shared_ptr< WProgress>( new WProgress( "TestP1", 11 ) );
        boost::shared_ptr< WProgress> p2 = boost::shared_ptr< WProgress>( new WProgress( "TestP2", 11 ) );
        boost::shared_ptr< WProgress> p3 = boost::shared_ptr< WProgress>( new WProgress( "TestP3" ) );

        // as the first and only child is determined (has a known end point) -> the combiner is determined
        TS_ASSERT_THROWS_NOTHING( p.addSubProgress( p1 ) );
        p.update();
        TS_ASSERT( p.isDetermined() );

        // increment a bit
        ++++++++++( *p1 );
        p.update(); // updating is needed in every case, as this is used to propagate changes.
        // p1 is now at 50% -> the combiner should also be at 50%
        TS_ASSERT( p1->getProgress() == 50.0 );
        TS_ASSERT( p.getProgress() == 50.0 );

        // add another determined progress
        TS_ASSERT_THROWS_NOTHING( p.addSubProgress( p2 ) );
        p.update();
        TS_ASSERT( p.isDetermined() );
        TS_ASSERT( p.getProgress() == 25.0 );           // as p2 is at 0% currently

        // now add an indetermined progress
        TS_ASSERT_THROWS_NOTHING( p.addSubProgress( p3 ) );
        p.update();
        TS_ASSERT( !p3->isDetermined() );
        TS_ASSERT( !p.isDetermined() );

        // now finish the progress and test whether to combiner reacts on it.

        // when finishing the indetermined progress the combiner is determined again.
        p3->finish();
        p.update();
        TS_ASSERT( p.isDetermined() );
        TS_ASSERT( p.isPending() );

        // finish the other progress
        p1->finish();
        p2->finish();
        p.update();
        TS_ASSERT( !p.isPending() );

        // finish the combiner
        p.finish();
        TS_ASSERT( p.m_children.empty() );
    }
};

#endif  // WPROGRESSCOMBINER_TEST_H

