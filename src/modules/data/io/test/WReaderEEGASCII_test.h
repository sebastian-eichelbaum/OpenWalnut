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

#ifndef WREADEREEGASCII_TEST_H
#define WREADEREEGASCII_TEST_H

#include <ctime>
#include <string>
#include <cxxtest/TestSuite.h>
#include <boost/thread.hpp>

#include "../WReaderEEGASCII.h"

/**
 * Tests for the loader of ASCII EEG files.
 */
class WReaderEEGASCIITest : public CxxTest::TestSuite
{
public:
    /**
     * Test whether loading itself does not throw an error.
     */
    void testLoading( void )
    {
        std::string fileName = W_FIXTURE_PATH + "eeg_testData.asc";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;

        WReaderEEGASCII eegASCIIReader( fileName );
        TS_ASSERT( eegASCIIReader.load() );
    }
};

#endif  // WREADEREEGASCII_TEST_H
