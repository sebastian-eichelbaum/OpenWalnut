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

#ifndef WLOADERMANAGER_TEST_H
#define WLOADERMANAGER_TEST_H

#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>


#include "../WDataHandler.h"
#include "../WLoaderManager.h"
#include "../WDataSet.h"

/**
 * Testsuite for WLoaderManager.
 */
class WLoaderManagerTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the loading routine.
     */
    void testLoad( void )
    {
        std::string fileName = "fixtures/scalar_signed_short.nii.gz";
        std::cout << "Test loading of " << fileName << "." << std::endl;
        boost::shared_ptr< WDataHandler > dataHandler =
            boost::shared_ptr< WDataHandler >( new WDataHandler() );
        TS_ASSERT_EQUALS( dataHandler->getNumberOfSubjects(), 0 );
        WLoaderManager testLoaderManager;
        testLoaderManager.load( fileName, dataHandler );
        sleep( 4 );  // we need this to allow the thread to terminate
        // TODO(wiebel): we need to change this because loading, in the end,
        // should not always increase the number of subjects.
        TS_ASSERT_EQUALS( dataHandler->getNumberOfSubjects(), 1 );
    }
};

#endif  // WLOADERMANAGER_TEST_H
