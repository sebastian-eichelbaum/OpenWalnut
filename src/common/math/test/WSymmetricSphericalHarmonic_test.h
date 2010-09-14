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

#ifndef WSYMMETRICSPHERICALHARMONIC_TEST_H
#define WSYMMETRICSPHERICALHARMONIC_TEST_H

#include <cxxtest/TestSuite.h>

#include "../WMatrix.h"

#include "../WSymmetricSphericalHarmonic.h"

#include "WMatrixTraits.h"

/**
 * Testsuite for WPosition.
 */
class WSymmetricSphericalHarmonicTest : public CxxTest::TestSuite
{
public:
    /**
     * testCalcFRTMatrix
     * 
     */
    void testCalcFRTMatrix( void )
    {
        wmath::WMatrix<double> result( wmath::WSymmetricSphericalHarmonic::calcFRTMatrix( 4 ) );
        wmath::WMatrix<double> reference( 15, 15 );
        // j  01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
        // lj  0  2  2  2  2  2  4  4  4  4  4  4  4  4  4
        reference( 0, 0 ) = 2.0 * wmath::piDouble;
        for ( size_t i = 1; i <= 5; i++ )
        {
          reference( i, i ) = -2.0 * wmath::piDouble * 1.0 / 2.0;
        }
        for ( size_t i = 6; i <= 14; i++ )
        {
          reference( i, i ) = 2.0 * wmath::piDouble * 3.0 / 8.0;
        }
        TS_ASSERT_EQUALS( result, reference );
    }

    /**
     * testCalcSmoothingMatrix
     * 
     */
    void testCalcSmoothingMatrix( void )
    {
        wmath::WMatrix<double> result( wmath::WSymmetricSphericalHarmonic::calcSmoothingMatrix( 4 ) );
        wmath::WMatrix<double> reference( 15, 15 );
        // j  01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
        // lj  0  2  2  2  2  2  4  4  4  4  4  4  4  4  4
        reference( 0, 0 ) = 0.0;
        for ( size_t i = 1; i <= 5; i++ )
        {
          reference( i, i ) = 36.0;
        }
        for ( size_t i = 6; i <= 14; i++ )
        {
          reference( i, i ) = 400.0;
        }
        TS_ASSERT_EQUALS( result, reference );
    }
};

#endif  // WSYMMETRICSPHERICALHARMONIC_TEST_H
