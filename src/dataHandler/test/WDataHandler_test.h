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
#include "../WDataSet.h"
#include "../WMetaInfo.h"
#include "../exceptions/WNoSuchDataSetException.h"

class WDataHandlerTest : public CxxTest::TestSuite
{
public:
    boost::shared_ptr< WDataSet > dummyDataSet;

    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        // create a DataSet dummy, since this is needed in almost every test
        boost::shared_ptr< WMetaInfo > metaInfo =
            boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        dummyDataSet = boost::shared_ptr< WDataSet >( new WDataSet( metaInfo ) );
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
        TS_ASSERT_EQUALS( dataHandler.m_dataSets.size(), 0 );
        TS_ASSERT_THROWS_NOTHING( dataHandler.addDataSet( dummyDataSet ) );
        TS_ASSERT_EQUALS( dataHandler.m_dataSets.size(), 1 );
    }

    /**
     * Checks if the size (meaning the number of elements) of our container
     * works properly.
     */
    void testGetNumberOfDataSets( void )
    {
        WDataHandler dataHandler;
        TS_ASSERT_EQUALS( dataHandler.getNumberOfDataSets(), 0 );
        dataHandler.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfDataSets(), 1 );
        dataHandler.addDataSet( dummyDataSet );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfDataSets(), 2 );
    }

    /**
     * When retrieving a DataSet only valid indices are allowed.
     */
    void testGetDataSets( void )
    {
        WDataHandler dataHandler;
        boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        metaInfo->setName( "Other Data Set" );
        boost::shared_ptr< WDataSet > otherDataSet =
            boost::shared_ptr< WDataSet >( new WDataSet( metaInfo ) );
        dataHandler.addDataSet( dummyDataSet );
        dataHandler.addDataSet( otherDataSet );
        TS_ASSERT_THROWS_NOTHING( dataHandler.getDataSet( 0 ) );
        TS_ASSERT_THROWS_NOTHING( dataHandler.getDataSet( 1 ) );
        TS_ASSERT_EQUALS( dataHandler.getDataSet( 0 ), dummyDataSet );
        TS_ASSERT_EQUALS( dataHandler.getDataSet( 1 ), otherDataSet );
        TS_ASSERT_DIFFERS( dataHandler.getDataSet( 1 ), dummyDataSet );
        TS_ASSERT_THROWS( dataHandler.getDataSet( 2 ), WNoSuchDataSetException );
    }

    /**
     * Test prevention of modification of a retrieved DataSet.
     */
    void testConstnessOnDataSet( void )
    {
        WDataHandler dh;
        dh.addDataSet( dummyDataSet );
        boost::shared_ptr< const WDataSet > dataSet = dh.getDataSet( 0 );
        // ToDo(math): I need to try to modify dataSet in order to test
    }
};

#endif  // WDATAHANDLER_TEST_H
