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

#ifndef WDATASETSINGLE_TEST_H
#define WDATASETSINGLE_TEST_H

#include <stdint.h>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../WDataSetSingle.h"
#include "../WValueSet.h"
#include "../WGrid.h"
#include "../WGridRegular3D.h"
#include "../WDataHandlerEnums.h"
#include "../../common/WLogger.h"

/**
 * Test important functionality of WDataSetSingle class
 */
class WDataSetSingleTest : public CxxTest::TestSuite
{
public:
    boost::shared_ptr< WGrid > gridDummy; //!< Dummy grid used in the tests.
    boost::shared_ptr< WValueSetBase > valueSetDummy; //!< Dummy value set used in the tests.

    /**
     * Constructs unit test environment.
     */
    void setUp( void )
    {
        WLogger::startup();

        // create dummies, since they are needed in almost every test
        gridDummy = boost::shared_ptr< WGrid >( new WGridRegular3D( 1, 1, 1 ) );
        boost::shared_ptr< std::vector< int8_t > > data = boost::shared_ptr< std::vector< int8_t > >( new std::vector< int8_t >( 1, 1 ) );
        valueSetDummy = boost::shared_ptr< WValueSet< int8_t > >( new WValueSet< int8_t >( 0, 1, data, W_DT_INT8 ) );
    }

    /**
     * During instantiation nothing should be thrown.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WDataSetSingle ds( valueSetDummy, gridDummy ) );
    }

    /**
     * Retrieving a WValueSetBase should always give the original pointer.
     */
    void testGetValueSet( void )
    {
        boost::shared_ptr< std::vector< double > > data = boost::shared_ptr< std::vector< double > >( new std::vector< double >( 1, 3.1415 ) );
        boost::shared_ptr< WValueSet< double > > other;
        other = boost::shared_ptr< WValueSet< double > >( new WValueSet< double >( 0, 1, data, W_DT_DOUBLE ) );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getValueSet(), valueSetDummy );
        TS_ASSERT_DIFFERS( dataSetSingle.getValueSet(), other );
    }

    /**
     * Retrieving a WGrid should always give the original pointer.
     */
    void testGetGrid( void )
    {
        boost::shared_ptr< WGrid > other = boost::shared_ptr< WGridRegular3D >( new WGridRegular3D( 1, 1, 1 ) );
        WDataSetSingle dataSetSingle( valueSetDummy, gridDummy );
        TS_ASSERT_EQUALS( dataSetSingle.getGrid(), gridDummy );
        TS_ASSERT_DIFFERS( dataSetSingle.getGrid(), other );
    }
};
#endif  // WDATASETSINGLE_TEST_H
