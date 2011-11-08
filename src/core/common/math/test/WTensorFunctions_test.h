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
#include <algorithm>

#include <cxxtest/TestSuite.h>

#include "../../WAssert.h"
#include "../../WLimits.h"
#include "../../WStringUtils.h"
#include "../WTensorFunctions.h"

/**
 * Test class for some tensor functions.
 */
class WTensorFunctionsTest : public CxxTest::TestSuite
{
public:
    /**
     * The eigenvalue of the symmetrical matrix:
     * 0.000179516, 2.09569e-05, 2.76557e-06, 0.000170189, -5.52619e-07, 0.00015239
     * (0.000196397;0.000155074;0.000150625)
     */
    void testSpecialSymMatrixEigenvalueTestCaseNumericalStability( void )
    {
        WTensorSym< 2, 3 > t;
        t( 0, 0 ) =  0.000179516;
        t( 0, 1 ) =  2.09569e-05;
        t( 0, 2 ) =  2.76557e-06;
        t( 1, 1 ) =  0.000170189;
        t( 1, 2 ) = -5.52619e-07;
        t( 2, 2 ) =  0.00015239;
        RealEigenSystem sys;
        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.5062467240725114e-04, 1e-9 );
        TS_ASSERT_DELTA( sys[1].first, 1.5507354000104679e-04, 1e-9 );
        TS_ASSERT_DELTA( sys[2].first, 1.9639678759170208e-04, 1e-9 );
    }

    /**
     * Test the jacobi eigenvector calculation.
     */
    void testJacobiEigenvectors()
    {
        // the test matrix
        WTensorSym< 2, 3 > t;

        // simple diagonal matrices should be decomposed correctly
        // 1 2 3
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = 3.0;

        RealEigenSystem sys;
        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 3.0, 1e-6 );
        // eigenvectors should be perpendicular
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // 1 2 -3
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = -3.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, -3.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // 1 2 2
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = 2.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // -1 -1 -1
        t( 0, 0 ) = -1.0;
        t( 1, 1 ) = -1.0;
        t( 2, 2 ) = -1.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, -1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, -1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, -1.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // 1 0 1
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 1.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 0.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // 0 0 0
        t( 0, 0 ) = 0.0;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 0.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 0.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 0.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 0.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // similar eigenvalues
        // 2.000001 0.0 1.999998
        t( 0, 0 ) = 2.000001;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 1.999998;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 0.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 1.999998, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 2.000001, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // very large eigenvalues
        // 3.824572321236e1000 1 2
        t( 0, 0 ) = 3.824572321236e30;
        t( 1, 1 ) = 1.0;
        t( 2, 2 ) = 2.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 3.824572321236e30, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // very small eigenvalues
        // 3.824572321236e-1000 1 2
        t( 0, 0 ) = 3.824572321236e-30;
        t( 1, 1 ) = 1.0;
        t( 2, 2 ) = 2.0;

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 3.824572321236e-30, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );

        // some more sophisticated tests
        // (using similarity transformations on diagonal matrices to create test cases)
        t( 0, 0 ) = 1;
        t( 1, 1 ) = 2;
        t( 2, 2 ) = 3;

        // note that the jacobi eigenvector functions does not sort the eigenvalues and
        // eigenvectors that were found
        t = similarity_rotate_givens( t, 0, 2, 2.78 );

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 3.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );
        compare_results( t, sys );

        t = WTensorSym< 2, 3 >();
        t( 0, 0 ) = 2;
        t( 1, 1 ) = 1;
        t( 2, 2 ) = 3;

        t = similarity_rotate_givens( t, 0, 2, 2.79 );
        t = similarity_rotate_givens( t, 1, 2, -3.44 );
        t = similarity_rotate_givens( t, 1, 0, -0.46 );
        t = similarity_rotate_givens( t, 2, 1, 5.98 );

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 1.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 3.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );
        compare_results( t, sys );

        t = WTensorSym< 2, 3 >();
        t( 0, 0 ) = 2;
        t( 1, 1 ) = 2;
        t( 2, 2 ) = 2;

        t = similarity_rotate_givens( t, 1, 2, -3.44 );
        t = similarity_rotate_givens( t, 2, 1, 5.98 );
        t = similarity_rotate_givens( t, 0, 2, 2.79 );
        t = similarity_rotate_givens( t, 1, 0, -0.46 );

        jacobiEigenvector3D( t, &sys );

        TS_ASSERT_DELTA( sys[0].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[1].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( sys[2].first, 2.0, 1e-6 );
        TS_ASSERT_DELTA( dot( sys[0].second, sys[1].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[1].second, sys[2].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( dot( sys[2].second, sys[0].second ), 0.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[0].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[1].second ), 1.0, 1e-9 );
        TS_ASSERT_DELTA( length( sys[2].second ), 1.0, 1e-9 );
        compare_results( t, sys );
    }

    /**
     * Test the cardano eigenvalue calculation.
     */
    void testCardanoEigenvalues()
    {
        // the test matrix
        WTensorSym< 2, 3 > t;
        // variables for the output values
        std::vector< double > d( 3 );

        // simple diagonal matrices should be decomposed correctly

        // 1 2 3
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = 3.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 3.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 1.0, 1e-6 );

        // 1 2 -3
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = -3.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], -3.0, 1e-6 );

        // 1 2 2
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 2.0;
        t( 2, 2 ) = 2.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 1.0, 1e-6 );

        // -1 -1 -1
        t( 0, 0 ) = -1.0;
        t( 1, 1 ) = -1.0;
        t( 2, 2 ) = -1.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], -1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], -1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], -1.0, 1e-6 );

        // 1 0 1
        t( 0, 0 ) = 1.0;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 1.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 0.0, 1e-6 );

        // 0 0 0
        t( 0, 0 ) = 0.0;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 0.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 0.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 0.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 0.0, 1e-6 );

        // similar eigenvalues
        // 2.000001 0.0 1.999998
        t( 0, 0 ) = 2.000001;
        t( 1, 1 ) = 0.0;
        t( 2, 2 ) = 1.999998;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 2.000001, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 1.999998, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 0.0, 1e-6 );

        // very large eigenvalues
        // 3.824572321236e10 1 2
        t( 0, 0 ) = 3.824572321236e10;
        t( 1, 1 ) = 1.0;
        t( 2, 2 ) = 2.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 3.824572321236e10, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 1.0, 1e-6 );

        // very small eigenvalues
        // 3.824572321236e-10 1 2
        t( 0, 0 ) = 3.824572321236e-30;
        t( 1, 1 ) = 1.0;
        t( 2, 2 ) = 2.0;

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 1.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 3.824572321236e-30, 1e-6 );

        // some more sophisticated tests
        // (using similarity transformations on diagonal matrices to create test cases)
        t( 0, 0 ) = 1;
        t( 1, 1 ) = 2;
        t( 2, 2 ) = 3;

        // note that the jacobi eigenvector functions does not sort the eigenvalues and
        // eigenvectors that were found
        t = similarity_rotate_givens( t, 0, 2, 2.78 );

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 3.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 1.0, 1e-6 );

        // the eigenvalues calculated for this test are a bit off
        //t = WTensorSym< 2, 3 >();
        //t( 0, 0 ) = 2;
        //t( 1, 1 ) = 1;
        //t( 2, 2 ) = 3;

        //t = similarity_rotate_givens( t, 0, 2, 2.79 );
        //t = similarity_rotate_givens( t, 1, 2, -3.44 );
        //t = similarity_rotate_givens( t, 1, 0, -0.46 );
        //t = similarity_rotate_givens( t, 2, 1, 5.98 );

        //d = getEigenvaluesCardano( t );

        //TS_ASSERT_DELTA( d[ 0 ], 3.0, 1e-6 );
        //TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        //TS_ASSERT_DELTA( d[ 2 ], 1.0, 1e-6 );

        t = WTensorSym< 2, 3 >();
        t( 0, 0 ) = 2;
        t( 1, 1 ) = 2;
        t( 2, 2 ) = 2;

        t = similarity_rotate_givens( t, 1, 2, -3.44 );
        t = similarity_rotate_givens( t, 2, 1, 5.98 );
        t = similarity_rotate_givens( t, 0, 2, 2.79 );
        t = similarity_rotate_givens( t, 1, 0, -0.46 );

        d = getEigenvaluesCardano( t );

        TS_ASSERT_DELTA( d[ 0 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 1 ], 2.0, 1e-6 );
        TS_ASSERT_DELTA( d[ 2 ], 2.0, 1e-6 );
    }

    /**
     * Test if tensor log and exp functions behave correctly.
     */
    void testLogAndExp()
    {
        // init some tensor
        WTensorSym< 2, 3, double > tens;
        tens( 0, 0 ) = 8.0;
        tens( 0, 1 ) = 10.0;
        tens( 0, 2 ) = 4.0;
        tens( 1, 1 ) = 17.0;
        tens( 1, 2 ) = 14.0;
        tens( 2, 2 ) = 20.0;

        WTensorSym< 2, 3, double > s = tensorExp( tensorLog( tens ) );

        TS_ASSERT_DELTA( s( 0, 0 ), tens( 0, 0 ), 1e-4 );
        TS_ASSERT_DELTA( s( 0, 1 ), tens( 0, 1 ), 1e-4 );
        TS_ASSERT_DELTA( s( 0, 2 ), tens( 0, 2 ), 1e-4 );
        TS_ASSERT_DELTA( s( 1, 1 ), tens( 1, 1 ), 1e-4 );
        TS_ASSERT_DELTA( s( 1, 2 ), tens( 1, 2 ), 1e-4 );
        TS_ASSERT_DELTA( s( 2, 2 ), tens( 2, 2 ), 1e-4 );
    }

