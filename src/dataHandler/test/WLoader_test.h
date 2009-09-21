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

#ifndef WLOADER_TEST_H
#define WLOADER_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WLoader.h"

/**
 * Just a dummy for testing the base class
 */
class DummyLoader : public WLoader
{
public:
    explicit DummyLoader( std::string fileName )
        : WLoader( fileName )
    {
    }

    virtual void operator()()
    {
    }
};

/**
 * Unit tests the Loader base class.
 */
class WLoaderTest : public CxxTest::TestSuite
{
public:
    /**
     * If the given file name is invalid an exception should be thrown.
     */
    void testExceptionOnInvalidFilename( void )
    {
        TS_ASSERT_THROWS_EQUALS( DummyLoader( "no such file" ),
                                 const WDHIOFailure &e,
                                 e.what(),
                                 std::string( "file 'no such file' doesn't exists." ) );
        TS_ASSERT_THROWS_NOTHING( DummyLoader( "fixtures/scalar_float.nii.gz" ) );
    }
};

#endif  // WLOADER_TEST_H
