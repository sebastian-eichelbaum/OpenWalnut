//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WOPTIONHANDLER_TEST_H
#define WOPTIONHANDLER_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WOptionHandler.h"

class WOptionHandlerTest : public CxxTest::TestSuite
{
public:
    void testInstanziation( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--thisoptiondoesnotexist" );

        WOptionHandler th( argc, arg );
    }

    void testParsing( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--thisoptiondoesnotexist" );

        bool exceptionCaught = false;
        WOptionHandler th( argc, arg );
        try
        {
            th.parseOptions();
        }
        catch( po::error )
        {
            exceptionCaught = true;
        }

        TS_ASSERT( exceptionCaught );
    }
};

#endif  // WOPTIONHANDLER_TEST_H
