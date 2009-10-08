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

#include <vector>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../WFiber.h"
#include "../WPosition.h"

// New value trait for wmath::WFiber
#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
class ValueTraits< wmath::WFiber >
{
private:
    char _s[256];

public:
    explicit ValueTraits( const wmath::WFiber &fib )
    {
        std::stringstream ss;
        ss << fib;
        snprintf( _s, ss.str().size(), "%s", ss.str().c_str() );
    }
    const char *asString() const
    {
        return _s;
    }
};
}
#endif  // CXXTEST_RUNNING

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
        std::vector< WPosition > lineData1;
        lineData1.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        lineData1.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        std::vector< WPosition > lineData2;
        lineData2.push_back( WPosition( 1.2, 3.4, 5.6 ) );
        lineData2.push_back( WPosition( 7.8, 9.0, -1.2 ) );
        using wmath::WFiber;
        WFiber fib1( lineData1 );
        WFiber fib2( lineData2 );
        TS_ASSERT_EQUALS( fib1, fib2 );
    }
};

#endif  // WFIBER_TEST_H
