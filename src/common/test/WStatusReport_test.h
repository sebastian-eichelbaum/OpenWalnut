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

#ifndef WSTATUSREPORT_TEST_H
#define WSTATUSREPORT_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WStatusReport.h"

/**
 * Unit tests our StatusReporter class
 */
class WStatusReportTest : public CxxTest::TestSuite
{
public:
    /**
     * Given a certain percentage of progress and a char, and a number
     * of total chars to display, the StringBar of a WStatusReporter
     * object should be determined.
     */
    void testStringBar( void )
    {
        WStatusReport st( 312 );
        st += 189;
        std::string expected( 30, '#' );
        TS_ASSERT_EQUALS( st.stringBar(), expected );
    }

    /**
     * When incrementing over total and the progress is below 100%, it should
     * be accumulated to 100%. But if the progress is already at 100%
     * an assert should fail!
     */
    void testIncrementOverTotal( void )
    {
        WStatusReport st( 312 );
        st += 311;
        st += 9000;
        TS_ASSERT_EQUALS( st.progress(), 1.0 );
    }
};

#endif  // WSTATUSREPORT_TEST_H
