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

#ifndef WVALUE_TEST_H
#define WVALUE_TEST_H

#include <string>

#include <cxxtest/TestSuite.h>

#include "../WValue.h"


/**
 * Testsuite for WValue.
 */
class WValueTest : public CxxTest::TestSuite
{
private:
    double delta; //!< Some values are allowed to differ by delta.

public:
    /**
     * Called before every test.
     */
    void setUp( void )
    {
        delta = 1e-12;
    }

    /**
     * Instantiation should throw nothing.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WValue< double > value( 3 ) );
        TS_ASSERT_THROWS_NOTHING( WValue< float > value( 3 ) );
    }

    /**
     * size should give the value we have put into the constructor
     */
    void testSize( void )
    {
        const size_t size = 3;
        WValue< double > value( size );
        TS_ASSERT_EQUALS( value.size(), size );
    }

    /**
     * Element access operator should return the right values
     */
    void testElementAccessOperator( void )
    {
        const size_t size = 3;
        WValue< double > value( size );
        TS_ASSERT_EQUALS( value[0], 0. );
        TS_ASSERT_EQUALS( value[1], 0. );
        TS_ASSERT_EQUALS( value[2], 0. );

        const double a = 3.14;
        value[1] = a;
        TS_ASSERT_EQUALS( value[1], a );
    }

    /**
     * Const element access operator should return the right values.
     */
    void testConstElementAccessOperator( void )
    {
        const size_t size = 3;
        const WValue< double > value( size );
        TS_ASSERT_EQUALS( value[0], 0. );
        TS_ASSERT_EQUALS( value[1], 0. );
        TS_ASSERT_EQUALS( value[2], 0. );
    }

    /**
     * == operator should return true if the WValues contain the same elements and false if the don't.
     */
    void testEqualityOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a;
        value2[1] = b;
        value2[2] = c;

        TS_ASSERT_EQUALS( value1 == value2, true );

        value2[0] += 1;

