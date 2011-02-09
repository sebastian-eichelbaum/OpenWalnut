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

#include <cxxtest/TestSuite.h>

#include "../WMatrix.h"
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

    /**
     * The matrix calculated by the calcSHToTensorSymMatrix function should produce tensors that
     * evaluate to the same values as the respective spherical harmonics.
     */
    void testCalcSHtoTensorMatrix()
    {
#ifdef OW_USE_OSSIM
        wmath::WValue< double > w( 6 );
        for( int i = 0; i < 6; ++i )
        {
            w[ i ] = exp( i / 6.0 );
        }

        wmath::WSymmetricSphericalHarmonic i( w );

        std::vector< wmath::WUnitSphereCoordinates > orientations;
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 1.0, 0.0, 0.0 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 0.6, -0.1, 0.2 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 1.0, 1.0, 1.0 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( -0.1, -0.3, 0.5 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 0.56347, 0.374, 0.676676 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 0.56347, 0.374, -0.676676 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 0.0, 0.0, -4.0 ).normalized() ) );
        orientations.push_back( wmath::WUnitSphereCoordinates( wmath::WVector3D( 0.0, 4.0, 1.0 ).normalized() ) );

        wmath::WMatrix< double > SHToTensor = wmath::WSymmetricSphericalHarmonic::calcSHToTensorSymMatrix( 2, orientations );
        wmath::WTensorSym< 2, 3, double > t( SHToTensor * w );

        for( std::vector< wmath::WUnitSphereCoordinates >::iterator it = orientations.begin();
             it != orientations.end(); ++it )
        {
            TS_ASSERT_DELTA( i.getValue( *it ), wmath::evaluateSphericalFunction( t, it->getEuclidean() ), 0.001 );
        }
#endif  // OW_USE_OSSIM
    }

    /**
     * Test complex SH coefficient conversion.
     */
    void testComplex()
    {
        // calc a conversion matrix
        std::vector< wmath::WVector3D > grad;
        std::vector< unsigned int > edges;
        wmath::tesselateIcosahedron( &grad, &edges, 3 );
        edges.clear();

        std::vector< wmath::WUnitSphereCoordinates > orientations;
        for( std::size_t i = 0; i < grad.size(); ++i )
        {
            if( grad[ i ][ 0 ] > 0.0 )
            {
                orientations.push_back( wmath::WUnitSphereCoordinates( grad[ i ] ) );
            }
        }
        grad.clear();

        wmath::WValue< double > values( 15 );
        for( std::size_t i = 0; i < 15; ++i )
        {
            values[ i ] = i / 15.0;
        }
        wmath::WSymmetricSphericalHarmonic sh( values );

        wmath::WValue< std::complex< double > > values2 = sh.getCoefficientsComplex();

        wmath::WMatrix< std::complex< double > > complexBaseMatrix = wmath::WSymmetricSphericalHarmonic::calcComplexBaseMatrix( orientations, 4 );

        wmath::WValue< std::complex< double > > res = complexBaseMatrix * values2;

        for( std::size_t k = 0; k < orientations.size(); ++k )
        {
            TS_ASSERT_DELTA( res[ k ].imag(), 0.0, 1e-15 );
            TS_ASSERT_DELTA( res[ k ].real(), sh.getValue( orientations[ k ] ), 1e-15 );
        }
    }
};

#endif  // WSYMMETRICSPHERICALHARMONIC_TEST_H
