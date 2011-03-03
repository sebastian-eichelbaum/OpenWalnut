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

#ifndef WDATASETDTI_TEST_H
#define WDATASETDTI_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../common/math/test/WTensorTraits.h"
#include "../../common/WLogger.h"
#include "../WDataSetDTI.h"
#include "../WGridRegular3D.h"

/**
 * Testsuite for unit tests of the WDataSetDTI class.
 */
class WDataSetDTITest : public CxxTest::TestSuite
{
public:
    /**
     * Setup logger and other stuff for each test.
     */
    void setUp()
    {
        WLogger::startup();
    }

    /**
     * Only values sets of order 1, dim 6 should be used to construct DTI datasets.
     */
    void testInstanziation( void )
    {
        float dataArray[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 }; // NOLINT array init list
        boost::shared_ptr< std::vector< float > > data =
            boost::shared_ptr< std::vector< float > >(
                new  std::vector< float >( &dataArray[0], &dataArray[0] + sizeof( dataArray ) / sizeof( float ) ) );
        boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet< float >( 1, 6, data, W_DT_FLOAT ) );
        boost::shared_ptr< WGrid > newGrid( new WGridRegular3D( 1, 1, 1 ) );
        TS_ASSERT_THROWS_NOTHING( WDataSetDTI( newValueSet, newGrid ) );
    }

    /**
     * Accessing the i'th tensor is: getting the WValue at that position and
     * transform it to a WTensorSym< 2, 3 >.
     */
    void testTensorAccess( void )
    {
        float dataArray[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 }; // NOLINT array init list
        boost::shared_ptr< std::vector< float > > data =
            boost::shared_ptr< std::vector< float > >(
                new std::vector< float >( &dataArray[0], &dataArray[0] + sizeof( dataArray ) / sizeof( float ) ) );
        boost::shared_ptr< WValueSetBase > newValueSet( new WValueSet< float >( 1, 6, data, W_DT_FLOAT ) );
        boost::shared_ptr< WGrid > newGrid( new WGridRegular3D( 1, 1, 1 ) );
        WDataSetDTI dataset( newValueSet, newGrid );
        WTensorSym< 2, 3, float > expected;
        expected( 0, 0 ) = 0.0;
        expected( 0, 1 ) = 1.0;
        expected( 0, 2 ) = 2.0;
        expected( 1, 1 ) = 3.0;
        expected( 1, 2 ) = 4.0;
        expected( 2, 2 ) = 5.0;
        TS_ASSERT_EQUALS( dataset.getTensor( 0 ), expected );
    }
};

#endif  // WDATASETDTI_TEST_H
