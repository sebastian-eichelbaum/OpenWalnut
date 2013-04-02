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

#ifndef WFIBER_TEST_H
#define WFIBER_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../math/linearAlgebra/WPosition.h"
#include "../WFiber.h"
#include "WFiberTraits.h"

/**
 * Unit tests our WFiber class
 */
class WFiberTest : public CxxTest::TestSuite
{
public:
    /**
     * Two fibers are equal if they have equal WPositions in same order
     */
    void testEqualityOperator( void )
    {
        WFiber fib1;
        WFiber fib2;
        fib1.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        fib1.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        fib2.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        fib2.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        TS_ASSERT_EQUALS( fib1, fib2 );
    }

    /**
     * dLt(Q,R) chooses just the maximum out come of either dt(Q,r) or dt(R,Q)
     * and hence it is a symmetric metric.
     */
    void testDLTisSymmetric( void )
    {
        WFiber q;
        q.push_back( WPosition( 0, 1, 0 ) );
        q.push_back( WPosition( 0, 0, 0 ) );
        WFiber r;
        r.push_back( WPosition( 1, 1, 0 ) );
        r.push_back( WPosition( 2, 2, 0 ) );

        TS_ASSERT_EQUALS( WFiber::distDLT( 1.0, q, r ), std::sqrt( 5.0 ) / 2.0 );
        TS_ASSERT_EQUALS( WFiber::distDLT( 1.0, r, q ), std::sqrt( 5.0 ) / 2.0 );
    }

    /**
     * dSt(Q,R) chooses just the minimum outcome of either dt(Q,r) or dt(R,Q)
     * and hence it is a symmetric metric.
     */
    void testDSTisSymmetric( void )
    {
        WFiber q;
        q.push_back( WPosition( 0, 1, 0 ) );
        q.push_back( WPosition( 0, 0, 0 ) );
        WFiber r;
        r.push_back( WPosition( 1, 1, 0 ) );
        r.push_back( WPosition( 2, 2, 0 ) );

        TS_ASSERT_EQUALS( WFiber::distDST( 1.0, q, r ), std::sqrt( 2.0 ) / 2.0 );
        TS_ASSERT_EQUALS( WFiber::distDST( 1.0, r, q ), std::sqrt( 2.0 ) / 2.0 );
    }

    /**
     * The dt(Q,R) measure (mean closest point distance) is not symmetric.
     * distances below a certain threshold will be omitted.
     */
    void testDTMeasure( void )
    {
        WFiber q;
        q.push_back( WPosition( 0, 1, 0 ) );
        q.push_back( WPosition( 0, 0, 0 ) );
        WFiber r;
        r.push_back( WPosition( 1, 1, 0 ) );
        r.push_back( WPosition( 2, 2, 0 ) );

        TS_ASSERT_EQUALS( WFiber::distDST( 1.0, q, r ), std::sqrt( 2.0 ) / 2.0 );
        TS_ASSERT_EQUALS( WFiber::distDLT( 1.0, q, r ), std::sqrt( 5.0 ) / 2.0 );
    }
};

#endif  // WFIBER_TEST_H
