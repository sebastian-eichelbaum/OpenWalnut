//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2017 OpenWalnut Community and Hochschule Worms
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

#ifndef WVISITRACE_TEST_H
#define WVISITRACE_TEST_H

#include <utility>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WVisiTrace.h"

/**
 * Test for WMWriteMesh
 */
class WVisiTraceTest : public CxxTest::TestSuite
{
public:
    /**
     * Setup method called before every test case.
     */
    void setUp()
    {
    }

    /**
     * Ensure instatiation does not throw and does initialization right.
     */
    void testInstatiation()
    {
        TS_ASSERT_THROWS_NOTHING( WVisiTrace() );
    }

    /**
     * Ensure that getline does not return anything until something is added.
     */
    void testGetLineOfInitial()
    {
        WVisiTrace myVisiTrace;
        std::vector< WPosition > line = myVisiTrace.getLine();
        TS_ASSERT_EQUALS( line.size(), 0 );
    }

    /**
     * Check correct order resulting from getLinearizedNodesRefs
     */
    void testLinearization()
    {
        WVisiTrace myVisiTrace;
        std::vector< std::vector< WPosition > > candidates;
        const std::vector< int > sizes = { 2, 4, 5, 8, 3, 6 };
        for( size_t id = 0; id < sizes.size(); ++id )
        {
            myVisiTrace.m_candidatePositions.push_back( std::vector< WPosition >( sizes[id] ) );
        }

        std::vector< std::pair< int, int > > order = myVisiTrace.getLinearizedNodesRefs();

        TS_ASSERT_EQUALS( order.size(), 28 );

        TS_ASSERT_EQUALS( order[0], std::make_pair( 0, 0 ) );
        TS_ASSERT_EQUALS( order[1], std::make_pair( 0, 1 ) );
        TS_ASSERT_EQUALS( order[2], std::make_pair( 1, 0 ) );
        TS_ASSERT_EQUALS( order[3], std::make_pair( 1, 1 ) );

        TS_ASSERT_EQUALS( order[5], std::make_pair( 1, 3 ) );
        TS_ASSERT_EQUALS( order[6], std::make_pair( 2, 0 ) );

        TS_ASSERT_EQUALS( order[27], std::make_pair( 5, 5 ) );
    }

    /**
     * Check getLinearizedNodesRefs
     */
    void testInverseLinearization()
    {
        WVisiTrace myVisiTrace;
        std::vector< std::vector< WPosition > > candidates;
        const std::vector< int > sizes = { 2, 4, 5, 8, 3, 6 };
        for( size_t id = 0; id < sizes.size(); ++id )
        {
            myVisiTrace.m_candidatePositions.push_back( std::vector< WPosition >( sizes[id] ) );
        }

        std::vector< std::vector< int > > inverseRefs = myVisiTrace.getInverseLinearizedNodesRefs();

        TS_ASSERT_EQUALS( inverseRefs.size(), 6 );

        TS_ASSERT_EQUALS( inverseRefs[0][0], 0 );
        TS_ASSERT_EQUALS( inverseRefs[0][1], 1 );

        TS_ASSERT_EQUALS( inverseRefs[1][0], 2 );
        TS_ASSERT_EQUALS( inverseRefs[1][1], 3 );
        TS_ASSERT_EQUALS( inverseRefs[1][2], 4 );
        TS_ASSERT_EQUALS( inverseRefs[1][3], 5 );

        TS_ASSERT_EQUALS( inverseRefs[2][0], 6 );

        TS_ASSERT_EQUALS( inverseRefs[5][5], 27 );
    }
};

#endif  // WVISITRACE_TEST_H
