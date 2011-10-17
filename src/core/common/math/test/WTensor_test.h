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

#ifndef WTENSOR_TEST_H
#define WTENSOR_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>
#include "../WTensor.h"

/**
 * Test class for the WTensor template.
 */
class WTensorTest : public CxxTest::TestSuite
{
public:
    /**
     * Test access operator ().
     */
    void testAccessOperator1()
    {
        WTensor< 3, 2 > w;
        w( 0, 0, 0 ) = 2;
        w( 0, 0, 1 ) = 3;
        w( 0, 1, 0 ) = 0;
        w( 0, 1, 1 ) = 5;
        w( 1, 0, 0 ) = 2;
        w( 1, 0, 1 ) = 1;
        w( 1, 1, 0 ) = 8;
        w( 1, 1, 1 ) = 10;

        TS_ASSERT_EQUALS( w( 0, 0, 0 ), 2 );
        TS_ASSERT_EQUALS( w( 0, 0, 1 ), 3 );
        TS_ASSERT_EQUALS( w( 0, 1, 0 ), 0 );
        TS_ASSERT_EQUALS( w( 0, 1, 1 ), 5 );
        TS_ASSERT_EQUALS( w( 1, 0, 0 ), 2 );
        TS_ASSERT_EQUALS( w( 1, 0, 1 ), 1 );
        TS_ASSERT_EQUALS( w( 1, 1, 0 ), 8 );
        TS_ASSERT_EQUALS( w( 1, 1, 1 ), 10 );
    }

    /**
     * Test access operator [].
     */
    void testAccessOperator2()
    {
        std::vector< unsigned int > v( 3, 0 );
        WTensor< 3, 4 > w;

        for( v[ 0 ] = 0; v[ 0 ] < 4; ++v[ 0 ] )
        {
            for( v[ 1 ] = 0; v[ 1 ] < 4; ++v[ 1 ] )
            {
                for( v[ 2 ] = 0; v[ 2 ] < 4; ++v[ 2 ] )
                {
                    w[ v ] = v[ 0 ] + v[ 1 ] + v[ 2 ];
                }
            }
        }

        unsigned int f[] = { 0, 0, 0 };
        for( f[ 0 ] = 0; f[ 0 ] < 4; ++f[ 0 ] )
        {
            for( f[ 1 ] = 0; f[ 1 ] < 4; ++f[ 1 ] )
            {
                for( f[ 2 ] = 0; f[ 2 ] < 4; ++f[ 2 ] )
                {
                    TS_ASSERT_EQUALS( w[ f ], f[ 0 ] + f[ 1 ] + f[ 2 ] );
                }
            }
        }
    }

    /**
     * Test the standard constructor.
     */
    void testStandardConstructor()
    {
        // create lots of tensors
        // these should all compile
        WTensor< 0, 0 > t00d;
        WTensor< 0, 3 > t03d;
        WTensor< 1, 1 > t11d;
        WTensor< 1, 2 > t12d;
        WTensor< 1, 3 > t13d;
        WTensor< 1, 4 > t14d;
        WTensor< 1, 1, float > t11f;
        WTensor< 1, 2, int > t12i;
        WTensor< 1, 3, char > t13c;
        WTensor< 1, 4, std::string > t14s;
        WTensor< 2, 1 > t21d;
        WTensor< 2, 2 > t22d;
        WTensor< 2, 3 > t23d;
        WTensor< 2, 4 > t24d;
        WTensor< 2, 1, int > t21i;
        WTensor< 2, 2, char > t22c;
        WTensor< 2, 3, float > t23f;
        WTensor< 2, 4, float > t24f;
        WTensor< 3, 5 > t35d;
        WTensor< 4, 3 > t43d;
        WTensor< 5, 2 > t52d;
        WTensor< 6, 3 > t63d;

        TS_ASSERT_EQUALS( t35d( 0, 4, 2 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 1, 4, 0 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 0, 3, 0 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 2, 4, 1 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 0, 2, 2 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 4, 1, 4 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 4, 4, 4 ), 0.0 );
        TS_ASSERT_EQUALS( t35d( 3, 4, 3 ), 0.0 );

        TS_ASSERT_EQUALS( t11d( 0 ), 0.0 );
        TS_ASSERT_EQUALS( t22d( 0, 1 ), 0.0 );
    }

