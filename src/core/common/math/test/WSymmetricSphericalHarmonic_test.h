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

#include <vector>

// the following block is only for the last termporarly test
// #include <boost/nondet_random.hpp>
// #include <boost/random.hpp>
// #include <boost/random/normal_distribution.hpp>
// #include <boost/random/uniform_real.hpp>
// #include <boost/random/linear_congruential.hpp>
// #include <boost/random/uniform_real.hpp>
// #include <boost/random/variate_generator.hpp>

#include <cxxtest/TestSuite.h>

#include "../WMatrix.h"
#include "../linearAlgebra/WLinearAlgebra.h"
#include "../WValue.h"

#include "../WGeometryFunctions.h"
#include "../WSymmetricSphericalHarmonic.h"
#include "../WTensorSym.h"
#include "../WTensorFunctions.h"

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
        WMatrix<double> result( WSymmetricSphericalHarmonic< double >::calcFRTMatrix( 4 ) );
        WMatrix<double> reference( 15, 15 );
        reference.setZero();
        // j  01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
        // lj  0  2  2  2  2  2  4  4  4  4  4  4  4  4  4
        reference( 0, 0 ) = 2.0 * piDouble;
        for( size_t i = 1; i <= 5; i++ )
        {
            reference( i, i ) = -2.0 * piDouble * 1.0 / 2.0;
        }
        for( size_t i = 6; i <= 14; i++ )
        {
            reference( i, i ) = 2.0 * piDouble * 3.0 / 8.0;
        }

        for( size_t i = 0; i < 15; ++i )
        {
            for( size_t j = 0; j < 15; ++j )
            {
                TS_ASSERT_DELTA( result( i, j ), reference( i, j ), 1e-9 );
            }
        }
    }

    /**
     * testCalcSmoothingMatrix
     *
     */
    void testCalcSmoothingMatrix( void )
    {
        WMatrix<double> result( WSymmetricSphericalHarmonic< double >::calcSmoothingMatrix( 4 ) );
        WMatrix<double> reference( 15, 15 );
        reference.setZero();
        // j  01 02 03 04 05 06 07 08 09 10 11 12 13 14 15
        // lj  0  2  2  2  2  2  4  4  4  4  4  4  4  4  4
        reference( 0, 0 ) = 0.0;
        for( size_t i = 1; i <= 5; i++ )
        {
            reference( i, i ) = 36.0;
        }
        for( size_t i = 6; i <= 14; i++ )
        {
            reference( i, i ) = 400.0;
        }

        for( size_t i = 0; i < 15; ++i )
        {
            for( size_t j = 0; j < 15; ++j )
            {
                TS_ASSERT_DELTA( result( i, j ), reference( i, j ), 1e-9 );
            }
        }
    }

    // TODO(reichenbach, philips): repair the following to tests
    /**
     * The matrix calculated by the calcSHToTensorSymMatrix function should produce tensors that
     * evaluate to the same values as the respective spherical harmonics.
     */
    // void testCalcSHtoTensorMatrix()
    // {
    //     WValue<double> w( 6 );
    //     for( int i = 0; i < 6; ++i )
    //     {
    //         w[ i ] = exp( i / 6.0 );
    //     }

    //     WSymmetricSphericalHarmonic i( w );

    //     std::vector< WUnitSphereCoordinates > orientations;
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 1.0, 0.0, 0.0 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 0.6, -0.1, 0.2 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 1.0, 1.0, 1.0 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( -0.1, -0.3, 0.5 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 0.56347, 0.374, 0.676676 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 0.56347, 0.374, -0.676676 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 0.0, 0.0, -4.0 ) ) ) );
        // orientations.push_back( WUnitSphereCoordinates( normalize( WVector3d( 0.0, 4.0, 1.0 ) ) ) );

    //     WMatrix<double> SHToTensor = WSymmetricSphericalHarmonic::calcSHToTensorSymMatrix( 2, orientations );
    //     // TODO(all): remove the WValue from the following line, when WTensorSym supports WVector_2
    //     WTensorSym< 2, 3, double > t( WValue<double>( SHToTensor * w ) );

    //     for( std::vector< WUnitSphereCoordinates >::iterator it = orientations.begin();
    //          it != orientations.end();
    //          ++it )
    //     {
    //         TS_ASSERT_DELTA( i.getValue( *it ), evaluateSphericalFunction( t, it->getEuclidean() ), 0.001 );
    //     }
    // }

    /**
     * Test complex SH coefficient conversion.
     */
    // void testComplex()
    // {
    //     // calc a conversion matrix
    //     std::vector< WVector3d > grad;
    //     std::vector< unsigned int > edges;
    //     tesselateIcosahedron( &grad, &edges, 3 );
    //     edges.clear();

    //     std::vector< WUnitSphereCoordinates > orientations;
    //     for( std::size_t i = 0; i < grad.size(); ++i )
    //     {
    //         if( grad[ i ][ 0 ] > 0.0 )
    //         {
    //             orientations.push_back( WUnitSphereCoordinates( grad[ i ] ) );
    //         }
    //     }
    //     grad.clear();

    //     WVector_2 values( 15 );
    //     for( std::size_t i = 0; i < 15; ++i )
    //     {
    //         values[ i ] = i / 15.0;
    //     }
    //     WSymmetricSphericalHarmonic sh( values );

    //     WVectorComplex_2 values2 = sh.getCoefficientsComplex();

    //     WMatrixComplex_2 complexBaseMatrix = WSymmetricSphericalHarmonic::calcComplexBaseMatrix( orientations, 4 );

    //     WVectorComplex_2 res = complexBaseMatrix * values2;

    //     for( std::size_t k = 0; k < orientations.size(); ++k )
    //     {
    //         TS_ASSERT_DELTA( res[ k ].imag(), 0.0, 1e-15 );
    //         TS_ASSERT_DELTA( res[ k ].real(), sh.getValue( orientations[ k ] ), 1e-15 );
    //     }
    // }
};

#endif  // WSYMMETRICSPHERICALHARMONIC_TEST_H