private:
    /**
     * A helper function performing a similarity transform using a givens rotation.
     *
     * \param m The symmetric tensor to transform.
     * \param i A row index.
     * \param j A column index.
     * \param angle The rotation angle (in radians).
     *
     * \note i must not have the same values as j
     *
     * \return The new tensor
     */
    template< std::size_t dim, typename Data_T >
    WTensorSym< 2, dim, Data_T > similarity_rotate_givens( WTensorSym< 2, dim, Data_T > const& m,
                                                                  std::size_t i,
                                                                  std::size_t j,
                                                                  double angle )
    {
        WAssert( i != j, "" );

        double s = sin( angle );
        double c = cos( angle );
        WTensor< 2, dim, Data_T > r;
        for( std::size_t k = 0; k < dim; ++k )
        {
            if( k == i || k == j )
            {
                r( k, k ) = c;
            }
            else
            {
                r( k, k ) = 1.0;
            }
        }
        r( i, j ) = s;
        r( j, i ) = -s;
        WTensor< 2, dim, Data_T > t( r );
        std::swap( t( i, j ), t( j, i ) );

        r = t * m * r;
        WTensorSym< 2, dim, Data_T > res;
        res( 0, 0 ) = r( 0, 0 );
        res( 1, 0 ) = r( 1, 0 );
        res( 2, 0 ) = r( 2, 0 );
        res( 1, 1 ) = r( 1, 1 );
        res( 2, 1 ) = r( 2, 1 );
        res( 2, 2 ) = r( 2, 2 );

        return res;
    }

    /**
     * Test if the given vectors are eigenvectors to the given eigenvalues of a
     * symmetric matrix.
     *
     * \param m A symmetric matrix.
     * \param sys The eigen system ( eigenvalues and eigenvectors )
     */
    template< std::size_t dim, typename Data_T >
    void compare_results( WTensorSym< 2, dim, Data_T > const& m, RealEigenSystem const& sys )
    {
        WTensor< 2, dim, Data_T > t, r;
        for( std::size_t i = 0; i < dim; ++i )
        {
            for( std::size_t j = 0; j < dim; ++j )
            {
                t( j, i ) = sys[i].second[j];
                r( j, i ) = sys[i].first * sys[i].second[j];
            }
        }

        t = m * t;
        for( std::size_t i = 0; i < dim; ++i )
        {
            for( std::size_t j = 0; j < dim; ++j )
            {
                TS_ASSERT_DELTA( t( i, j ), r( i, j ), 1e-15 );
            }
        }
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

    /**
     * The optimizations for symmetric tensors should not corrupt the result.
     */
    void testEvaluateSphericalFunction()
    {
        WTensorSym< 4, 3, double > t;
        // the tensor
        t( 0, 0, 0, 0 ) = 2.5476;
        t( 1, 1, 1, 1 ) = 3.5476;
        t( 2, 2, 2, 2 ) = 4.5476;
        t( 0, 0, 0, 1 ) = 5.5476;
        t( 0, 0, 0, 2 ) = 6.5476;
        t( 1, 1, 1, 0 ) = 7.5476;
        t( 1, 1, 1, 2 ) = 8.5476;
        t( 2, 2, 2, 0 ) = 9.5476;
        t( 2, 2, 2, 1 ) = 10.5476;
        t( 0, 0, 1, 2 ) = 11.5476;
        t( 1, 1, 0, 2 ) = 12.5476;
        t( 2, 2, 0, 1 ) = 13.5476;
        t( 0, 0, 1, 1 ) = 14.5476;
        t( 0, 0, 2, 2 ) = 15.5476;
        t( 1, 1, 2, 2 ) = 16.5476;

        // the gradients
        std::vector< WVector3d > gradients;
        gradients.push_back( WVector3d( 1.0, 0.0, 0.0 ) );
        gradients.push_back( WVector3d( 0.0, 1.0, 0.0 ) );
        gradients.push_back( normalize( WVector3d( 1.0, 1.0, 0.0 ) ) );
        gradients.push_back( normalize( WVector3d( 0.3, 0.4, 0.5 ) ) );
        gradients.push_back( normalize( WVector3d( -7.0, 3.0, -1.0 ) ) );

        for( int k = 0; k < 5; ++k )
        {
            double res = calcTens( t, gradients[ k ] );
            TS_ASSERT_DELTA( res, evaluateSphericalFunction( t, gradients[ k ] ), 0.001 );
        }
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

    /**
     * A helper function that implements the simple approach to tensor evaluation.
     *
     * \param t The tensor.
     * \param v The gradient.
     *
     * \return value
     */
    double calcTens( WTensorSym< 4, 3, double > const& t, WVector3d const& v )
    {
        double res = 0.0;
        for( int a = 0; a < 3; ++a )
        {
            for( int b = 0; b < 3; ++b )
            {
                for( int c = 0; c < 3; ++c )
                {
                    for( int d = 0; d < 3; ++d )
                    {
                        res += v[ a ] * v[ b ] * v[ c ] * v[ d ] * t( a, b, c, d );
                    }
                }
            }
        }
        return res;
    }

    //! a test tensor
    WTensor< 2, 3, int > one;
    //! a test tensor
    WTensor< 2, 3, int > zero;
    //! a test tensor
    WTensor< 2, 3, int > rdm1;
    //! a test tensor
    WTensor< 2, 3, int > rdm2;
    //! a test tensor
    WTensor< 2, 3, int > res1;
    //! a test tensor
    WTensor< 2, 3, int > res2;
    //! a test tensor
    WTensor< 2, 3, int > res3;
    //! a test tensor
    WTensor< 2, 3, int > res4;
    //! a test tensor
    WTensorSym< 2, 3, int > sone;
    //! a test tensor
    WTensorSym< 2, 3, int > szero;
    //! a test tensor
    WTensorSym< 2, 3, int > srdm1;
    //! a test tensor
    WTensorSym< 2, 3, int > srdm2;
};

#endif  // WTENSORFUNCTIONS_TEST_H
