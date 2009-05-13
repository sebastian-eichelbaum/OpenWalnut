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

#ifndef WDATASETSINGLE_TEST_H
#define WDATASETSINGLE_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WDataSetSingle.h"
#include "../WMetaInfo.h"
#include "../WValueSet.h"
#include "../WGrid.h"

class WDataSetSingleTest : public CxxTest::TestSuite
{
public:
    void testInstantiation( void )
    {
        boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGrid );
        boost::shared_ptr< WValueSet > valueSet = boost::shared_ptr< WValueSet >( new WValueSet );
        TS_ASSERT_THROWS_NOTHING( WDataSetSingle dataSetSingle( valueSet, grid, metaInfo ) )
    }
    void testGetValueSet( void )
    {
        boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGrid );
        boost::shared_ptr< WValueSet > valueSet = boost::shared_ptr< WValueSet >( new WValueSet );
        boost::shared_ptr< WValueSet > valueSet2 = boost::shared_ptr< WValueSet >( new WValueSet );
        WDataSetSingle dataSetSingle( valueSet, grid, metaInfo );
        TS_ASSERT_EQUALS( dataSetSingle.getValueSet(), valueSet )
        TS_ASSERT_DIFFERS( dataSetSingle.getValueSet(), valueSet2 )
    }
    void testGetGrid( void )
    {
        boost::shared_ptr< WMetaInfo > metaInfo = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        boost::shared_ptr< WGrid > grid = boost::shared_ptr< WGrid >( new WGrid );
        boost::shared_ptr< WGrid > grid2 = boost::shared_ptr< WGrid >( new WGrid );
        boost::shared_ptr< WValueSet > valueSet = boost::shared_ptr< WValueSet >( new WValueSet );
        WDataSetSingle dataSetSingle( valueSet, grid, metaInfo );
        TS_ASSERT_EQUALS( dataSetSingle.getGrid(), grid )
        TS_ASSERT_DIFFERS( dataSetSingle.getGrid(), grid2 )
    }
};

#endif  // WDATASETSINGLE_TEST_H
