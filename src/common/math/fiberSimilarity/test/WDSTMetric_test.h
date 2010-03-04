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

#ifndef WDSTMETRIC_TEST_H
#define WDSTMETRIC_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WDSTMetric.h"

/**
 * Unit tests the dSt metric from Zhang.
 */
class WDSTMetricTest : public CxxTest::TestSuite
{
public:
    /**
     * dSt(Q,R) chooses just the minimum outcome of either dt(Q,r) or dt(R,Q)
     * and hence it is a symmetric metric.
     */
    void testDSTisSymmetric( void )
    {
        WDSTMetric m( 1.0 );

        // generates two fibers just in 2D
        using wmath::WPosition;
        std::vector< WPosition > lineData1;
        lineData1.push_back( WPosition( 0, 1, 0 ) );
        lineData1.push_back( WPosition( 0, 0, 0 ) );
        std::vector< WPosition > lineData2;
        lineData2.push_back( WPosition( 1, 1, 0 ) );
        lineData2.push_back( WPosition( 2, 2, 0 ) );
        wmath::WFiber q( lineData1 );
        wmath::WFiber r( lineData2 );

        TS_ASSERT_EQUALS( m.dist( q, r ), std::sqrt( 2.0 ) / 2.0 );
        TS_ASSERT_EQUALS( m.dist( r, q ), std::sqrt( 2.0 ) / 2.0 );
    }
};

#endif  // WDSTMETRIC_TEST_H
