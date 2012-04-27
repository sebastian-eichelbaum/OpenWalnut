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

#ifndef WSTRUCTUREDTEXTPARSER_TEST_H
#define WSTRUCTUREDTEXTPARSER_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../exceptions/WParseError.h"
#include "../exceptions/WFileNotFound.h"
#include "../exceptions/WNotFound.h"
#include "../exceptions/WTypeMismatch.h"
#include "../WStructuredTextParser.h"

/**
 * Test parsing and query functionality.
 */
class WStructuredTextParserTest: public CxxTest::TestSuite
{
public:
    /**
     * Test whether the parser loads the file and handles invalid files
     */
    void testParseFromFile()
    {
        using WStructuredTextParser::parseFromFile;

        // try to parse the fixture file
        TS_ASSERT_THROWS_NOTHING( parseFromFile(
            boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test.txt" )
        ) );

        TS_ASSERT_THROWS( parseFromFile(
            boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test_invalid.txt" )
        ), WParseError );

        TS_ASSERT_THROWS( parseFromFile(
            boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test_doesnotexist.txt" )
        ), WFileNotFound );

        // NOTE: we do not test parseFromString as both use the same backend functionality
    }

    /**
     * This method test the basic empty and count features of \ref WStructuredTextParser::StructuredValueTree.
     */
    void testEmptyAndCount()
    {
        using WStructuredTextParser::StructuredValueTree;

        // load some data. Please see core/common/test/fixtures/WStructuredTextParser_test.txt for details
        StructuredValueTree t( boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test.txt" ) );

        // check StructuredValueTree::exists
        TS_ASSERT( t.exists( "level0/level1/somekv" ) );
        // check also for existence of a object:
        TS_ASSERT( t.exists( "level0/level1/level2" ) );
        // not exists
        TS_ASSERT( !t.exists( "level0/level1/levelNotExists" ) );
        // needs to support check of not unique object names
        TS_ASSERT( t.exists( "level0/notuniquelevel1" ) );
        // check existence of key-value pair ONLY
        TS_ASSERT( t.exists( "level0/level1/somekv", true ) );
        TS_ASSERT( t.exists( "level0/level1", true ) ); // NOTE: level1 is an object AND kv pair
        TS_ASSERT( !t.exists( "level0/notuniquelevel1", true ) );
        TS_ASSERT( t.exists( "level0/notuniquelevel1/unique", true ) );

        // exists the other level0 object?
        TS_ASSERT( t.exists( "anotherlevel0" ) );
        TS_ASSERT( t.exists( "anotherlevel0/avalue" ) );

        // exists the file level kv pair?
        TS_ASSERT( t.exists( "fileLevelValue", true ) );
        TS_ASSERT( t.exists( "filelevelvalue", true ) );

        // object names with spaces
        TS_ASSERT( t.exists( "Name With Spaces" ) );
        TS_ASSERT( t.exists( "Name With Spaces/akey", true ) );

        // check StructuredValueTree::count
        TS_ASSERT( t.count( "level0/level1/somekv" ) == 1 );
        // check also for existence of a object:
        TS_ASSERT( t.count( "level0/level1/level2" ) == 1 );
        TS_ASSERT( t.count( "level0/level1/level2", true ) == 0 );

        // not exists
        TS_ASSERT( t.count( "level0/level1/levelNotExists" ) == 0 );
        // needs to support check of not unique object names
        TS_ASSERT( t.count( "level0/notuniquelevel1" ) == 2 );
        // check existence of key-value pair ONLY
        TS_ASSERT( t.count( "level0/level1", true ) == 1 );
        TS_ASSERT( t.count( "level0/level1" ) == 2 ); // NOTE: level1 is an object AND kv pair
        TS_ASSERT( t.count( "level0/notuniquelevel1" ) == 2 );
        TS_ASSERT( t.count( "level0/notuniquelevel1", true ) == 0 );

        // to ensure case sensitivity:
        TS_ASSERT( t.count( "filelevelvalue", true ) == 1 );
        TS_ASSERT( t.count( "fileLevelValue", true ) == 1 );

        // object names with spaces
        TS_ASSERT( t.count( "Name With Spaces" ) == 1 );
        TS_ASSERT( t.count( "Name With Spaces/akey", true ) == 1 );
    }

    /**
     * This method tests the basic query features of \ref WStructuredTextParser::StructuredValueTree.
     */
    void testQuery()
    {
        using WStructuredTextParser::StructuredValueTree;

        // load some data. Please see core/common/test/fixtures/WStructuredTextParser_test.txt for details
        StructuredValueTree t( boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test.txt" ) );

        // query only values here.

        // this MUST return the first found value
        TS_ASSERT( t.getValue< std::string >( "level0/notuniquekv", "default" ) == "hello" );
        TS_ASSERT( t.getValue< std::string >( "level0/notuniquelevel1/somekv", "default" ) == "abc" );

        // even if the object name is not unique, it needs to find the unique key value pair
        TS_ASSERT( t.getValue< std::string >( "level0/notuniquelevel1/unique", "default" ) == "yes" );

        // return default if no valid key was found
        TS_ASSERT( t.getValue< std::string >( "level0/notexists", "default" ) == "default" );

        // check if we can find not unique names which represent a class and a kv pair
        TS_ASSERT( t.getValue< std::string >( "level0/level1", "default" ) == "something" );

        // check if we can find the other level0 object
        TS_ASSERT( t.getValue< std::string >( "anotherlevel0/avalue", "default" ) == "hey" );

        // now check getValues which should return a list of matching values
        std::vector< std::string > defs;

        TS_ASSERT( t.getValues< std::string >( "level0/notuniquelevel1/somekv", defs ).size() == 2 );
        TS_ASSERT( ( *t.getValues< std::string >( "level0/notuniquelevel1/somekv", defs ).begin() ) == "abc" );
        TS_ASSERT( ( *( t.getValues< std::string >( "level0/notuniquelevel1/somekv", defs ).begin() + 1 ) ) == "def" );

        // check the return of a default
        TS_ASSERT( t.getValues< std::string >( "level0/notexists", defs ).size() == 0 );
        // and the empty default
        TS_ASSERT( t.getValues< std::string >( "level0/notexists" ).size() == 0 );

        // check operator[] (it uses getValue internally. So we only check for the WNotFound exception)
        TS_ASSERT_THROWS( t.operator[]< std::string >( "level0/notexists" ), WNotFound );

        // check type conversion
        // this is valid for getValue, getValues and [] as they utilize the same function
        TS_ASSERT_THROWS( t.operator[]< size_t >( "level0/notuniquekv" ), WTypeMismatch );
        TS_ASSERT( t.operator[]< size_t >( "level0/level1/somekv" ) == 123 );

        // to ensure case sensitivity:
        TS_ASSERT( t.getValues< std::string >( "filelevelvalue", defs ).size() == 1 );
        TS_ASSERT( t.getValues< std::string >( "fileLevelValue", defs ).size() == 1 );

        // access to names with spaces
        TS_ASSERT( t.getValue< std::string >( "Name With Spaces/akey", "nooo" ) == "value" );
    }

    /**
     * Test the getSubTree functionality
     */
    void testSubTreeQuery()
    {
        using WStructuredTextParser::StructuredValueTree;

        // load some data. Please see core/common/test/fixtures/WStructuredTextParser_test.txt for details
        StructuredValueTree t( boost::filesystem::path( W_FIXTURE_PATH + "WStructuredTextParser_test.txt" ) );

        // get this object ("level0")
        StructuredValueTree level0;
        TS_ASSERT_THROWS_NOTHING( level0 = *( t.getSubTrees( "level0" ).begin() ) );
        TS_ASSERT( level0.count( "uniquekv", true ) == 1 );

        std::vector< StructuredValueTree > v = level0.getSubTrees( "notuniquelevel1" );
        TS_ASSERT( v.size() == 2 );
    }
};

#endif  // WSTRUCTUREDTEXTPARSER_TEST_H

