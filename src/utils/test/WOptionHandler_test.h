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

#ifndef WOPTIONHANDLER_TEST_H
#define WOPTIONHANDLER_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WOptionHandler.h"

/**
 * Basic testing facilities for our OptionHandler.
 */
class WOptionHandlerTest : public CxxTest::TestSuite
{
public:
    /**
     * Even if invalid options are given the instantiation should not fail.
     */
    void testInstantiation( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--thisoptiondoesnotexist" );

        TS_ASSERT_THROWS_NOTHING( WOptionHandler th( argc, arg ) );
    }

    /**
     * If there are incorrect options then:
     *  1. the internal error flag should be set
     *  2. takeActions() should return 1
     */
    void testCorrectSetErrorFlag( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--thisoptiondoesnotexist" );
        WOptionHandler th( argc, arg );

        TS_ASSERT_EQUALS( th.m_errorOccured, true );
        TS_ASSERT_EQUALS( th.takeActions(), 1 );
    }

    /**
     * If there are incorrect options a meaningful error message should be
     * written to std::cerr
     */
    void testParsingUnkownOption( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--thisoptiondoesnotexist" );

        WOptionHandler th( argc, arg );
        TS_ASSERT_THROWS_EQUALS( th.parseOptions(), po::error &e, std::string( e.what() ),
                                 "unknown option thisoptiondoesnotexist" );
    }

    /**
     * Using the help option (for example) should result in:
     * 1. printing help to stdout (which we cannot check here)
     * 2. Return immediately (don't start GUI) with exit code 0
     */
    void testValidHelpOption( void )
    {
        const int argc = 2;
        char* arg[argc];
        arg[1] = const_cast<char *>( "--help" );

        WOptionHandler th( argc, arg );
        TS_ASSERT_EQUALS( th.takeActions(), 0 );
    }

    /**
     * TODO(math): use QTTestLib for checking the QT-start.
     *
     * We cannot check if WMainApplication::runQT starts and return properly since
     * it won't return unless we close the Window. At this point we should
     * introduce QTTestLib to simulate the closing. But this isn't also a good
     * idea since there must not be a display to which the window may connect.
     * (e.g. consider automated testing without an X display)
     */
};

#endif  // WOPTIONHANDLER_TEST_H
