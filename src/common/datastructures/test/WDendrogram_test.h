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

#ifndef WDENDROGRAM_TEST_H
#define WDENDROGRAM_TEST_H

#include <cxxtest/TestSuite.h>

#include "../../WLogger.h"
#include "../WDendrogram.h"

/**
 * TestSuite for the WDendrogram class
 */
class WDendrogramTest : public CxxTest::TestSuite
{
public:
    /**
     * Check if the dendrogram correctly constructs the txt string.  <dfn>($level, ($childs,...),
     * ($left, $right), $similarity)\n</dfn> But for the leaves we have <dfn>(0,
     * ($tractNum,))\n</dfn>.
     \verbatim
                   .----'----. 0.32
                   |    8    |
         0.4   .---'---.     |
               |   6   |  .--'--. 0.6
      0.8   .--'--.    |  |  7  |
            |  5  |    |  |     |
            |     |    |  |     |
            0     1    2  3     4
     \endverbatim
     */
    void testStringCreation( void )
    {
        WDendrogram d( 5 );
        d.merge( 0, 1, 0.8 ); // 5
        d.merge( 5, 2, 0.4 ); // 6
        d.merge( 3, 4, 0.60 ); // 7
        d.merge( 6, 7, 0.32 ); // 8
        std::stringstream ss;
        ss << "(0, (0,))" << std::endl;
        ss << "(0, (1,))" << std::endl;
        ss << "(0, (2,))" << std::endl;
        ss << "(0, (3,))" << std::endl;
        ss << "(0, (4,))" << std::endl;
        ss << "(1, (0, 1), (0, 1), 0.8)" << std::endl;
        ss << "(2, (2, 0, 1), (2, 5), 0.4)" << std::endl;
        ss << "(1, (3, 4), (3, 4), 0.6)" << std::endl;
        ss << "(3, (2, 0, 1, 3, 4), (6, 7), 0.32)" << std::endl;
        if( ss.str() != d.toString() )
        {
            std::cout << "Expected:" << std::endl << ss.str();
            std::cout << "But got:" << std::endl << d.toString();
            TS_FAIL( "Invalid dendrogram to string generation" );
        }
    }

    /**
     * Sets up the WLogger to properly log some thing...
     */
    void setUp( void )
    {
        WLogger::startup();
    }
};

#endif  // WDENDROGRAM_TEST_H
