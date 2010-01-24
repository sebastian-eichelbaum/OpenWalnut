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

#ifndef WSTRINGUTILS_TEST_H
#define WSTRINGUTILS_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WStringUtils.h"

namespace su = string_utils;

/**
 * Testing some boundary cases and basic behaviour of those helper functions.
 */
class WStringUtilsTest : public CxxTest::TestSuite
{
public:
    /**
     * Every vector with elements which may be passed to an ostream, should
     * be correctly written to that stream.
     */
    void testVectorOutputOperator( void )
    {
        std::stringstream ss;
        std::vector< double > pansen;
        using string_utils::operator<<;
        ss << pansen;
        TS_ASSERT_EQUALS( ss.str(), "[]" );
        ss.str( "" );
        ss.clear();
        pansen = std::vector< double >( 2, 3.1415 );
        pansen[1] = 1.414;
        std::string expected( "[3.1415000000000002e+00, 1.4139999999999999e+00]" );
        ss << pansen;
        TS_ASSERT_EQUALS( ss.str(), expected );
    }

    /**
     * Trimming from the right side means that the left side stays unmodified
     * and each character which is in the given character set (WHITESPACE on
     * default) and occurs on the right side will be removed.
     */
    void testRightTrimming( void )
    {
        std::string str( " abc\t  \r\n  \t   \n\n\n" );
        std::string expected( " abc" );
        std::string actual = su::rTrim( str );
        TS_ASSERT_EQUALS( expected, actual );
        TS_ASSERT_EQUALS( str, " abc\t  \r\n  \t   \n\n\n" );

        // check the boundaries
        TS_ASSERT_EQUALS( su::rTrim( std::string( "" ) ), "" );
        TS_ASSERT_EQUALS( su::rTrim( std::string( " " ) ), "" );
        TS_ASSERT_EQUALS( su::rTrim( std::string( "abc" ) ), "abc" );

        // check different character set
        str = " pansenn";
        TS_ASSERT_EQUALS( su::rTrim( str, "pn" ), " panse" );
    }

    /**
     * Same testing like right side trimming but now from the left side.
     */
    void testLeftTrimming( void )
    {
        std::string str( "\t  \r\n  \t   \n\n\nabc " );
        std::string expected( "abc " );
        std::string actual = su::lTrim( str );
        TS_ASSERT_EQUALS( expected, actual );
        TS_ASSERT_EQUALS( str, "\t  \r\n  \t   \n\n\nabc " );

        // check the boundaries
        TS_ASSERT_EQUALS( su::lTrim( std::string( "" ) ), "" );
        TS_ASSERT_EQUALS( su::lTrim( std::string( " " ) ), "" );
        TS_ASSERT_EQUALS( su::lTrim( std::string( "abc" ) ), "abc" );

        // check different character set
        str = "splendor pansen ";
        TS_ASSERT_EQUALS( su::lTrim( str, "splendor " ), "ansen " );
    }

    // we don't test trim since it just uses rTrim and lTrim... => not breakable


    /**
     * Switching to upper case means that all chars [a-z] will be transformed
     * into [A-Z]. This does explicitly not include umlauts.
     */
    void testCaseTransformations( void )
    {
        std::string str( "loWeR text\t with some ü\n" );
        std::string expected( "LOWER TEXT\t WITH SOME ü\n" );

        TS_ASSERT_EQUALS( su::toUpper( str ), expected );
        TS_ASSERT_EQUALS( str, "loWeR text\t with some ü\n" );
        expected = "lower text\t with some ü\n";
        TS_ASSERT_EQUALS( su::toLower( str ), expected );
        TS_ASSERT_EQUALS( str, "loWeR text\t with some ü\n" );
    }

    /**
     * Tokenizers break of a string or other character sequence into a series
     * of tokens.
     */
    void testTokenizer( void )
    {
        std::string source;
        std::vector< std::string > expected;
        TS_ASSERT_EQUALS( su::tokenize( source ), expected );
        source = "Foo bar-foo \r\n\t blubb ";
        expected.push_back( "Foo" );
        expected.push_back( "bar-foo" );
        expected.push_back( "blubb" );
        TS_ASSERT_EQUALS( su::tokenize( source ), expected );
        TS_ASSERT_EQUALS( source, "Foo bar-foo \r\n\t blubb " );
        expected.clear();
        expected.push_back( "Foo " );
        expected.push_back( "ar-foo \r\n\t " );
        expected.push_back( "lu" );
        expected.push_back( "" );
        expected.push_back( " " );
        TS_ASSERT_EQUALS( su::tokenize( source, "b", false ), expected );
    }
};

#endif  // WSTRINGUTILS_TEST_H
