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

#ifndef WHISTOGRAM_TEST_H
#define WHISTOGRAM_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WHistogram.h"
#include "../WLimits.h"
#include "../WLogger.h"

static WLogger logger;

/**
 * Unit tests the WHistogram class.
 */
class WHistogramTest : public CxxTest::TestSuite
{
public:
    /**
     * Check when nothing was inserted every thing is empty.
     */
    void testInitialization( void )
    {
        WHistogram h( 0.0, 1.0 );
        TS_ASSERT_EQUALS( h.binSize(), 1000 );
        TS_ASSERT_EQUALS( h.valuesSize(), 0 );
    }

    /**
     * Check normal insertion inside the min max boundaries.
     */
    void testInsert( void )
    {
        WHistogram h( 0.0, 1.0 );
        h.insert( 0.7234 );
        TS_ASSERT_EQUALS( h.binSize(), 1000 );
        TS_ASSERT_EQUALS( h.valuesSize(), 1 );
        TS_ASSERT_EQUALS( h[723], 1 );
    }

    /**
     * If the value is directly on the borderline it counts to the right interval.
     */
    void testInsertOnIntervalBorder( void )
    {
        WHistogram h( 0.0, 1.0 );
        h.insert( 0.001 );
        TS_ASSERT_EQUALS( h[1], 1 );
        h.insert( 0.0039999 );
        TS_ASSERT_EQUALS( h[3], 1 );
        h.insert( 0.0070001 );
        TS_ASSERT_EQUALS( h[7], 1 );
    }

    /**
     * If the minimum is inserted the first bin should be incremented.
     */
    void testInsertMin( void )
    {
        WHistogram h( 0.0, 1.0 );
        h.insert( 0.0 );
        TS_ASSERT_EQUALS( h[0], 1 );
        TS_ASSERT_EQUALS( h[1], 0 );
    }

    /**
     * If the maximum is inserted the right most interval is used.
     */
    void testInsertMax( void )
    {
        WHistogram h( 0.0, 1.0 );
        h.insert( 0.0 );
        h.insert( 1.0 );
        TS_ASSERT_EQUALS( h[999], 1 );
        TS_ASSERT_EQUALS( h[0], 1 );
    }

    /**
     * If above the maximum values are inserted a warning should be printed and nothing should happen.
     */
    void testInsertOutOfBounds( void )
    {
        WHistogram h( 0.0, 1.0 );
        h.insert( 1.0 + wlimits::DBL_EPS );
        h.insert( 0.0 - wlimits::DBL_EPS );
        for( size_t i = 0; i < h.binSize(); ++i )
        {
            TS_ASSERT_EQUALS( h[i], 0 );
        }
    }

    /**
     * For each insert this number should increase by one.
     */
    void testOperatorToGetNumberOfElementsInsideTheBin( void )
    {
        WHistogram h( 0.0, 1.0 );
        for( size_t i = 0; i < h.binSize(); ++i )
        {
            TS_ASSERT_EQUALS( h[i], 0 );
        }
        h.insert( 0.0 );
        h.insert( 0.0 );
        TS_ASSERT_EQUALS( h.valuesSize(), 2 );
    }
};

#endif  // WHISTOGRAM_TEST_H