    /**
     * Test copy constructor.
     */
    void testCopyConstructor()
    {
        WTensor< 2, 3 > w;
        w( 0, 1 ) = 2;
        w( 2, 1 ) = 0.456;

        WTensor< 2, 3 > m( w );
        TS_ASSERT_EQUALS( m( 0, 1 ), 2 );
        TS_ASSERT_EQUALS( m( 2, 1 ), 0.456 );
    }

    /**
     * Test copy operator.
     */
    void testCopyOperator()
    {
        WTensor< 6, 2 > w;
        w( 0, 0, 1, 1, 0, 1 ) = 4.0;
        w( 1, 1, 0, 0, 0, 0 ) = 0.56;
        WTensor< 6, 2 > m;

        {
            m = w;
            TS_ASSERT_EQUALS( m( 0, 0, 1, 1, 0, 1 ), 4.0 );
            TS_ASSERT_EQUALS( m( 1, 1, 0, 0, 0, 0 ), 0.56 );
            TS_ASSERT_EQUALS( m( 0, 0, 0, 1, 0, 0 ), 0.0 );
        }
    }

    /**
     * Test if all the WTensorSym->WTensor copy operators and copy constructors compile.
     */
    void testCopyFromTensorSym()
    {
        // order = 3
        {
            WTensorSym< 3, 3 > s;
            s( 2, 1, 0 ) = 2.0;

            // copy construct t from s
            WTensor< 3, 3 > t( s );
            TS_ASSERT_EQUALS( t( 1, 2, 0 ), 2.0 );

            WTensor< 3, 3 > w;
            w( 0, 0, 0 ) = 3.0;

            // copy from s
            w = s;
            TS_ASSERT_EQUALS( w( 0, 0, 0 ), 0.0 );
            TS_ASSERT_EQUALS( w( 0, 2, 1 ), 2.0 );
        }
        // order = 1
        {
            WTensorSym< 1, 3 > s;
            s( 2 ) = 2.0;

            // copy construct t from s
            WTensor< 1, 3 > t( s );
            TS_ASSERT_EQUALS( t( 2 ), 2.0 );

            WTensor< 1, 3 > w;
            w( 0 ) = 3.0;

            // copy from s
            w = s;
            TS_ASSERT_EQUALS( w( 0 ), 0.0 );
            TS_ASSERT_EQUALS( w( 2 ), 2.0 );
        }
        // order = 0
        {
            WTensorSym< 0, 3 > s;
            s() = 2.0;

            // copy construct t from s
            WTensor< 0, 3 > t( s );
            TS_ASSERT_EQUALS( t(), 2.0 );

            WTensor< 0, 3 > w;
            w() = 3.0;

            // copy from s
            w = s;
            TS_ASSERT_EQUALS( w(), 2.0 );
        }
    }

    /**
     * Test casts to Data_T, WValue or WMatrix, depending on the order of the Tensor.
     */
    void testCastToVariousTypes()
    {
        // make sure these casts compile
        // we don't actually want to thoroughly test functionality here
        // more sophisticated tests can be found in WTensorFuncTest
        // cast to Data_T
        {
            WTensor< 0, 0, double > t;
            t() = 3.0;
            double d = t;
            TS_ASSERT_EQUALS( d, 3.0 );
        }
        // cast to WValue
        {
            WTensor< 1, 2, int > t;
            t( 0 ) = 3.0;
            WValue< int > v = t;
            TS_ASSERT_EQUALS( v[ 0 ], 3.0 );
        }
        // cast to WMatrix
        {
            WTensor< 2, 3, float > t;
            t( 0, 1 ) = 3.0;
            WMatrix< float > m = t;
            TS_ASSERT_EQUALS( m( 0, 1 ), 3.0 );
        }
    }
};

#endif  // WTENSOR_TEST_H
