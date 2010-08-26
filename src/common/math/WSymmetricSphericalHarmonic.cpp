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

#include <boost/math/special_functions/spherical_harmonic.hpp>

#include "stdint.h"

#include "WLinearAlgebraFunctions.h"
#include "WUnitSphereCoordinates.h"
#include "WMatrix.h"
#include "WValue.h"

#include "WSymmetricSphericalHarmonic.h"

namespace wmath
{
std::vector<size_t> wmath::WSymmetricSphericalHarmonic::m_lj;

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
  if ( WSymmetricSphericalHarmonic::m_lj.size() < m_SHCoefficients.size() ) WSymmetricSphericalHarmonic::calcLj( m_order );
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
//   old implementation
//   double result = 0.0;
//   int j = 0;
//   for ( int k = 0; k <= static_cast<int>( m_order ); k+=2 )
//   {
//     // m = 1 .. k
//     for ( int m = 1; m <= k; m++ )
//     {
//       j = ( k*k+k+2 ) / 2 + m;
//       result += m_SHCoefficients[ j-1 ] * std::sqrt( 2.0 ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
//     }
//     // m = 0
//     result += m_SHCoefficients[ ( k*k+k+2 ) / 2 - 1 ] * boost::math::spherical_harmonic_r( k, 0, theta, phi );
//     // m = -k .. -1
//     for ( int m = -k; m < 0; m++ )
//     {
//       j = ( k*k+k+2 ) / 2 + m;
//       result += m_SHCoefficients[ j-1 ] * std::sqrt( 2.0 ) * boost::math::spherical_harmonic_r( k, m, theta, phi );
//     }
//   }
//   return result;
}

double WSymmetricSphericalHarmonic::getValue( const WUnitSphereCoordinates& coordinates ) const
{
  return getValue( coordinates.getTheta(), coordinates.getPhi() );
}

const wmath::WValue<double>& WSymmetricSphericalHarmonic::getCoefficients() const
{
  return m_SHCoefficients;
}

void WSymmetricSphericalHarmonic::applyFunkRadonTransformation()
{
  // Funk-Radon-Transformation like in the 2007 Descoteaux paper
  // TODO(philips): implement
  size_t lj = 0;
//   double ljDbl = 0.0;
  double factor = 0.0;
  for ( size_t j = 0; j < m_SHCoefficients.size(); j++ )
  {
//     lj = 2 * ( ( j == 1 ) ? 1 : ( j+2 )/4  );
    lj = WSymmetricSphericalHarmonic::m_lj[j];
//     ljDbl = static_cast<double>( lj );
//     std::cerr << "j: " << j << " lj: " << ljDbl << std::endl;
    // lj is always even!
    factor = 2.0 * wmath::piDouble * static_cast<double>( std::pow( static_cast<double>( -1.0 ), static_cast<double>( lj / 2 ) ) ) *
                  static_cast<double>( wmath::oddFactorial( ( lj <= 1 ) ? 1 : lj-1 ) ) / static_cast<double>( wmath::evenFactorial( lj ) );
// the following line is the (wrong) first calculation
//                   static_cast<double>( wmath::oddFactorial( ( lj == 1 ) ? lj-1 : 1 ) ) / static_cast<double>( wmath::evenFactorial( lj ) );

//     factor = (double) std::pow( -1, ( lj / 2 ) ) * (double)wmath::oddFaculty( lj ) / (double)wmath::evenFaculty( lj );
//     std::cerr << "factor: " << factor << std::endl;
    m_SHCoefficients[ j ] = m_SHCoefficients[ j ] * factor;
  }
}

size_t WSymmetricSphericalHarmonic::getOrder() const
{
  return m_order;
}

void WSymmetricSphericalHarmonic::calcLj( size_t order )
{
  m_lj.clear();
  for ( size_t k=0; k<=order; k+=2 )
  {
    for ( int m=-k; m<=static_cast<int>( k ); m++ )
    {
      m_lj.push_back( k );
    }
  }
}


