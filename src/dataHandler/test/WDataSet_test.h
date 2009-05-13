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

#ifndef WDATASET_TEST_H
#define WDATASET_TEST_H

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>
#include "../WDataSet.h"
#include "../WMetaInfo.h"

class WDataSetTest : public CxxTest::TestSuite
{
public:
    void testInstantiation( void )
    {
        boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr< WMetaInfo >( new WMetaInfo() );
        TS_ASSERT_THROWS_NOTHING( WDataSet dataSet( metaInfo ) );
    }
    void testGetMetaInfo( void )
    {
        boost::shared_ptr< WMetaInfo > metaInfo  =
            boost::shared_ptr< WMetaInfo >( new WMetaInfo() );
        metaInfo->setName( "testGetMetaInfo Meta Info" );
        WDataSet dataSet( metaInfo );
        TS_ASSERT_EQUALS( dataSet.getMetaInfo(), metaInfo )
    }
};

#endif  // WDATASET_TEST_H
