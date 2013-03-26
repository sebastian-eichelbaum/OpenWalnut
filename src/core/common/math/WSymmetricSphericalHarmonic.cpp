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

#include <stdint.h>

#include <cmath>
#include <vector>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "core/common/WLogger.h"
#include "core/common/math/WGeometryFunctions.h"
#include "../exceptions/WPreconditionNotMet.h"
#include "linearAlgebra/WLinearAlgebra.h"
#include "WLinearAlgebraFunctions.h"
#include "WMatrix.h"
#include "WTensorSym.h"
#include "WUnitSphereCoordinates.h"

#include "WSymmetricSphericalHarmonic.h"

WSymmetricSphericalHarmonic::WSymmetricSphericalHarmonic() :
    m_order( 0 ),
    m_SHCoefficients( 0 )
{
}

WSymmetricSphericalHarmonic::WSymmetricSphericalHarmonic( const WValue<double>& SHCoefficients ) :
  m_SHCoefficients( SHCoefficients )
{
  // calculate order from SHCoefficients.size:
  // - this is done by reversing the R=(l+1)*(l+2)/2 formula from the Descoteaux paper
  double q = std::sqrt( 0.25 + 2.0 * static_cast<double>( m_SHCoefficients.size() ) ) - 1.5;
  m_order = static_cast<size_t>( q );
}

WSymmetricSphericalHarmonic::~WSymmetricSphericalHarmonic()
{
}

