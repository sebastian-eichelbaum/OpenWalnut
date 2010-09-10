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

#include <vector>

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "stdint.h"

#include "WLinearAlgebraFunctions.h"
#include "WMatrix.h"
#include "WUnitSphereCoordinates.h"
#include "WValue.h"

#include "WSymmetricSphericalHarmonic.h"

namespace wmath
{
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
  for ( int k = 0; k <= static_cast<int>( m_order ); k+=2 )
  {
    // m = 1 .. k
    for ( int m = 1; m <= k; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * rootOf2 *
                  static_cast<double>( std::pow( -1.0, m+1 ) ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
    }
    // m = 0
    result += m_SHCoefficients[ ( k*k+k+2 ) / 2 - 1 ] * boost::math::spherical_harmonic_r( k, 0, theta, phi );
    // m = -k .. -1
    for ( int m = -k; m < 0; m++ )
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

const wmath::WValue<double>& WSymmetricSphericalHarmonic::getCoefficients() const
{
  return m_SHCoefficients;
}

double WSymmetricSphericalHarmonic::calcGFA( std::vector< wmath::WUnitSphereCoordinates > const& orientations ) const
{
    double n = static_cast< double >( orientations.size() );
    double d = 0.0;
    double gfa = 0.0;
    double mean = 0.0;
    double v[ 15 ];

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

double WSymmetricSphericalHarmonic::calcGFA( wmath::WMatrix< double > const& B ) const
{
    // sh coeffs
    wmath::WMatrix< double > s( B.getNbCols(), 1 );
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

void WSymmetricSphericalHarmonic::applyFunkRadonTransformation( wmath::WMatrix< double > const& frtMat )
{
    WAssert( frtMat.getNbCols() == m_SHCoefficients.size(), "" );
    WAssert( frtMat.getNbRows() == m_SHCoefficients.size(), "" );
    // Funk-Radon-Transformation as in Descoteaux's thesis
    for ( size_t j = 0; j < m_SHCoefficients.size(); j++ )
    {
        m_SHCoefficients[ j ] *= frtMat( j, j );
    }
}

size_t WSymmetricSphericalHarmonic::getOrder() const
{
  return m_order;
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< wmath::WVector3D >& orientations,
                                                                        int order,
                                                                        double lambda,
                                                                        bool withFRT )
{
  // convert euclid 3d orientations/gradients to sphere coordinates
  std::vector< wmath::WUnitSphereCoordinates > sphereCoordinates;
  for ( std::vector< wmath::WVector3D >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
  {
    sphereCoordinates.push_back( wmath::WUnitSphereCoordinates( *it ) );
  }
  return WSymmetricSphericalHarmonic::getSHFittingMatrix( sphereCoordinates, order, lambda, withFRT );
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< wmath::WUnitSphereCoordinates >& orientations,
                                                                        int order,
                                                                        double lambda,
                                                                        bool withFRT )
{
  wmath::WMatrix<double> B( WSymmetricSphericalHarmonic::calcBaseMatrix( orientations, order ) );
  wmath::WMatrix<double> Bt( B.transposed() );
  wmath::WMatrix<double> result( Bt*B );
  if ( lambda != 0.0 )
  {
    wmath::WMatrix<double> L( WSymmetricSphericalHarmonic::calcSmoothingMatrix( order ) );
    result += lambda*L;
  }
  result = wmath::pseudoInverse( result )*Bt;
//   result *= Bt;
  if ( withFRT )
  {
    wmath::WMatrix<double> P( WSymmetricSphericalHarmonic::calcFRTMatrix( order ) );
    return ( P * result );
  }
  return result;
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcBaseMatrix( const std::vector< wmath::WUnitSphereCoordinates >& orientations,
                                                                    int order )
{
  wmath::WMatrix<double> B( orientations.size(), ( ( order + 1 ) * ( order + 2 ) ) / 2 );

  // calc B Matrix like in the 2007 Descoteaux paper ("Regularized, Fast, and Robust Analytical Q-Ball Imaging")
  int j = 0;
  const double rootOf2 = std::sqrt( 2.0 );
  for (uint32_t row = 0; row < orientations.size(); row++ )
  {
    const double theta = orientations[row].getTheta();
    const double phi = orientations[row].getPhi();
    for ( int k = 0; k <= order; k+=2 )
    {
      // m = 1 .. k
      for ( int m = 1; m <= k; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2 * static_cast<double>( std::pow( static_cast<double>( -1 ), m + 1 ) )
                        * boost::math::spherical_harmonic_i( k, m, theta, phi );
      }
      // m = 0
      B( row, ( k * k + k + 2 ) / 2 - 1 ) = boost::math::spherical_harmonic_r( k, 0, theta, phi );
      // m = -k .. -1
      for ( int m = -k; m < 0; m++ )
      {
        j = ( k * k + k + 2 ) / 2 + m;
        B( row, j-1 ) = rootOf2*boost::math::spherical_harmonic_r( k, -m, theta, phi );
      }
    }
  }
  return B;
}


wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcSmoothingMatrix( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    wmath::WMatrix<double> L( R, R );
    for ( size_t k = 0; k <= order; k += 2 )
    {
        for ( int m = -k; m <= static_cast< int >( k ); ++m )
        {
            L( i, i ) = static_cast< double > ( k * k * ( k + 1 ) * ( k + 1 ) );
            ++i;
        }
    }
    return L;
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcFRTMatrix( size_t order )
{
    size_t R = ( ( order + 1 ) * ( order + 2 ) ) / 2;
    std::size_t i = 0;
    wmath::WMatrix< double > result( R, R );
    for ( size_t k = 0; k <= order; k += 2 )
    {
        double h = 2.0 * wmath::piDouble * static_cast< double >( std::pow( static_cast< double >( -1 ), static_cast< double >( k / 2 ) ) ) *
                    static_cast< double >( wmath::oddFactorial( ( k <= 1 ) ? 1 : k - 1 ) ) / static_cast<double>( wmath::evenFactorial( k ) );
        for ( int m = -k; m <= static_cast< int >( k ); ++m )
        {
            result( i, i ) = h;
            ++i;
        }
    }
    return result;
}
} // Namespace WMath
