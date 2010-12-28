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

#ifndef WLOGENTRY_TEST_H
#define WLOGENTRY_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WLogEntry.h"

/**
 * Unit tests our log messages.
 */
class WLogEntryTest : public CxxTest::TestSuite
{
public:
    /**
     * If given a format string of the form: "%t :: %l :: %m" then
     * the log message will replace %t with time of logging and
     * %l with level of logging and %m with the message itself.
     */
    void testFormatStringReplacement( void )
    {
        std::string dummyTime = "2009-Oct-02 14:46:50";
        WLogEntry entry( dummyTime, "Dummy message", LL_INFO, "WLogEntryTest" );
        // build our customized format string
        std::string format = "%m :: %t %t %l";
        std::string expected = "Dummy message :: 2009-Oct-02 14:46:50 %t INFO   ";
        TS_ASSERT_EQUALS( entry.getLogString( format, false ), expected );
    }

    /**
     * If an empty format string is given, then an empty string should be
     * returned.
     */
    void testEmptyStringAsFormatString( void )
    {
        WLogEntry entry( "now", "msg", LL_INFO, "WLogEntryTest" );
        TS_ASSERT_EQUALS( entry.getLogString( "", false ), "" );
    }

    /**
     * If ever an unknown log level was used to construct the entry then no
     * replacement should be done.
     */
    void testUnkownLogLevel( void )
    {
        WLogEntry entry( "now", "msg", static_cast< LogLevel >( 4711 ), "WLogEntryTest" );
        std::string expected( "%l" );
        std::string format( "%l" );
        TS_ASSERT_EQUALS( entry.getLogString( format, false ), expected );
    }
};

#endif  // WLOGENTRY_TEST_H
