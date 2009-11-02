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

#ifndef WLINE_TEST_H
#define WLINE_TEST_H

#include <string>
#include <sstream>
#include <vector>

#include <cxxtest/TestSuite.h>
#include <cxxtest/ValueTraits.h>

#include "../WLine.h"

#ifdef CXXTEST_RUNNING
namespace CxxTest
{
CXXTEST_TEMPLATE_INSTANTIATION
class ValueTraits< wmath::WLine >
{
private:
    std::string _s;

public:
    explicit ValueTraits( const wmath::WLine &line )
    {
        std::stringstream ss;
        ss << "WLine(" << line << ")";
        _s = ss.str();
    }
    const char *asString() const
    {
        return _s.c_str();
    }
};
}
#endif  // CXXTEST_RUNNING

/**
 * Unit tests the WLine class
 */
class WLineTest : public CxxTest::TestSuite
{
public:
    /**
     * When printing a line to stdout it the output should be as follows:
     * [first_pos, ..., last_pos] where the positions also should be in
     * the same format, so in the end we would have:
     * [[0,1,0],...[0,0,0]]
     */
    void testOutputOperator( void )
    {
        using wmath::WPosition;
        std::vector< WPosition > lineData;
        lineData.push_back( WPosition( 1., 1., 3.1415 ) );
        lineData.push_back( WPosition( 0., 0., .44 ) );
        lineData.push_back( WPosition( 1., 1., 1. ) );
        std::string expected( "[[1.0000000000000000e+00, 1.0000000000000000e+00, 3.1415000000000002e+00], "
                               "[0.0000000000000000e+00, 0.0000000000000000e+00, 4.4000000000000000e-01], "
                               "[1.0000000000000000e+00, 1.0000000000000000e+00, 1.0000000000000000e+00]]" );
        std::stringstream ss;
        ss << wmath::WLine( lineData );
        TS_ASSERT_EQUALS( expected, ss.str() );
    }

    /**
     * Two Lines are equal if they have the same points in the same order.
     */
    void testEqualityOperator( void )
    {
        using wmath::WPosition;
        std::vector< WPosition > lineData1;
        lineData1.push_back( WPosition( 1., 1., 3.1415 ) );
        lineData1.push_back( WPosition( 0., 0., .44 ) );
        lineData1.push_back( WPosition( 1., 1., 1. ) );
        std::vector< WPosition > lineData2;
        lineData2.push_back( WPosition( 1., 1., 3.1415 ) );
        lineData2.push_back( WPosition( 0., 0., .44 ) );
        lineData2.push_back( WPosition( 1., 1., 1. ) );
        using wmath::WLine;
        WLine line1( lineData1 );
        WLine line2( lineData2 );
        TS_ASSERT_EQUALS( line1, line2 );
        line2.m_points.back()[1] += 0.0000000001;
        TS_ASSERT_DIFFERS( line1, line2 );
    }

    /**
     * When accessing an item within 0..length-1 a const reference to the 
     * WPosition object should be returned.
     */
    void testAccessOperatorWithinValidBounds( void )
    {
        using wmath::WPosition;
        std::vector< WPosition > lineData;
        lineData.push_back( WPosition( 1., 1., 3.1415 ) );
        lineData.push_back( WPosition( 0., 0., .44 ) );
        lineData.push_back( WPosition( 1., 1., 1. ) );
        wmath::WLine line( lineData );
        WPosition expected( 1., 1., 1. );
        TS_ASSERT_EQUALS( expected, line[2] );
    }
};
#endif  // WLINE_TEST_H
