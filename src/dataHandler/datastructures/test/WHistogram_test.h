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

#ifndef WHISTOGRAM_TEST_H
#define WHISTOGRAM_TEST_H

//#include <stdint.h>
#include <vector>

#include <cxxtest/TestSuite.h>

#include "../../WValueSet.h"
//#include "../../WDataHandlerEnums.h"
//#include "../../WValueSetBase.h"

#include "../WHistogram.h"

/**
 * Test WHistogram
 **/
class WHistogramTest : public CxxTest::TestSuite
{
    public:
        /**
         * Test instantiation
         **/
        void testInstantiation( void )
        {
            double a[2] = { 0.0, 3.1415 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );
            TS_ASSERT_THROWS_NOTHING( WHistogram hist( *valueSet ) );

            boost::shared_ptr< WValueSet< double > > vsPtr( valueSet );
            TS_ASSERT_THROWS_NOTHING( WHistogram hist( vsPtr ) );
        }

        /**
         * Test operator[]
         **/
        void testOperator( void )
        {
            double a[4] = { 0.0, 1.0, 1.0, 4.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );
            WHistogram hist( *valueSet );
            TS_ASSERT_EQUALS( hist[0], 1 ); // 0.0
            TS_ASSERT_EQUALS( hist[1], 2 ); // 1.0
            TS_ASSERT_EQUALS( hist[2], 0 ); // 2.0
            TS_ASSERT_EQUALS( hist[3], 0 ); // 3.0
            TS_ASSERT_EQUALS( hist[4], 1 ); // 4.0
        }

        /**
         * Test at()
         **/
        void testAt( void )
        {
            double a[4] = { 0.0, 1.0, 1.0, 4.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );
            WHistogram hist( *valueSet );
            TS_ASSERT_EQUALS( hist.at(0), 1 ); // 0.0
            TS_ASSERT_EQUALS( hist.at(1), 2 ); // 1.0
            TS_ASSERT_EQUALS( hist.at(2), 0 ); // 2.0
            TS_ASSERT_EQUALS( hist.at(3), 0 ); // 3.0
            TS_ASSERT_EQUALS( hist.at(4), 1 ); // 4.0
            TS_ASSERT_THROWS_ANYTHING( hist.at( 5 ) );
        }

        /**
         * Test getMin(), getMax()
         **/
        void testMinMax( void )
        {
            double a[4] = { 0.0, -5.0, 1.0, 2.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );
            WHistogram hist( *valueSet );
            TS_ASSERT_EQUALS( hist.getMin(), -5.0 );
            TS_ASSERT_EQUALS( hist.getMax(), 2.0 );
        }

        /**
         * Test size(), setInterval()
         **/
        void testSizeSetInterval( void )
        {
            double a[4] = { 0.0, 4.0, 1.0, 2.0 };
            const std::vector< double > v( a, a + sizeof( a ) / sizeof( double ) );
            WValueSet< double >* valueSet = new WValueSet< double >( 0, 1, v, W_DT_DOUBLE );
            WHistogram hist( *valueSet );
            TS_ASSERT_EQUALS( hist.size(), 5 ); // 0.0, 1.0, 2.0, 3.0 and 4.0
            TS_ASSERT_EQUALS( hist.setInterval( 2 ), 3 ); // setInterval() returns new size()
            TS_ASSERT_EQUALS( hist.at(0), 2 ); // 0.0, 1.0
            TS_ASSERT_EQUALS( hist.at(1), 1 ); // 2.0, 3.0
            TS_ASSERT_EQUALS( hist.at(2), 1 ); // 4.0
            hist.setInterval( 1 );
            TS_ASSERT_EQUALS( hist.size(), 5 );
        }
};

#endif  // WHISTOGRAM_TEST_H