double WSymmetricSphericalHarmonic::getValue( double theta, double phi ) const
{
  double result = 0.0;
  int j = 0;
  const double rootOf2 = std::sqrt( 2.0 );
  for( int k = 0; k <= static_cast<int>( m_order ); k+=2 )
  {
    // m = 1 .. k
    for( int m = 1; m <= k; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * rootOf2 *
                  static_cast<double>( std::pow( -1.0, m+1 ) ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
    }
    // m = 0
    result += m_SHCoefficients[ ( k*k+k+2 ) / 2 - 1 ] * boost::math::spherical_harmonic_r( k, 0, theta, phi );
    // m = -k .. -1
    for( int m = -k; m < 0; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * rootOf2 * boost::math::spherical_harmonic_r( k, -m, theta, phi );
    }
  }
  return result;
}

double WSymmetricSphericalHarmonic::getValue( const WUnitSphereCoordinates& coordinates ) const
{
  return getValue( coordinates.getTheta(), coordinates.getPhi() );
}

const WValue<double>& WSymmetricSphericalHarmonic::getCoefficients() const
{
  return m_SHCoefficients;
}

WValue< double > WSymmetricSphericalHarmonic::getCoefficientsSchultz() const
{
    WValue< double > res( m_SHCoefficients.size() );
    size_t k = 0;
    for( int l = 0; l <= static_cast< int >( m_order ); l += 2 )
    {
        for( int m = -l; m <= l; ++m )
        {
            res[ k ] = m_SHCoefficients[ k ];
            if( m < 0 && ( ( -m ) % 2 == 1 ) )
            {
                res[ k ] *= -1.0;
            }
            else if( m > 0 && ( m % 2 == 0 ) )
            {
                res[ k ] *= -1.0;
            }
            ++k;
        }
    }
    return res;
}

WValue< std::complex< double > > WSymmetricSphericalHarmonic::getCoefficientsComplex() const
{
    WValue< std::complex< double > > res( m_SHCoefficients.size() );
    size_t k = 0;
    double r = 1.0 / sqrt( 2.0 );
    std::complex< double > i( 0.0, -1.0 );
    for( int l = 0; l <= static_cast< int >( m_order ); l += 2 )
    {
        for( int m = -l; m <= l; ++m )
        {
            if( m == 0 )
            {
                res[ k ] = m_SHCoefficients[ k ];
            }
            else if( m < 0 )
            {
                res[ k ] += i * r * m_SHCoefficients[ k - 2 * m ];
                res[ k ] += ( -m % 2 == 1 ? -r : r ) * m_SHCoefficients[ k ];
            }
            else if( m > 0 )
            {
                res[ k ] += i * ( -m % 2 == 0 ? -r : r ) * m_SHCoefficients[ k ];
                res[ k ] += r * m_SHCoefficients[ k - 2 * m ];
            }
            ++k;
        }
    }
    return res;
}

double WSymmetricSphericalHarmonic::calcGFA( std::vector< WUnitSphereCoordinates > const& orientations ) const
{
    double n = static_cast< double >( orientations.size() );
    double d = 0.0;
    double gfa = 0.0;
    double mean = 0.0;
    std::vector< double > v( orientations.size() );

    for( std::size_t i = 0; i < orientations.size(); ++i )
    {
        v[ i ] = getValue( orientations[ i ] );
        mean += v[ i ];
    }
    mean /= n;

    for( std::size_t i = 0; i < orientations.size(); ++i )
    {
        double f = v[ i ];
        // we use gfa as a temporary here
        gfa += f * f;
        f -= mean;
        d += f * f;
    }

    if( gfa == 0.0 || n <= 1.0 )
    {
        return 0.0;
    }
    // this is the real gfa
    gfa = sqrt( ( n * d ) / ( ( n - 1.0 ) * gfa ) );

    WAssert( gfa >= -0.01 && gfa <= 1.01, "" );
    if( gfa < 0.0 )
    {
        return 0.0;
    }
    else if( gfa > 1.0 )
    {
        return 1.0;
    }
    return gfa;
}

double WSymmetricSphericalHarmonic::calcGFA( WMatrix< double > const& B ) const
{
    // sh coeffs
    WMatrix< double > s( B.getNbCols(), 1 );
    WAssert( B.getNbCols() == m_SHCoefficients.size(), "" );
    for( std::size_t k = 0; k < B.getNbCols(); ++k )
    {
        s( k, 0 ) = m_SHCoefficients[ k ];
    }
    s = B * s;
    WAssert( s.getNbRows() == B.getNbRows(), "" );
    WAssert( s.getNbCols() == 1u, "" );

    double n = static_cast< double >( s.getNbRows() );
    double d = 0.0;
    double gfa = 0.0;
    double mean = 0.0;
    for( std::size_t i = 0; i < s.getNbRows(); ++i )
    {
        mean += s( i, 0 );
    }
    mean /= n;

    for( std::size_t i = 0; i < s.getNbRows(); ++i )
    {
        double f = s( i, 0 );
        // we use gfa as a temporary here
        gfa += f * f;
        f -= mean;
        d += f * f;
    }

    if( gfa == 0.0 || n <= 1.0 )
    {
        return 0.0;
    }
    // this is the real gfa
    gfa = sqrt( ( n * d ) / ( ( n - 1.0 ) * gfa ) );

    WAssert( gfa >= -0.01 && gfa <= 1.01, "" );
    if( gfa < 0.0 )
    {
        return 0.0;
    }
    else if( gfa > 1.0 )
    {
        return 1.0;
    }
    return gfa;
}

void WSymmetricSphericalHarmonic::applyFunkRadonTransformation( WMatrix< double > const& frtMat )
{
    WAssert( frtMat.getNbCols() == m_SHCoefficients.size(), "" );
    WAssert( frtMat.getNbRows() == m_SHCoefficients.size(), "" );
    // Funk-Radon-Transformation as in Descoteaux's thesis
    for( size_t j = 0; j < m_SHCoefficients.size(); j++ )
    {
        m_SHCoefficients[ j ] *= frtMat( j, j );
    }
}

size_t WSymmetricSphericalHarmonic::getOrder() const
{
  return m_order;
}

WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< WVector3d >& orientations,
                                                                        int order,
                                                                        double lambda,
                                                                        bool withFRT )
{
  // convert euclid 3d orientations/gradients to sphere coordinates
  std::vector< WUnitSphereCoordinates > sphereCoordinates;
  for( std::vector< WVector3d >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
  {
    sphereCoordinates.push_back( WUnitSphereCoordinates( *it ) );
  }
  return WSymmetricSphericalHarmonic::getSHFittingMatrix( sphereCoordinates, order, lambda, withFRT );
}

WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< WUnitSphereCoordinates >& orientations,
                                                                 int order,
                                                                 double lambda,
                                                                 bool withFRT )
{
  WMatrix<double> B( WSymmetricSphericalHarmonic::calcBaseMatrix( orientations, order ) );
  WMatrix<double> Bt( B.transposed() );
  WMatrix<double> result( Bt*B );
  if( lambda != 0.0 )
  {
    WMatrix<double> L( WSymmetricSphericalHarmonic::calcSmoothingMatrix( order ) );
    result += lambda*L;
  }

  result = pseudoInverse( result )*Bt;
  if( withFRT )
  {
    WMatrix<double> P( WSymmetricSphericalHarmonic::calcFRTMatrix( order ) );
    return ( P * result );
  }
  return result;
}

WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrixForConstantSolidAngle( const std::vector< WVector3d >& orientations,
                                                                                      int order,
                                                                                      double lambda )
{
  // convert euclid 3d orientations/gradients to sphere coordinates
  std::vector< WUnitSphereCoordinates > sphereCoordinates;
  for( std::vector< WVector3d >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
  {
    sphereCoordinates.push_back( WUnitSphereCoordinates( *it ) );
  }
  return WSymmetricSphericalHarmonic::getSHFittingMatrixForConstantSolidAngle( sphereCoordinates, order, lambda );
}

WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrixForConstantSolidAngle(
    const std::vector< WUnitSphereCoordinates >& orientations,
    int order,
    double lambda )
{
  WMatrix<double> B( WSymmetricSphericalHarmonic::calcBaseMatrix( orientations, order ) );
  WMatrix<double> Bt( B.transposed() );
  WMatrix<double> result( Bt*B );

  // regularisation
  if( lambda != 0.0 )
  {
    WMatrix<double> L( WSymmetricSphericalHarmonic::calcSmoothingMatrix( order ) );
    result += lambda*L;
  }

  result = pseudoInverse( result )*Bt;

  // multiply with eigenvalues of coefficients / Laplace-Beltrami operator
  WMatrix<double> LB( WSymmetricSphericalHarmonic::calcMatrixWithEigenvalues( order ) );
  wlog::debug( "" ) << "LB: " << LB;
  result = LB*result;
  wlog::debug( "" ) << "LB*result: " << result;

  // apply FRT
  WMatrix<double> P( WSymmetricSphericalHarmonic::calcFRTMatrix( order ) );
  result = P * result;
  wlog::debug( "" ) << "P: " << P;
  wlog::debug( "" ) << "P*result: " << result;

  // correction factor
  double correctionFactor = 1.0 / ( 16.0 * std::pow( piDouble, 2 ) );
  result *= correctionFactor;

  return result;
}


WMatrix<double> WSymmetricSphericalHarmonic::calcBaseMatrix( const std::vector< WUnitSphereCoordinates >& orientations,
                                                                    int order )
{
  WMatrix<double> B( orientations.size(), ( ( order + 1 ) * ( order + 2 ) ) / 2 );

  // calc B Matrix like in the 2007 Descoteaux paper ("Regularized, Fast, and Robust Analytical Q-Ball Imaging")
  int j = 0;
  const double rootOf2 = std::sqrt( 2.0 );
  for(uint32_t row = 0; row < orientations.size(); row++ )
  {
    const double theta = orientations[row].getTheta();
    const double phi = orientations[row].getPhi();
    for( int k = 0; k <= order; k+=2 )
    {
      // m = 1 .. k
      for( int m = 1; m <= k; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2 * static_cast<double>( std::pow( static_cast<double>( -1 ), m + 1 ) )
                        * boost::math::spherical_harmonic_i( k, m, theta, phi );
      }
      // m = 0
      B( row, ( k * k + k + 2 ) / 2 - 1 ) = boost::math::spherical_harmonic_r( k, 0, theta, phi );
      // m = -k .. -1
      for( int m = -k; m < 0; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2*boost::math::spherical_harmonic_r( k, -m, theta, phi );
      }
    }
  }
  return B;
}

WMatrix< std::complex< double > >
WSymmetricSphericalHarmonic::calcComplexBaseMatrix( std::vector< WUnitSphereCoordinates > const& orientations, int order )
{
    WMatrix< std::complex< double > > B( orientations.size(), ( ( order + 1 ) * ( order + 2 ) ) / 2 );

    for( std::size_t row = 0; row < orientations.size(); row++ )
    {
        const double theta = orientations[ row ].getTheta();
        const double phi = orientations[ row ].getPhi();

        int j = 0;
        for( int k = 0; k <= order; k += 2 )
        {
            for( int m = -k; m < 0; m++ )
            {
                B( row, j ) = boost::math::spherical_harmonic( k, m, theta, phi );
                ++j;
            }
            B( row, j ) = boost::math::spherical_harmonic( k, 0, theta, phi );
            ++j;
            for( int m = 1; m <= k; m++ )
            {
                B( row, j ) = boost::math::spherical_harmonic( k, m, theta, phi );
                ++j;
            }
        }
    }
    return B;
}

WValue<double> WSymmetricSphericalHarmonic::calcEigenvalues( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    WValue<double> result( R );
    for( size_t k = 0; k <= order; k += 2 )
    {
        for( int m = -static_cast< int >( k ); m <= static_cast< int >( k ); ++m )
        {
            result[ i ] = -static_cast< double > ( k * ( k + 1 ) );
            ++i;
        }
    }
    return result;
}

WMatrix<double> WSymmetricSphericalHarmonic::calcMatrixWithEigenvalues( size_t order )
{
    WValue<double> eigenvalues( WSymmetricSphericalHarmonic::calcEigenvalues( order ) );
    WMatrix<double> L( eigenvalues.size(), eigenvalues.size() );
    for( std::size_t i = 0; i < eigenvalues.size(); ++i )
    {
        L( i, i ) = eigenvalues[ i ];
    }
    return L;
}

WMatrix<double> WSymmetricSphericalHarmonic::calcSmoothingMatrix( size_t order )
{
    WValue<double> eigenvalues( WSymmetricSphericalHarmonic::calcEigenvalues( order ) );
    WMatrix<double> L( eigenvalues.size(), eigenvalues.size() );
    for( std::size_t i = 0; i < eigenvalues.size(); ++i )
    {
        L( i, i ) = std::pow( eigenvalues[ i ], 2 );
    }
    return L;
}

WMatrix<double> WSymmetricSphericalHarmonic::calcFRTMatrix( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    WMatrix< double > result( R, R );
    for( size_t k = 0; k <= order; k += 2 )
    {
        double h = 2.0 * piDouble * static_cast< double >( std::pow( static_cast< double >( -1 ), static_cast< double >( k / 2 ) ) ) *
                    static_cast< double >( oddFactorial( ( k <= 1 ) ? 1 : k - 1 ) ) / static_cast<double>( evenFactorial( k ) );
        for( int m = -static_cast< int >( k ); m <= static_cast< int >( k ); ++m )
        {
            result( i, i ) = h;
            ++i;
        }
    }
    return result;
}

WMatrix< double > WSymmetricSphericalHarmonic::calcSHToTensorSymMatrix( std::size_t order )
{
    std::vector< WVector3d > vertices;
    std::vector< unsigned int > triangles;
    // calc necessary tesselation level
    int level = static_cast< int >( log( static_cast< float >( ( order + 1 ) * ( order + 2 ) ) ) / 2.0f + 1.0f );
    tesselateIcosahedron( &vertices, &triangles, level );
    std::vector< WUnitSphereCoordinates > orientations;
    for( std::vector< WVector3d >::const_iterator cit = vertices.begin(); cit != vertices.end(); cit++ )
    {
        // avoid linear dependent orientations
        if( ( *cit )[ 0 ] >= 0.0001 )
        {
            orientations.push_back( WUnitSphereCoordinates( *cit ) );
        }
    }
    return WSymmetricSphericalHarmonic::calcSHToTensorSymMatrix( order, orientations );
}

WMatrix< double > WSymmetricSphericalHarmonic::calcSHToTensorSymMatrix( std::size_t order,
                                                                        const std::vector< WUnitSphereCoordinates >& orientations )
{
    std::size_t numElements = ( order + 1 ) * ( order + 2 ) / 2;
    WPrecondEquals( order % 2, 0u );
    WPrecondLess( numElements, orientations.size() + 1 );

    // store first numElements orientations as 3d-vectors
    std::vector< WVector3d > directions( numElements );
    for( std::size_t i = 0; i < numElements; ++i )
    {
        directions[ i ] = orientations[ i ].getEuclidean();
    }

    // initialize an index array
    std::vector< std::size_t > indices( order, 0 );

    // calc tensor evaluation matrix
    WMatrix< double > TEMat( numElements, numElements );
    for( std::size_t j = 0; j < numElements; ++j ) // j is the 'permutation index'
    {
        // stores how often each value is represented in the index array
        std::size_t amount[ 3 ] = { 0, 0, 0 };
        for( std::size_t k = 0; k < order; ++k )
        {
            ++amount[ indices[ k ] ];
        }

        // from WTensorSym.h
        std::size_t multiplicity = calcSupersymmetricTensorMultiplicity( order, amount[ 0 ], amount[ 1 ], amount[ 2 ] );
        for( std::size_t i = 0; i < numElements; ++i ) // i is the 'direction index'
        {
            TEMat( i, j ) = multiplicity;
            for( std::size_t k = 0; k < order; ++k )
            {
                TEMat( i, j ) *= directions[ i ][ indices[ k ] ];
            }
        }

        // from TensorBase.h
        positionIterateSortedOneStep( order, 3, indices );
    }
    directions.clear();

    // we do not want more orientations than nessessary
    std::vector< WUnitSphereCoordinates > ori2( orientations.begin(), orientations.begin() + numElements );

    WMatrix< double > p = pseudoInverse( TEMat );

    WAssert( ( TEMat*p ).isIdentity( 1e-3 ), "Test of inverse matrix failed. Are the given orientations linear independent?" );

    return p * calcBaseMatrix( ori2, order );
}

void WSymmetricSphericalHarmonic::normalize()
{
  double scale = 0.0;
  if( m_SHCoefficients.size() > 0 )
    scale = std::sqrt( 4.0 * piDouble ) * m_SHCoefficients[0];
  for( size_t i = 0; i < m_SHCoefficients.size(); i++ )
  {
    m_SHCoefficients[ i ] /= scale;
  }
}
// NOLINT
