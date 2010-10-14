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

#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <utility> // for std::pair
#include <vector>

#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <boost/thread/thread.hpp>

#include "HARDIToSphericalHarmonics.xpm"
#include "../../common/WLimits.h"
#include "../../common/WAssert.h"

#include "WSphericalHarmonicsCoefficientsThread.h"

WSphericalHarmonicsCoefficientsThread::WSphericalHarmonicsCoefficientsThread(
    WThreadParameter parameter,
    std::pair< size_t, size_t > range ) :
  m_parameter( parameter ),
  m_range( range )
{
}

void WSphericalHarmonicsCoefficientsThread::threadMain()
{
  m_errorCount = 0;
  m_overallError = 0.0;
  for ( size_t i = m_range.first; i < m_range.second; i++ )
  {
    if ( m_shutdownFlag.get( true ) ) break;
    // get measure vector
    wmath::WValue< int16_t > allMeasures( m_parameter.m_valueSet->getWValue( i ) );

    // extract measures for gradients != 0
    wmath::WValue< double > measures( m_parameter.m_validIndices.size() );
    unsigned int idx = 0;

    // find max S0 value
    double S0max = 1.0;
    for ( std::vector< size_t >::const_iterator it = m_parameter.m_S0Indexes.begin(); it != m_parameter.m_S0Indexes.end(); it++ )
    {
      if ( S0max < allMeasures[ *it ] ) S0max = allMeasures[ *it ];
    }
//       if ( !m_parameter.m_S0Indexes.empty() && ( allMeasures[ m_parameter.m_S0Indexes[0] ] != 0 ) )
//       {
//         for ( size_t j = 0; j < m_parameter.m_S0Indexes.size(); j++ )
//         {
//           std::cerr << " " << allMeasures[ m_parameter.m_S0Indexes[ j ] ];
//         }
//         std::cerr << ": ";
//         for ( size_t j = 0; j < m_parameter.m_validIndices.size(); j++ )
//         {
//           std::cerr << " " << allMeasures[ m_parameter.m_validIndices[ j ] ];
//         }
//         std::cerr << std::endl;
//       }
    // Normalisation between 0 to 1
    if ( m_parameter.m_normalize )
    {
      double minVal = 1e99;
      double maxVal = -1e99;
      for ( std::vector< size_t >::const_iterator it = m_parameter.m_validIndices.begin(); it != m_parameter.m_validIndices.end(); it++, idx++ )
      {
        measures[ idx ] = allMeasures[ m_parameter.m_S0Indexes[0] ] == 0.0 ? 0.0 :
                          static_cast<double>( allMeasures[ *it ] ) / allMeasures[ m_parameter.m_S0Indexes[0] ];

        if ( measures[ idx ] < minVal ) minVal = measures[ idx ];
        if ( measures[ idx ] > maxVal ) maxVal = measures[ idx ];
      }

      for ( size_t i = 0; i < measures.size(); i++ )
      {
        measures[ i ] -= minVal;
        measures[ i ] /= ( ( maxVal-minVal ) == 0.0 ) ? 1.0 : ( maxVal-minVal );
      }
    }
    else
    {
      for ( std::vector< size_t >::const_iterator it = m_parameter.m_validIndices.begin(); it != m_parameter.m_validIndices.end(); it++, idx++ )
      {
        measures[ idx ] = allMeasures[ m_parameter.m_S0Indexes[0] ] == 0.0 ? 0.0 :
                          static_cast<double>( allMeasures[ *it ] ) / allMeasures[ m_parameter.m_S0Indexes[0] ];
      }
    }

    wmath::WValue< double > coefficients( ( *m_parameter.m_TransformMatrix ) * measures );

    if ( m_parameter.m_doErrorCalculation )
    {
      wmath::WSymmetricSphericalHarmonic currentSphericalHarmonic( coefficients );
      double S0 = allMeasures[ m_parameter.m_S0Indexes[0] ];
      if ( S0 != 0.0 )
      {
        for ( idx = 0; idx < m_parameter.m_validIndices.size(); idx++ )
        {
          WAssert( measures[ idx ] <= 1.0, "" );
          double error = static_cast<double>( measures[ idx ] )
                          - currentSphericalHarmonic.getValue( wmath::WUnitSphereCoordinates( m_parameter.m_gradients[ idx ] ) );
//           WAssert( 0.0 < currentSphericalHarmonic.getValue( wmath::WUnitSphereCoordinates( m_parameter.m_gradients[ idx ] ) ) && WAssert(
//             std::cerr << error << std::endl;

          if ( m_parameter.m_doResidualCalculation )
          {
              m_parameter.m_dataResiduals->operator[]( m_parameter.m_validIndices.size()*i+idx ) = error;
          }

          error = std::fabs( error );
          if ( error != 0.0 )
          {
            WAssert( !isnan( error ), "Error is NaN." );
//             m_overallError += error/measures[ idx ];
//             WAssert( S0 != 0.0, "S0 is zero." );
//             m_overallError += error/ ( measures[ idx ] / S0 );
            m_overallError += error;
            m_errorCount++;
          }
        }
      }
    }

    // show progress
    ++( *( m_parameter.m_progress ) );

    // copy coefficients to output "data"
    size_t l = ( m_parameter.m_order+1 )*( m_parameter.m_order+2 ) / 2;
    for ( size_t j = 0; j < l; j++ ) m_parameter.m_data->operator[]( l*i + j ) = coefficients[ j ];
  }
}

double WSphericalHarmonicsCoefficientsThread::getError() const
{
  if ( m_errorCount == 0 ) return 0.0;
  return m_overallError / static_cast<double>( m_errorCount );
}