        TS_ASSERT_EQUALS( value1 == value2, false );
    }

    /**
     * != operator should return false if the WValues contain the same elements and false if the don't.
     */
    void testInEqualityOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a;
        value2[1] = b;
        value2[2] = c;

        TS_ASSERT_EQUALS( value1 != value2, false );

        value2[0] += 1;

        TS_ASSERT_EQUALS( value1 != value2, true );
    }

    /**
     * assignment operator= should assign the correct values
     */
    void testAssignmentOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value1 == value2, false );

        // test simple assignment
        value1 = value2;
        TS_ASSERT_EQUALS( value1 == value2, true );

        WValue< double > value3( size );
        WValue< double > value4( size );

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value2 == value3, false );
        TS_ASSERT_EQUALS( value2 == value4, false );

        // test whether return the reference to self works
        // for multiple assignment
        value4 = value3 = value2;
        TS_ASSERT_EQUALS( value2 == value3, true );
        TS_ASSERT_EQUALS( value2 == value4, true );
        TS_ASSERT_EQUALS( value3 == value4, true );
    }

    /**
     * plus assignment operator-= should assign the correct values
     */
    void testPlusAssignmentOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value1 == value2, false );

        // test simple plus assignement
        value1 += value2;
        TS_ASSERT_DELTA( value1[0], 1. + 2. * a, delta );
        TS_ASSERT_DELTA( value1[1], 2. + 2. * b, delta );
        TS_ASSERT_DELTA( value1[2], 3. + 2. * c, delta );

        WValue< double > value3( size );
        WValue< double > value4( size );

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value2 == value3, false );
        TS_ASSERT_EQUALS( value2 == value4, false );

        // test whether return the reference to self works
        // for multiple plus assignment
        value4 += value3 += value2;
        TS_ASSERT_EQUALS( value2 == value3, true );
        TS_ASSERT_EQUALS( value2 == value4, true );
        TS_ASSERT_EQUALS( value3 == value4, true );
    }

    /**
     * minus assignment operator-= should assign the correct values
     */
    void testMinusAssignmentOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value1 == value2, false );

        // test simple minus assignement
        value1 -= value2;
        TS_ASSERT_DELTA( value1[0], -1., delta );
        TS_ASSERT_DELTA( value1[1], -2., delta );
        TS_ASSERT_DELTA( value1[2], -3., delta );

        WValue< double > value3( size );
        WValue< double > value4( size );

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value2 == value3, false );
        TS_ASSERT_EQUALS( value2 == value4, false );

        // test whether return the reference to self works
        // for multiple minus assignment
        value4 -= value3 -= value2;
        TS_ASSERT_DELTA( value3[0], -value2[0], delta );
        TS_ASSERT_DELTA( value3[1], -value2[1], delta );
        TS_ASSERT_DELTA( value3[2], -value2[2], delta );
        TS_ASSERT_DELTA( value4[0], value2[0], delta );
        TS_ASSERT_DELTA( value4[1], value2[1], delta );
        TS_ASSERT_DELTA( value4[2], value2[2], delta );
    }

    /**
     * product with scalar assignment operator*= should assign the correct values
     */
    void testProductWithScalarAssignmentOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        const double scalar = 32.32;

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        // test simple product with scalar assignement
        value1 *= scalar;
        double expected[] = { 38.784, 109.888, 180.992 };
        TS_ASSERT_DELTA( value1[0], expected[0], delta );
        TS_ASSERT_DELTA( value1[1], expected[1], delta );
        TS_ASSERT_DELTA( value1[2], expected[2], delta );

        WValue< double > value2( size );

        // reinitialize value
        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        // this should be the precondition for the test
        TS_ASSERT_EQUALS( value2 == value1, false );

        // test whether return the reference to self works
        // for multiple assignments
        value2 = value1 *= scalar;
        TS_ASSERT_DELTA( value1[0], expected[0], delta );
        TS_ASSERT_DELTA( value1[1], expected[1], delta );
        TS_ASSERT_DELTA( value1[2], expected[2], delta );
        TS_ASSERT_DELTA( value2[0], value1[0], delta );
        TS_ASSERT_DELTA( value2[1], value1[1], delta );
        TS_ASSERT_DELTA( value2[2], value1[2], delta );
    }
    /**
     * componentwise product assignment operator*= should assign the correct values
     */
    void testComponentWiseProductAssignmentOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // test simple componentwise product assignement
        value1 *= value2;
        double expected[] = { 2.64, 18.36, 48.16 };
        TS_ASSERT_DELTA( value1[0], expected[0], delta );
        TS_ASSERT_DELTA( value1[1], expected[1], delta );
        TS_ASSERT_DELTA( value1[2], expected[2], delta );

        // reinitialize value
        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        WValue< double > value3( size );

        value3[0] = a + 1.1;
        value3[1] = b + 2.2;
        value3[2] = c + 3.3;

        double expected2[] = { 6.072, 102.816, 428.624 };

        // test whether return the reference to self works
        // for multiple assignments
        value3 *= value1 *= value2;
        TS_ASSERT_DELTA( value1[0], expected[0], delta );
        TS_ASSERT_DELTA( value1[1], expected[1], delta );
        TS_ASSERT_DELTA( value1[2], expected[2], delta );
        TS_ASSERT_DELTA( value3[0], expected2[0], delta );
        TS_ASSERT_DELTA( value3[1], expected2[1], delta );
        TS_ASSERT_DELTA( value3[2], expected2[2], delta );
    }

    /**
     * plus operator+
     */
    void testPlusOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        WValue< double > value3( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // test addition
        value3 = value1 + value2;

        TS_ASSERT_DELTA( value3[0], 2 * a + 1, delta );
        TS_ASSERT_DELTA( value3[1], 2 * b + 2, delta );
        TS_ASSERT_DELTA( value3[2], 2 * c + 3, delta );

        // Ensure that value1 and value2 have not been altered
        TS_ASSERT_EQUALS( value1[0], a );
        TS_ASSERT_EQUALS( value1[1], b );
        TS_ASSERT_EQUALS( value1[2], c );
        TS_ASSERT_EQUALS( value2[0], a + 1 );
        TS_ASSERT_EQUALS( value2[1], b + 2 );
        TS_ASSERT_EQUALS( value2[2], c + 3 );
    }

    /**
     * minus operator+-
     */
    void testMinusOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        WValue< double > value3( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // test subtraction
        value3 = value1 - value2;

        TS_ASSERT_DELTA( value3[0], -1, delta );
        TS_ASSERT_DELTA( value3[1], -2, delta );
        TS_ASSERT_DELTA( value3[2], -3, delta );

        // Ensure that value1 and value2 have not been altered
        TS_ASSERT_EQUALS( value1[0], a );
        TS_ASSERT_EQUALS( value1[1], b );
        TS_ASSERT_EQUALS( value1[2], c );
        TS_ASSERT_EQUALS( value2[0], a + 1 );
        TS_ASSERT_EQUALS( value2[1], b + 2 );
        TS_ASSERT_EQUALS( value2[2], c + 3 );
    }

    /**
     * componentwise multiplication operator*
     */
    void testComponentWiseProductOperator( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        WValue< double > value3( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        value2[0] = a + 1;
        value2[1] = b + 2;
        value2[2] = c + 3;

        // test subtraction
        value3 = value1 * value2;

        double expected[] = { 2.64, 18.36, 48.16 };
        TS_ASSERT_DELTA( value3[0], expected[0], delta );
        TS_ASSERT_DELTA( value3[1], expected[1], delta );
        TS_ASSERT_DELTA( value3[2], expected[2], delta );

        // Ensure that value1 and value2 have not been altered
        TS_ASSERT_EQUALS( value1[0], a );
        TS_ASSERT_EQUALS( value1[1], b );
        TS_ASSERT_EQUALS( value1[2], c );
        TS_ASSERT_EQUALS( value2[0], a + 1 );
        TS_ASSERT_EQUALS( value2[1], b + 2 );
        TS_ASSERT_EQUALS( value2[2], c + 3 );
    }

    /**
     * norm with doubles
     */
    void testNormDouble( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;
        TS_ASSERT_DELTA( value1.norm(), 6.660330322, 1e-7 );
    }

    /**
     * norm int
     */
    void testNormInt( void )
    {
        const size_t size = 3;
        const int a = 1, b = 2, c = 3;
        WValue< int > value1( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;
        TS_ASSERT_DELTA( value1.norm(), 3.74165738677, 1e-7 );
    }

    /**
     * normsquare
     */
    void testNormSquare( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;
        TS_ASSERT_DELTA( value1.normSquare(), 44.36, delta );
    }

    /**
     * test normalization of the current WValue
     */
    void testNormalize( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        TS_ASSERT( std::abs( value1.norm() - 1. ) > 1e-9 );
        value1.normalize();
        TS_ASSERT_DELTA( value1.norm(), 1., delta );
    }

    /**
     * test returning normalized version
     */
    void testNormalized( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;
        WValue< double > valueCopy = value1;

        TS_ASSERT( std::abs( value1.norm() - 1. ) > 1e-9 );
        WValue< double > value2 = value1.normalized();

        // value1 should not have been changed
        TS_ASSERT( std::abs( value1.norm() - 1. ) > 1e-9 );
        TS_ASSERT_EQUALS( value1, valueCopy );

        // value2 should contain the normalized version
        TS_ASSERT_DELTA( value2.norm(), 1., delta );
    }

    /**
     * scaling operator, scalar left hand side
     */
    void testScalingLeftHandSide( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        const double scalar = 32.32;

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        // test scaling with scalar left hand side
        value2 = scalar * value1;

        double expected[] = { 38.784, 109.888, 180.992 };
        TS_ASSERT_DELTA( value2[0], expected[0], delta );
        TS_ASSERT_DELTA( value2[1], expected[1], delta );
        TS_ASSERT_DELTA( value2[2], expected[2], delta );
    }

    /**
     * scaling operator, scalar right hand side
     */
    void testScalingRightHandSide( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        const double scalar = 32.32;

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;

        // test scaling with scalar right hand side
        value2 = value1 * scalar;

        double expected[] = { 38.784, 109.888, 180.992 };
        TS_ASSERT_DELTA( value2[0], expected[0], delta );
        TS_ASSERT_DELTA( value2[1], expected[1], delta );
        TS_ASSERT_DELTA( value2[2], expected[2], delta );
    }

    /**
     * ensure scaling operator commutativity
     */
    void testScalingCommutativity( void )
    {
        const size_t size = 3;
        const double a = 1.2, b = 3.4, c = 5.6;
        WValue< double > value1( size );
        WValue< double > value2( size );
        WValue< double > value3( size );
        const double scalar = 32.32;

        value1[0] = a;
        value1[1] = b;
        value1[2] = c;


        // test scaling with scalar right hand side
        value2 = value1 * scalar;
        value3 = scalar * value1;

        TS_ASSERT_EQUALS( value2, value3 );
    }

    /**
     * Every WValue should have an operator<< for writing to ostreams
     */
    void testOutputStreamOperator( void )
    {
        WValue< double > val( 2 );
        val[0] = 1.;
        val[1] = 5.;
        std::string expected( "[1.0000000000000000e+00, 5.0000000000000000e+00]" );
        std::stringstream ss;
        ss << val;
        TS_ASSERT_EQUALS( ss.str(), expected );
    }

    /**
     * Test the mean calculation.
     */
    void testMean( void )
    {
        WValue< double > val( 3 );
        val[0] = 1.0;
        val[1] = 2.0;
        val[2] = 3.0;
        TS_ASSERT_EQUALS( val.mean(), 2.0 );
    }

    /**
     * Test the median calculation.
     */
    void testMedian( void )
    {
        WValue< double > val( 3 );
        val[0] = 1.0;
        val[1] = 2.0;
        val[2] = 3.0;
        TS_ASSERT_EQUALS( val.mean(), 2.0 );
    }
};

#endif  // WVALUE_TEST_H
