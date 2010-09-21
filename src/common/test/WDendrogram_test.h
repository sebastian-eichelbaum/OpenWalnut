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

#ifndef WDENDROGRAM_TEST_H
#define WDENDROGRAM_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDendrogram.h"

/**
 * TestSuite for the WDendrogram class
 */
class WDendrogramTest : public CxxTest::TestSuite
{
public:
    /**
     * When a new node is created, each member should be equal to zero.
     */
    void testNewNodesHaveAlwaysZerosAssignedInitially( void )
    {
        WDendrogram d( 5 );
        for( int i = 0; i < 5; ++i )
        {
            TS_ASSERT_EQUALS( d.m_tree[i].parentTreeIdx, 0 );
            TS_ASSERT_EQUALS( d.m_tree[i].minTreeIdx, 0 );
            TS_ASSERT_EQUALS( d.m_tree[i].maxTreeIdx, 0 );
            TS_ASSERT_EQUALS( d.m_tree[i].dataIdx, 0 );
            TS_ASSERT_EQUALS( d.m_tree[i].height, 0.0 );
        }
    }
};

#endif  // WDENDROGRAM_TEST_H
