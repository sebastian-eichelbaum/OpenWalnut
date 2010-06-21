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
  for ( int k = 0; k <= static_cast<int>( m_order ); k+=2 )
  {
    // m = 1 .. k
    for ( int m = 1; m <= k; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * std::sqrt( 2.0 ) * boost::math::spherical_harmonic_i( k, m, theta, phi );
    }
    // m = 0
    result += m_SHCoefficients[ ( k*k+k+2 ) / 2 - 1 ]*boost::math::spherical_harmonic_r( k, 0, theta, phi );
    // m = -k .. -1
    for ( int m = -k; m < 0; m++ )
    {
      j = ( k*k+k+2 ) / 2 + m;
      result += m_SHCoefficients[ j-1 ] * std::sqrt( 2.0 ) * boost::math::spherical_harmonic_r( k, m, theta, phi );
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

void WSymmetricSphericalHarmonic::applyFunkRadonTransformation()
{
  // Funk-Radon-Transformation like in the 2007 Descoteaux paper
  // TODO(philips): implement
  WAssert( false, "implement!" );
  int lj = 0.0;
  double factor = 0.0;
  for ( size_t j = 0; j < 2*m_SHCoefficients.size(); j++ )
  {
    lj = 2 * ( ( j == 1 ) ? 0 : ( size_t )( j+2 )/4  );
    // lj is always even!
    factor = 2.0 * wmath::piDouble * static_cast<double>( std::pow( -1.0, ( lj / 2.0 ) ) ) *
                  static_cast<double>( wmath::oddFactorial( lj ) ) / static_cast<double>( wmath::evenFactorial( lj ) );
//     factor = (double) std::pow( -1, ( lj / 2 ) ) * (double)wmath::oddFaculty( lj ) / (double)wmath::evenFaculty( lj );
//     std::cerr << "factor: " << factor << std::endl;
//     m_SHCoefficients[ j ] = m_SHCoefficients[ j ] * factor;
  }
}

size_t WSymmetricSphericalHarmonic::getOrder() const
{
  return m_order;
}
}
