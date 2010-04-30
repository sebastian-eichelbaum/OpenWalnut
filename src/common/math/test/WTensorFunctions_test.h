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

#ifndef WTENSORFUNCTIONS_TEST_H
#define WTENSORFUNCTIONS_TEST_H

#include <string>
#include <vector>

#include <cxxtest/TestSuite.h>
#include "../WTensorFunctions.h"

/**
 * Test class for some tensor functions.
 */
class WTensorFunctionsTest : public CxxTest::TestSuite
{
public:
    /**
     * Test the jacobi eigenvector calculation.
     */
    void testJacobiEigenvectors()
    {
        // simple diagonal matrices should be decomposed correctly

        // 1 2 3

        // 1 2 -3

        // 1 2 2

        // -1 -1 -1

        // 1 0 1

        // 0 0 0

        // similar eigenvalues
        // 2.000001 0.0 1.999998

        // very large eigenvalues
        // 3.824572321236e1000 1 2

        // very small eigenvalues
        // 3.824572321236e-1000 1 2

        // some more sophisticated tests
        // (use orthogonal transformations on diagonal matrices to create test cases)
    }
};

/**
 * Test class for all tensor operators.
 */
class WTensorOperatorsTest : public CxxTest::TestSuite
{
public:
    /**
     * Test order 2 tensor multiplication.
     */
    void testMultiplyTensorsOperator()
    {
        // some special cases, WTensor * WTensor
        TS_ASSERT_EQUALS( zero * zero, zero );
        TS_ASSERT_EQUALS( zero * one, zero );
        TS_ASSERT_EQUALS( one * zero, zero );
        TS_ASSERT_EQUALS( one * one, one );
        TS_ASSERT_EQUALS( one * rdm1, rdm1 );
        TS_ASSERT_EQUALS( one * rdm2, rdm2 );
        TS_ASSERT_EQUALS( rdm1 * one, rdm1 );
        TS_ASSERT_EQUALS( rdm2 * one, rdm2 );

        // a normal case
        TS_ASSERT_EQUALS( rdm1 * rdm2, res1 );

        // some special cases, WTensorSym * WTensorSym
        TS_ASSERT_EQUALS( szero * szero, zero );
        TS_ASSERT_EQUALS( szero * sone, zero );
        TS_ASSERT_EQUALS( sone * szero, zero );
        TS_ASSERT_EQUALS( sone * sone, one );
        TS_ASSERT_EQUALS( sone * srdm1, res2 );
        TS_ASSERT_EQUALS( srdm1 * sone, res2 );
        TS_ASSERT_EQUALS( srdm1 * srdm2, res3 );

        // WTensor * WTensorSym
        TS_ASSERT_EQUALS( zero * szero, zero );
        TS_ASSERT_EQUALS( one * sone, one );
        TS_ASSERT_EQUALS( zero * sone, zero );
        TS_ASSERT_EQUALS( one * szero, zero );

        // WTensorSym * WTensor
        TS_ASSERT_EQUALS( szero * zero, zero );
        TS_ASSERT_EQUALS( sone * one, one );
        TS_ASSERT_EQUALS( szero * one, zero );
        TS_ASSERT_EQUALS( sone * zero, zero );
        TS_ASSERT_EQUALS( srdm1 * rdm1, res4 );
    }

private:
    /**
     * Initialize a lot of tensors.
     */
    void setUp()
    {
        one( 0, 0 ) = one( 1, 1 ) = one( 2, 2 ) = 1.0;
        sone( 0, 0 ) = sone( 1, 1 ) = sone( 2, 2 ) = 1.0;

        rdm1( 0, 0 ) = 2;
        rdm1( 0, 1 ) = 3;
        rdm1( 0, 2 ) = 1;
        rdm1( 1, 0 ) = 0;
        rdm1( 1, 1 ) = 4;
        rdm1( 1, 2 ) = 0;
        rdm1( 2, 0 ) = 5;
        rdm1( 2, 1 ) = -3;
        rdm1( 2, 2 ) = -7;

        rdm2( 0, 0 ) = -1;
        rdm2( 0, 1 ) = -1;
        rdm2( 0, 2 ) = -1;
        rdm2( 1, 0 ) = 3;
        rdm2( 1, 1 ) = 0;
        rdm2( 1, 2 ) = -2;
        rdm2( 2, 0 ) = -1;
        rdm2( 2, 1 ) = -2;
        rdm2( 2, 2 ) = -3;

        res1( 0, 0 ) = 6;
        res1( 0, 1 ) = -4;
        res1( 0, 2 ) = -11;
        res1( 1, 0 ) = 12;
        res1( 1, 1 ) = 0;
        res1( 1, 2 ) = -8;
        res1( 2, 0 ) = -7;
        res1( 2, 1 ) = 9;
        res1( 2, 2 ) = 22;

        srdm1( 0, 0 ) = 2;
        srdm1( 0, 1 ) = 3;
        srdm1( 0, 2 ) = 1;
        srdm1( 1, 1 ) = -1;
        srdm1( 1, 2 ) = 4;
        srdm1( 2, 2 ) = -3;

        srdm2( 0, 0 ) = -2;
        srdm2( 0, 1 ) = 5;
        srdm2( 0, 2 ) = 2;
        srdm2( 1, 1 ) = 0;
        srdm2( 1, 2 ) = -3;
        srdm2( 2, 2 ) = 1;

        // copy symmetric tensor srdm1 to an asymmetric tensor
        res2 = srdm1;

        res3( 0, 0 ) = 13;
        res3( 0, 1 ) = 7;
        res3( 0, 2 ) = -4;
        res3( 1, 0 ) = -3;
        res3( 1, 1 ) = 3;
        res3( 1, 2 ) = 13;
        res3( 2, 0 ) = 12;
        res3( 2, 1 ) = 14;
        res3( 2, 2 ) = -13;

        res4( 0, 0 ) = 9;
        res4( 0, 1 ) = 15;
        res4( 0, 2 ) = -5;
        res4( 1, 0 ) = 26;
        res4( 1, 1 ) = -7;
        res4( 1, 2 ) = -25;
        res4( 2, 0 ) = -13;
        res4( 2, 1 ) = 28;
        res4( 2, 2 ) = 22;
    }

    //! a test tensor
    wmath::WTensor< 2, 3, int > one;
    //! a test tensor
    wmath::WTensor< 2, 3, int > zero;
    //! a test tensor
    wmath::WTensor< 2, 3, int > rdm1;
    //! a test tensor
    wmath::WTensor< 2, 3, int > rdm2;
    //! a test tensor
    wmath::WTensor< 2, 3, int > res1;
    //! a test tensor
    wmath::WTensor< 2, 3, int > res2;
    //! a test tensor
    wmath::WTensor< 2, 3, int > res3;
    //! a test tensor
    wmath::WTensor< 2, 3, int > res4;
    //! a test tensor
    wmath::WTensorSym< 2, 3, int > sone;
    //! a test tensor
    wmath::WTensorSym< 2, 3, int > szero;
    //! a test tensor
    wmath::WTensorSym< 2, 3, int > srdm1;
    //! a test tensor
    wmath::WTensorSym< 2, 3, int > srdm2;
};

    //! a test tensor
#endif  // WTENSORFUNCTIONS_TEST_H
