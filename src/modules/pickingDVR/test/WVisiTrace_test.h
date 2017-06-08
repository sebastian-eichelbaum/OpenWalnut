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
#include "../../../core/common/math/test/WPositionTraits.h"

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
     * Ensure instantiation does not throw and does initialization right.
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
        const std::vector< int > sizes = { 4, 2, 5, 8, 3, 6 };
        for( size_t id = 0; id < sizes.size(); ++id )
        {
            myVisiTrace.m_candidatePositions.push_back( std::vector< WPosition >( sizes[id] ) );
        }

        std::vector< std::pair< int, int > > order = myVisiTrace.getLinearizedNodesRefs();

        TS_ASSERT_EQUALS( order.size(), 28 );

        TS_ASSERT_EQUALS( order[0], std::make_pair( 0, 0 ) );
        TS_ASSERT_EQUALS( order[1], std::make_pair( 0, 1 ) );
        TS_ASSERT_EQUALS( order[2], std::make_pair( 0, 2 ) );
        TS_ASSERT_EQUALS( order[3], std::make_pair( 0, 3 ) );

        TS_ASSERT_EQUALS( order[5], std::make_pair( 1, 1 ) );
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
        const std::vector< int > sizes = { 4, 2, 5, 8, 3, 6 };
        for( size_t id = 0; id < sizes.size(); ++id )
        {
            myVisiTrace.m_candidatePositions.push_back( std::vector< WPosition >( sizes[id] ) );
        }

        std::vector< std::vector< int > > inverseRefs = myVisiTrace.getInverseLinearizedNodesRefs();

        TS_ASSERT_EQUALS( inverseRefs.size(), 6 );

        TS_ASSERT_EQUALS( inverseRefs[0][0], 0 );
        TS_ASSERT_EQUALS( inverseRefs[0][1], 1 );
        TS_ASSERT_EQUALS( inverseRefs[0][2], 2 );
        TS_ASSERT_EQUALS( inverseRefs[0][3], 3 );

        TS_ASSERT_EQUALS( inverseRefs[1][0], 4 );
        TS_ASSERT_EQUALS( inverseRefs[1][1], 5 );

        TS_ASSERT_EQUALS( inverseRefs[2][0], 6 );

        TS_ASSERT_EQUALS( inverseRefs[5][5], 27 );
    }

    /**
     * Check throwing of Dijkstra and result according to VisiTrace with constant opacity weights.
     */
    void testPerformDijkstraException()
    {
        WVisiTrace myVisiTrace;

        myVisiTrace.m_candidatePositions.push_back(
            {
                WPosition( 1, 0, 0 ),
                WPosition( 1, 0, 1 ),
                WPosition( 1, 0, 2 ),
                WPosition( 1, 0, 3 )
            }
            );
        myVisiTrace.m_candidateJumps.push_back( { 0.5, 0.5, 0.5, 0.5 } ); // NOLINT

        myVisiTrace.m_candidatePositions.push_back(
            {
                WPosition( 1, 1, -3.5 ),
                WPosition( 1, 1, -2.5 )
            }
            );
        myVisiTrace.m_candidateJumps.push_back( { 0.5, 0.5 } ); // NOLINT

        myVisiTrace.m_candidatePositions.push_back(
            {
                WPosition( 1, 1.2, 0 ),
                WPosition( 1, 1.2, 1 ),
                WPosition( 1, 1.2, 2 ),
                WPosition( 1, 1.2, 3 )
            }
            );
        myVisiTrace.m_candidateJumps.push_back( { 0.5, 0.5, 0.5, 0.5 } ); // NOLINT

        TS_ASSERT_EQUALS( myVisiTrace.m_candidatePositions.size(), 3 );
        TS_ASSERT_THROWS_NOTHING( myVisiTrace.performDijkstra() );
        TS_ASSERT_EQUALS( myVisiTrace.m_curve3D.size(), 3 );
        TS_ASSERT_EQUALS( myVisiTrace.m_curve3D[0], WPosition( 1, 0, 0 ) );
        TS_ASSERT_EQUALS( myVisiTrace.m_curve3D[1], WPosition( 1, 1, -2.5 ) );
        TS_ASSERT_EQUALS( myVisiTrace.m_curve3D[2], WPosition( 1, 1.2, 0 ) );
    }


    /**
     * Does addCandidatesForRay() actually add positions
     */
    void testAddingPositions()
    {
        WVisiTrace visiTraceAdding;


        const size_t numCands1 = 11;
        const size_t numCands2 = 12;
        const size_t numCands3 = 13;
        {
            std::vector< std::pair< double, WPosition > > cands1;
            const double someDoubleValue1 = 1.1;
            for( size_t id = 0; id < numCands1; ++id )
            {
                cands1.push_back( std::make_pair( someDoubleValue1 * id, WPosition( someDoubleValue1 * id, someDoubleValue1, someDoubleValue1 ) ) );
            }
            visiTraceAdding.addCandidatesForRay( cands1 );
        }
        {
            std::vector< std::pair< double, WPosition > > cands2;
            const double someDoubleValue2 = 1.2;
            for( size_t id = 0; id < numCands2; ++id )
            {
                cands2.push_back( std::make_pair( someDoubleValue2 * id, WPosition( someDoubleValue2 * id, someDoubleValue2, someDoubleValue2 ) ) );
            }
            visiTraceAdding.addCandidatesForRay( cands2 );
        }
        {
            std::vector< std::pair< double, WPosition > > cands3;
            const double someDoubleValue3 = 1.3;
            for( size_t id = 0; id < numCands3; ++id )
            {
                cands3.push_back( std::make_pair( someDoubleValue3 * id, WPosition( someDoubleValue3 * id, someDoubleValue3, someDoubleValue3 ) ) );
            }
            visiTraceAdding.addCandidatesForRay( cands3 );
        }

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions.size(), 3 );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidateJumps.size(), 3 );

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[0].size(), numCands1 );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[1].size(), numCands2 );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[2].size(), numCands3 );

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidateJumps[0].size(), numCands1 );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidateJumps[1].size(), numCands2 );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidateJumps[2].size(), numCands3 );

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[0][0], WPosition( 0, 1.1, 1.1 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[0][1], WPosition( 1.1, 1.1, 1.1 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[0][10], WPosition( 11, 1.1, 1.1 ) );

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[1][0], WPosition( 0, 1.2, 1.2 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[1][1], WPosition( 1.2, 1.2, 1.2 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[1][10], WPosition( 12, 1.2, 1.2 ) );

        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[2][0], WPosition( 0, 1.3, 1.3 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[2][1], WPosition( 1.3, 1.3, 1.3 ) );
        TS_ASSERT_EQUALS( visiTraceAdding.m_candidatePositions[2][10], WPosition( 13, 1.3, 1.3 ) );
    }
};

#endif  // WVISITRACE_TEST_H
