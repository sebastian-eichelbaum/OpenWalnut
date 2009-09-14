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

#ifndef WDATAHANDLER_TEST_H
#define WDATAHANDLER_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDataHandler.h"
#include "../WSubject.h"
#include "../exceptions/WNoSuchDataSetException.h"

class WDataHandlerTest : public CxxTest::TestSuite
{
public:
    boost::shared_ptr< WSubject > dummySubject;

    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        // create a subject dummy, since this is needed in almost every test
        dummySubject = boost::shared_ptr< WSubject >( new WSubject() );
    }

    /**
     * An instantiation should never ever throw anything.
     * ToDo(math): What about insufficient memory? Is it then still true?
     */
    void testInstatiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WDataHandler dh() );
    }

    /**
     * When a DataSet is added the size of our container increments by one.
     */
    void testAddDataSet( void )
    {
        WDataHandler dataHandler;
        TS_ASSERT_EQUALS( dataHandler.m_subjects.size(), 0 );
        TS_ASSERT_THROWS_NOTHING( dataHandler.addSubject( dummySubject ) );
        TS_ASSERT_EQUALS( dataHandler.m_subjects.size(), 1 );
    }

    /**
     * Checks if the size (meaning the number of elements) of our container
     * works properly.
     */
    void testGetNumberOfSubjects( void )
    {
        WDataHandler dataHandler;
        TS_ASSERT_EQUALS( dataHandler.getNumberOfSubjects(), 0 );
        dataHandler.addSubject( dummySubject );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfSubjects(), 1 );
        dataHandler.addSubject( dummySubject );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfSubjects(), 2 );
    }

    /**
     * When retrieving a subject only valid indices are allowed.
     */
    void testGetSubjects( void )
    {
        WDataHandler dataHandler;
        boost::shared_ptr< WSubject > otherSubject =
            boost::shared_ptr< WSubject >( new WSubject );
        dataHandler.addSubject( dummySubject );
        dataHandler.addSubject( otherSubject );
        TS_ASSERT_THROWS_NOTHING( dataHandler.getSubject( 0 ) );
        TS_ASSERT_THROWS_NOTHING( dataHandler.getSubject( 1 ) );
        TS_ASSERT_EQUALS( dataHandler.getSubject( 0 ), dummySubject );
        TS_ASSERT_EQUALS( dataHandler.getSubject( 1 ), otherSubject );
        TS_ASSERT_DIFFERS( dataHandler.getSubject( 1 ), dummySubject );
        TS_ASSERT_THROWS( dataHandler.getSubject( 2 ), WNoSuchDataSetException );
    }

    /**
     * Test prevention of modification of a retrieved subject.
     */
    void testConstnessOnSubject( void )
    {
        WDataHandler dh;
        dh.addSubject( dummySubject );
        boost::shared_ptr< const WSubject > dataSet = dh.getSubject( 0 );
        // ToDo(math): I need to try to modify subject in order to test
    }
};

#endif  // WDATAHANDLER_TEST_H
