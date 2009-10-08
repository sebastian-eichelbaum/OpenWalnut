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

#ifndef WLOADEREEGASCII_TEST_H
#define WLOADEREEGASCII_TEST_H

#include <ctime>
#include <string>
#include <cxxtest/TestSuite.h>
#include <boost/thread.hpp>

#include "../WLoaderEEGASCII.h"

/**
 * Tests for the loader of ASCII EEG files.
 */
class WLoaderEEGASCIITest : public CxxTest::TestSuite
{
public:
    /**
     * Test whether loading itself does not throw an error.
     */
    void testLoading( void )
    {
        std::string fileName = "../fixtures/eeg_testData.asc";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        boost::shared_ptr< WDataHandler > dataHandler =
            boost::shared_ptr< WDataHandler >( new WDataHandler() );
        TS_ASSERT_EQUALS( dataHandler->getNumberOfSubjects(), 0 );

        WLoaderEEGASCII eegASCIILoader( fileName, dataHandler );
        boost::thread loaderThread( eegASCIILoader );

        std::clock_t startTime;
        startTime = std::clock();
        std::clock_t elapsedTime = 0;

        while( dataHandler->getNumberOfSubjects() == 0 && ( elapsedTime / static_cast< double >( CLOCKS_PER_SEC ) ) < 4 )
        {
            elapsedTime = std::clock() - startTime;
        }

        // TODO(wiebel): we need to change this because loading, in the end,
        // should not always increase the number of subjects.
        TS_ASSERT_EQUALS( dataHandler->getNumberOfSubjects(), 1 );
    }
};

#endif  // WLOADEREEGASCII_TEST_H
