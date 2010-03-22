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

#ifndef WVECTOR3D_TEST_H
#define WVECTOR3D_TEST_H

#include <cxxtest/TestSuite.h>
#include "../WVector3D.h"

using wmath::WVector3D;

/**
 * Test for WVector3D
 */
class WVector3DTest : public CxxTest::TestSuite
{
private:
    double delta; //!< Some values are allowed to differ this much
    double deltaLarge; //!< Some values are allowed to differ this much. This is more then the delta above.

public:

    /**
     * Called before every test.
     */
    void setUp( void )
    {
        delta = 1e-12;
        deltaLarge = 1e-10;
    }

public:
    /**
     * Instantiation should throw nothing.
     */
    void testInstantiationStandard( void )
    {
        TS_ASSERT_THROWS_NOTHING( WVector3D vec() );
    }

    /**
     * Instantiation should throw nothing.
     */
    void testInstantiationWithDoubles( void )
    {
        TS_ASSERT_THROWS_NOTHING( WVector3D vec( 32.32, 42.42, 23.23 ) );
    }

    /**
     * Instantiation should throw nothing.
     */
    void testInstantiationWithCopyConstructor( void )
    {
        WVector3D vecToCopy;
        TS_ASSERT_THROWS_NOTHING( WVector3D vec( vecToCopy ) );
    }

    /**
     * Instantiation with right sized WValue should throw nothing.
     */
    void testInstantiationWithCastConstructor( void )
    {
        wmath::WValue< double > arrayToCopy( 3 );
        TS_ASSERT_THROWS_NOTHING( WVector3D vec( arrayToCopy ) );
    }

    /**
     * Standard constructor should set values to zero
     */
    void testInitializationWithStandardConstructor( void )
    {
        WVector3D vec;
        TS_ASSERT_EQUALS( vec[0], 0. );
        TS_ASSERT_EQUALS( vec[1], 0. );
        TS_ASSERT_EQUALS( vec[2], 0. );
    }

    /**
     * Double constructor should set values to the provided params
     */
    void testInitializationWithDoubleConstructor( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec[0], a );
        TS_ASSERT_EQUALS( vec[1], b );
        TS_ASSERT_EQUALS( vec[2], c );
    }

    /**
     * Copy constructor should copy values
     */
    void testInitializationWithCopyConstructor( void )
    {
        WVector3D vecToCopy( 32.32, 42.42, 23.23 );
        WVector3D vec( vecToCopy );
        TS_ASSERT_EQUALS( vec[0], vecToCopy[0] );
        TS_ASSERT_EQUALS( vec[1], vecToCopy[1] );
        TS_ASSERT_EQUALS( vec[2], vecToCopy[2] );
    }

    /**
     * Cast constructor should copy values
     */
    void testInitializationWithCastConstructor( void )
    {
        wmath::WValue< double > arrayToCopy( 3 );
        arrayToCopy[0] = 32.32;
        arrayToCopy[1] = 42.42;
        arrayToCopy[2] = 23.23;

        WVector3D vec( arrayToCopy );
        TS_ASSERT_EQUALS( vec[0], arrayToCopy[0] );
        TS_ASSERT_EQUALS( vec[1], arrayToCopy[1] );
        TS_ASSERT_EQUALS( vec[2], arrayToCopy[2] );
    }

    /**
     * Element access operator should return the right values
     */
    void testElementAccessOperator( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec[0], a );
        TS_ASSERT_EQUALS( vec[1], b );
        TS_ASSERT_EQUALS( vec[2], c );
    }

    /**
     * Const element access operator should return the right values
     */
    void testConstElementAccessOperator( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        const WVector3D vec( a, b, c );
        TS_ASSERT_EQUALS( vec[0], a );
        TS_ASSERT_EQUALS( vec[1], b );
        TS_ASSERT_EQUALS( vec[2], c );
    }

    /**
     * Test cross product
     */
    void testCrossProduct( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        const WVector3D vec( a, b, c );
        const WVector3D vec2( a + 1.1, b + 2.2, c + 3.3 );

        WVector3D tmp;

        // product with self should be zero vector
        tmp = vec.crossProduct( vec );
        TS_ASSERT_DELTA( tmp[0], 0., delta );
        TS_ASSERT_DELTA( tmp[1], 0., delta );
        TS_ASSERT_DELTA( tmp[2], 0., delta );

        // test for some example
        tmp = vec.crossProduct( vec2 );
        TS_ASSERT_DELTA( tmp[0], 88.88, delta );
        TS_ASSERT_DELTA( tmp[1], -81.103, delta );
        TS_ASSERT_DELTA( tmp[2], 24.442, delta );

        // ensure orthogonality
        TS_ASSERT_DELTA( tmp.dotProduct( vec ), 0., deltaLarge );
        TS_ASSERT_DELTA( tmp.dotProduct( vec2 ), 0., deltaLarge );
    }

    /**
     * Test dot product
     */
    void testDotProduct( void )
    {
        double a = 32.32;
        double b = 42.42;
        double c = 23.23;
        const WVector3D vec( a, b, c );
        const WVector3D vec2( a + 1.1, b + 2.2, c + 3.3 );

        double tmp;

        // product with self has to be squared norm
        tmp = vec.dotProduct( vec );
        TS_ASSERT_EQUALS( tmp, vec.normSquare() );

        // test for some example
        tmp = vec.dotProduct( vec2 );
        TS_ASSERT_DELTA( tmp, 3589.2067, delta );

        // product with orthogonal vector has to be zero
        WVector3D tmpOrtho( 88.88, -81.103, 24.442 );
        tmp = vec.dotProduct( tmpOrtho );
        TS_ASSERT_DELTA( tmp, 0., delta );
    }

    /**
     * A vector a is small than a vector b if a is small than b in lexicographical order.
     */
    void testOperatorLessOnNumericalStability( void )
    {
        WVector3D a( 0, 0, 0 );
        WVector3D b( a );
        TS_ASSERT( !( a < b ) );
        b[2] = 1;
        TS_ASSERT( a < b );
        a += WVector3D( 0, 0, 1 - wlimits::FLT_EPS );
        TS_ASSERT( a < b );
    }
};

#endif  // WVECTOR3D_TEST_H
