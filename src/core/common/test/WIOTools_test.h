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

#ifndef WIOTOOLS_TEST_H
#define WIOTOOLS_TEST_H

#include <string>
#include <fstream>

#include <cxxtest/TestSuite.h>

#include "../WIOTools.h"

/**
 * Unit test WIOTools functions.
 */
class WIOToolsTest : public CxxTest::TestSuite
{
public:
    /**
     * When switching byte order, the first and last bytes should be swapped
     * and the bytes inbetween too.
     */
    void testByteOrderSwitching( void )
    {
        uint32_t x = 1;
        TS_ASSERT_EQUALS( x, 1 );
        x = switchByteOrder( x );
        TS_ASSERT_EQUALS( x, 16777216 );
        x = switchByteOrder( x );
        TS_ASSERT_EQUALS( x, 1 );
    }

    /**
     * Since templates should work on multiple types we just use double here.
     */
    void testByteOrderSwitchingOnFloats( void )
    {
        double x = 3.141592653589793238462643383279502884197169399375105820974;
        double original = x;
        x = switchByteOrder( x );
        TS_ASSERT_DIFFERS( x, original );
        x = switchByteOrder( x );
        TS_ASSERT_EQUALS( x, original );
    }

    /**
     * On single bytes we should do nothing.
     */
    void testByteOrderSwitchingOnSingleBytes( void )
    {
        char x = 1;
        TS_ASSERT_EQUALS( switchByteOrder( x ), x );
    }

    /**
     * When switching the byte order of an whole array every element should be
     * switched.
     */
    void testByteOrderSwitchOnArray( void )
    {
        uint32_t x[] = { 1, 16777216 };
        switchByteOrderOfArray( x, 2 );
        TS_ASSERT_EQUALS( x[0], 16777216 );
        TS_ASSERT_EQUALS( x[1], 1 );
    }

    /**
     * Test reading a text file in a string.
     */
    void testReadFileIntoString( void )
    {
        std::string expected = "Hello Pansen!\r\n";
        std::string actual = readFileIntoString( boost::filesystem::path( W_FIXTURE_PATH + "hello.world" ) );
        TS_ASSERT_EQUALS( expected, actual );
    }

    /**
     * Writes a text file, and afterwards checks if its the same, by reading it.
     */
    void testWriteStringIntoFile( void )
    {
        std::string content = "Hello Pansen!\r\n";
        boost::filesystem::path fpath = tempFileName();
        writeStringIntoFile( fpath, content );

        std::ifstream input( fpath.string().c_str() );
        std::string actual;
        actual.assign( ( std::istreambuf_iterator< char >( input ) ), std::istreambuf_iterator< char >() );
        input.close();
        TS_ASSERT_EQUALS( content, actual );
        TS_ASSERT( boost::filesystem::exists( fpath ) );
        boost::filesystem::remove( fpath );
    }
};

#endif  // WIOTOOLS_TEST_H