wmath::WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< wmath::WVector3D >& orientations, int order, double lambda, bool withFRT )
{
  // convert euclid 3d orientations/gradients to sphere coordinates
  std::vector< wmath::WUnitSphereCoordinates > sphereCoordinates;
  for ( std::vector< wmath::WVector3D >::const_iterator it = orientations.begin(); it != orientations.end(); it++ )
  {
    sphereCoordinates.push_back( wmath::WUnitSphereCoordinates( *it ) );
  }
  return WSymmetricSphericalHarmonic::getSHFittingMatrix( sphereCoordinates, order, lambda, withFRT );
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::getSHFittingMatrix( const std::vector< wmath::WUnitSphereCoordinates >& orientations, int order, double lambda, bool withFRT )
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

wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcBaseMatrix( const std::vector< wmath::WUnitSphereCoordinates >& orientations, int order )
{
  wmath::WMatrix<double> B( orientations.size(), ((order+1)*(order+2))/2 );

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
        j = ( k*k+k+2 ) / 2 + m;
        B( row, j-1 ) = rootOf2 * static_cast<double>( std::pow( static_cast<double>( -1 ), m+1 ) ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
      }
      // m = 0
      B( row, (k*k+k+2)/2-1 ) = boost::math::spherical_harmonic_r( k, 0, theta, phi );
      // m = -k .. -1
      for ( int m = -k; m < 0; m++ )
      {
        j = (k*k+k+2)/2+m;
        B( row, j-1 ) = rootOf2*boost::math::spherical_harmonic_r( k, -m, theta, phi );
      }
    }
  }
  return B;
//   old implementation
//   wmath::WMatrix<double> B( orientations.size(), ((order+1)*(order+2))/2 );
// 
//   // calc B Matrix like in the 2007 Descoteaux paper ("Regularized, Fast, and Robust Analytical Q-Ball Imaging")
//   int j = 0;
//   for (uint row = 0; row < orientations.size(); row++ )
//   {
//     const double theta = orientations[row].getTheta();
//     const double phi = orientations[row].getPhi();
//     for ( int k = 0; k <= order; k+=2 )
//     {
//       // m = 1 .. k
//       for ( int m = 1; m <= k; m++ )
//       {
//         j = (k*k+k+2)/2+m;
//         B( row, j-1 ) = std::sqrt(2.0)*boost::math::spherical_harmonic_i( k, m, theta, phi );
//       }
//       // m = 0
//       B( row, (k*k+k+2)/2-1 ) = boost::math::spherical_harmonic_r( k, 0, theta, phi );
//       // m = -k .. -1
//       for ( int m = -k; m < 0; m++ )
//       {
//         j = (k*k+k+2)/2+m;
//         B( row, j-1 ) = std::sqrt(2.0)*boost::math::spherical_harmonic_r( k, m, theta, phi );
//       }
//     }
//   }
//   return B;
}


wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcSmoothingMatrix( size_t order )
{
  std::vector<int> lj;
  lj.clear();
  for ( size_t k=0; k<=order; k+=2 )
  {
    for ( int m=-k; m<=static_cast<int>( k ); m++ )
    {
      lj.push_back( k );
    }
  }
  size_t R = ( (order+1) * (order+2) ) / 2;
  wmath::WMatrix<double> L( R, R );
  for ( size_t i = 0; i < R; i++ ) L( i, i ) = static_cast<double> ( lj[i] * lj[i] * ( lj[i] + 1) * ( lj[i] + 1) );
  return L;
}

wmath::WMatrix<double> WSymmetricSphericalHarmonic::calcFRTMatrix( size_t order )
{
  size_t dim = ( order + 1 ) * ( order + 2 ) / 2;
  if ( WSymmetricSphericalHarmonic::m_lj.size() < dim ) WSymmetricSphericalHarmonic::calcLj( order );
  
  wmath::WMatrix<double> result( dim, dim );
  for ( size_t j = 0; j < dim; j++ )
  {
    size_t lj = WSymmetricSphericalHarmonic::m_lj[j];
    // lj is always even!
    result( j, j ) = 2.0 * wmath::piDouble * static_cast<double>( std::pow( static_cast<double>( -1 ), static_cast<double>( lj / 2 ) ) ) *
                  static_cast<double>( wmath::oddFactorial( ( lj <= 1 ) ? 1 : lj-1 ) ) / static_cast<double>( wmath::evenFactorial( lj ) );
  }
  return result;
}
} // Namespace WMath
