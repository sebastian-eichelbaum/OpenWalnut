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

#include "../WFiber.h"
#include "../WPosition.h"
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
        using wmath::WPosition;
        wmath::WFiber fib1;
        wmath::WFiber fib2;
        fib1.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        fib1.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        fib2.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        fib2.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        TS_ASSERT_EQUALS( fib1, fib2 );
    }
};

#endif  // WFIBER_TEST_H
