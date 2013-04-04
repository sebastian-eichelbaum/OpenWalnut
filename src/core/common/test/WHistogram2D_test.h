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

#ifndef WHISTOGRAM2D_TEST_H
#define WHISTOGRAM2D_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WHistogram2D.h"
#include "../WLimits.h"
#include "../WLogger.h"

/**
 * Unit tests the WHistogramBasic class.
 */
class WHistogram2DTest : public CxxTest::TestSuite
{
public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Check when nothing was inserted every thing is empty.
     */
    void testInitialization( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 0.1, 10, 10 );
        TS_ASSERT_EQUALS( h.size(), 100 );
    }

    /**
     * Check normal insertion inside the min max boundaries.
     */
    void testInsert( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 1.0, 3, 3 );
        TS_ASSERT_EQUALS( h.size(), 9 );
        for( size_t i = 0; i < 3; ++i )
        {
            for( size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( h( i, j ), 0 );
            }
        }
        h.insert( 0.1, 0.1 );
        h.insert( 0.1, 0.4 );
        h.insert( 0.1, 0.7 );
        h.insert( 0.4, 0.1 );
        h.insert( 0.4, 0.4 );
        h.insert( 0.4, 0.7 );
        h.insert( 0.7, 0.1 );
        h.insert( 0.7, 0.4 );
        h.insert( 0.7, 0.7 );
        for( size_t i = 0; i < 3; ++i )
        {
            for( size_t j = 0; j < 3; ++j )
            {
                TS_ASSERT_EQUALS( h( i, j ), 1 );
            }
        }
    }

    /**
     * If the value is directly on the borderline it counts to the right interval.
     */
    void testInsertOnIntervalBorder( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 1.0, 10, 10 );
        h.insert( 0.0999999, 0.0 );
        TS_ASSERT_EQUALS( h( 0, 0 ), 1 );
        h.insert( 0.1, 0.0 );
        TS_ASSERT_EQUALS( h( 1, 0 ), 1 );
        h.insert( 0.1001, 0.0 );
        TS_ASSERT_EQUALS( h( 1, 0 ), 2 );
        h.insert( 0.39999, 0.39999 );
        TS_ASSERT_EQUALS( h( 3, 3 ), 1 );
    }

    /**
     * If the minimum is inserted the first bin should be incremented.
     */
    void testInsertMin( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 1.0, 2, 2 );
        h.insert( 0.0, 0.0 );
        TS_ASSERT_EQUALS( h( 0, 0 ), 1 );
        TS_ASSERT_EQUALS( h( 1, 0 ), 0 );
        TS_ASSERT_EQUALS( h( 0, 1 ), 0 );
        TS_ASSERT_EQUALS( h( 1, 1 ), 0 );
    }

    /**
     * If the maximum is inserted the right most interval is used.
     */
    void testInsertMax( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 1.0, 2, 2 );
        h.insert( 1.0, 1.0 );
        TS_ASSERT_EQUALS( h( 0, 0 ), 0 );
        TS_ASSERT_EQUALS( h( 1, 0 ), 0 );
        TS_ASSERT_EQUALS( h( 0, 1 ), 0 );
        TS_ASSERT_EQUALS( h( 1, 1 ), 1 );
    }

    /**
     * If above the maximum values are inserted a warning should be printed and nothing should happen.
     */
    void testInsertOutOfBounds( void )
    {
        WHistogram2D h( 0.0, 1.0, 0.0, 1.0, 10, 10 );
        h.insert( 1.0 + wlimits::DBL_EPS, 0.0 );
        h.insert( 0.0 - wlimits::DBL_EPS, 0.0 );
        for( size_t i = 0; i < 10; ++i )
        {
            for( size_t j = 0; j < 10; ++j )
            {
                TS_ASSERT_EQUALS( h( i, j ), 0 );
            }
        }
    }

    /**
     * Also for values near the maxium. You may also see #186 for further details.
     */
    void testInsertAlmostMax( void )
    {
        double max = 10000.000000010001;
        WHistogram2D h( -2147483646, max, -2147483646, max, 2, 2 );
        h.insert( 10000, 10000 );
        h.insert( max - 2.0 * wlimits::FLT_EPS, max - 2.0 * wlimits::FLT_EPS );
        TS_ASSERT_EQUALS( h( 1, 1 ), 2 );
    }
};

#endif  // WHISTOGRAM2D_TEST_H
