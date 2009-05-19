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
    boost::shared_ptr< WGrid > gridDummy;
    boost::shared_ptr< WValueSet > valueSetDummy;
    boost::shared_ptr< WMetaInfo > metaInfoDummy;

    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        // create dummies, since they are needed in almost every test
        metaInfoDummy = boost::shared_ptr< WMetaInfo >( new WMetaInfo );
        gridDummy = boost::shared_ptr< WGrid >( new WGrid );
        valueSetDummy = boost::shared_ptr< WValueSet >( new WValueSet );
    }

    /**
     * During instantiation nothing should be thrown.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WDataSetSingle ds( valueSetDummy, gridDummy, metaInfoDummy ) );
    }

    /**
     * Retrive a WValueSet should always give the original pointer.
     */
    void testGetValueSet( void )
    {
        boost::shared_ptr< WValueSet > other = boost::shared_ptr< WValueSet >( new WValueSet );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy, metaInfoDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getValueSet(), valueSetDummy )
        TS_ASSERT_DIFFERS( dataSetSingle.getValueSet(), other )
    }

    /**
     * Retrive a WGrid should always give the original pointer.
     */
    void testGetGrid( void )
    {
        boost::shared_ptr< WGrid > other = boost::shared_ptr< WGrid >( new WGrid );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy, metaInfoDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getGrid(), gridDummy );
        TS_ASSERT_DIFFERS( dataSetSingle.getGrid(), other )
    }
};

#endif  // WDATASETSINGLE_TEST_H
