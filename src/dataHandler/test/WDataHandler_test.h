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
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WDataHandler dh );
    }
    void testAddDataSets( void )
    {
        WDataHandler dataHandler;
        boost::shared_ptr< WMetaInfo > metaInfo =
            boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WDataSet > dataSet =
            boost::shared_ptr< WDataSet >( new WDataSet( metaInfo ) );
        TS_ASSERT_THROWS_NOTHING( dataHandler.addDataSet( dataSet ) )
    }
    void testGetNumberOfDataSets( void )
    {
        WDataHandler dataHandler;
        boost::shared_ptr< WMetaInfo > metaInfo =
            boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WDataSet > dataSet =
            boost::shared_ptr< WDataSet >( new WDataSet( metaInfo ) );
        dataHandler.addDataSet( dataSet );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfDataSets(), 1 )
        dataHandler.addDataSet( dataSet );
        TS_ASSERT_EQUALS( dataHandler.getNumberOfDataSets(), 2 )
    }
    void testGetDataSets( void )
    {
        WDataHandler dataHandler;
        boost::shared_ptr< WMetaInfo > metaInfo0 = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WMetaInfo > metaInfo1 = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        metaInfo0->setName( "Data Set 0" );
        metaInfo1->setName( "Data Set 1" );
        boost::shared_ptr< WDataSet > dataSet0 =
            boost::shared_ptr< WDataSet >( new WDataSet( metaInfo0 ) );
        boost::shared_ptr< WDataSet > dataSet1 =
            boost::shared_ptr< WDataSet >( new WDataSet( metaInfo1 ) );
        dataHandler.addDataSet( dataSet0 );
        dataHandler.addDataSet( dataSet1 );
        TS_ASSERT_THROWS_NOTHING( dataHandler.getDataSet( 0 ) )
        TS_ASSERT_THROWS_NOTHING( dataHandler.getDataSet( 1 ) )
        TS_ASSERT_EQUALS( dataHandler.getDataSet( 0 ), dataSet0 )
        TS_ASSERT_EQUALS( dataHandler.getDataSet( 1 ), dataSet1 )
        TS_ASSERT_DIFFERS( dataHandler.getDataSet( 1 ), dataSet0 )
        TS_ASSERT_THROWS( dataHandler.getDataSet( 2 ), WNoSuchDataSetException )
    }
};

#endif  // WDATAHANDLER_TEST_H
