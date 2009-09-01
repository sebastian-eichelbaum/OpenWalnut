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

#ifndef WLOADERBIOSIG_TEST_H
#define WLOADERBIOSIG_TEST_H

#include <string>
#include <cxxtest/TestSuite.h>
#include <boost/thread.hpp>

#include "../WLoaderBiosig.h"
#include "../../WDataHandler.h"

/**
 * Tests for the biosignal loader that uses biosig library.
 */
class WLoaderBiosigTest : public CxxTest::TestSuite
{
public:
    /**
     * TODO(wiebel): Document this!
     */
    void testSomething( void )
    {
        //  THIS IS INTENTIONALLY NOT STYLECONFORM AS IT SHOULD NOT  BE COMMITED yet
        //  std::string fileName = "/dargb/bv_data/Medical/MPI-CBS/ASA/A1_alex_Segment_1.edf";
        std::string fileName = "/windows/C/Users/wiebel/Documents/ASA/Export/A1_alex_Segment_1.edf";
        //  std::string fileName = "/home/wiebel/Data/EEG/A1.cnt";
        std::cout << std::endl << "Test loading of " << fileName << "." << std::endl;
        boost::shared_ptr< WDataHandler > dataHandler =
            boost::shared_ptr< WDataHandler >( new WDataHandler() );
        TS_ASSERT_EQUALS( dataHandler->getNumberOfDataSets(), 0 );

        WLoaderBiosig biosigLoader( fileName, dataHandler );
        boost::thread loaderThread( biosigLoader );
        sleep( 3 );
//        TS_FAIL( "Try to test EDF" );
    }
};

#endif  // WLOADERBIOSIG_TEST_H
