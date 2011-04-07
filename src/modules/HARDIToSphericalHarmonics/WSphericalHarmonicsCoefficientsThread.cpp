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
#include "../../common/math/WVector3D.h"
#include "../../common/math/WMatrix.h"

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
    WValue< int16_t > allMeasures( m_parameter.m_valueSet->getWValue( i ) );

    // extract measures for gradients != 0
    WVector_2 measures( m_parameter.m_validIndices.size() );
    unsigned int idx = 0;

    // find max S0 value
    double S0avg = 0.0;
    for ( std::vector< size_t >::const_iterator it = m_parameter.m_S0Indexes.begin(); it != m_parameter.m_S0Indexes.end(); it++ )
    {
        S0avg += allMeasures[ *it ];
    }
    S0avg /= m_parameter.m_S0Indexes.size();

    double minVal = 1e99;
    double maxVal = -1e99;
    for ( std::vector< size_t >::const_iterator it = m_parameter.m_validIndices.begin(); it != m_parameter.m_validIndices.end(); it++, idx++ )
    {
        measures[ idx ] = S0avg <= 0.0 ? 0.0 :
            static_cast<double>( allMeasures[ *it ] ) / S0avg;

        if ( measures[ idx ] < minVal ) minVal = measures[ idx ];
        if ( measures[ idx ] > maxVal ) maxVal = measures[ idx ];
    }

    WVector_2 coefficients( ( *m_parameter.m_TransformMatrix ) * measures );

    if ( m_parameter.m_doResidualCalculation || m_parameter.m_doErrorCalculation )
    {
        WSymmetricSphericalHarmonic currentSphericalHarmonic( coefficients );
        for ( idx = 0; idx < m_parameter.m_validIndices.size(); idx++ )
        {
            double error = static_cast< double >( measures[ idx ] )
                          - currentSphericalHarmonic.getValue( WUnitSphereCoordinates( m_parameter.m_gradients[ idx ] ) );

            if( m_parameter.m_doResidualCalculation )
            {
                m_parameter.m_dataResiduals->operator[]( m_parameter.m_validIndices.size()*i+idx ) = error;
            }
            if( m_parameter.m_doErrorCalculation )
            {
                m_overallError += fabs( error );
                m_errorCount++;
            }
        }
    }

    // show progress
    ++( *( m_parameter.m_progress ) );

    // copy coefficients to output "data"
    size_t l = ( m_parameter.m_order+1 )*( m_parameter.m_order+2 ) / 2;

    // normalization
    double scale = 1.0;
    if( m_parameter.m_normalize )
    {
        scale *= std::sqrt( 4.0 * piDouble ) / coefficients[ 0 ];
    }

    for ( size_t j = 0; j < l; j++ )
    {
        m_parameter.m_data->operator[]( l*i + j ) = coefficients[ j ];
    }
  }
}

double WSphericalHarmonicsCoefficientsThread::getError() const
{
  if ( m_errorCount == 0 ) return 0.0;
  return m_overallError / static_cast<double>( m_errorCount );
}
