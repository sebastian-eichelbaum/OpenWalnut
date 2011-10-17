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

#ifndef WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H
#define WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <utility> // for std::pair
#include <vector>

#include <boost/math/special_functions/spherical_harmonic.hpp>
#include <boost/thread/thread.hpp>

#include "core/common/WLimits.h"
#include "core/common/WAssert.h"
#include "core/common/WProgress.h"
#include "core/common/WThreadedRunner.h"
#include "core/common/math/WMatrix.h"

#include "core/dataHandler/WDataSetRawHARDI.h"
#include "core/dataHandler/WDataSetSphericalHarmonics.h"
#include "core/dataHandler/WGridRegular3D.h"

// forward declaration
template< typename T = void >
class WSphericalHarmonicsCoefficientsThread;

/**
 * A specialization for void.
 */
template<>
class WSphericalHarmonicsCoefficientsThread< void >
{
public:

    /**
    * This structure is a collection of parameter and pointer to input and output data needed by each thread.
    */
    struct ThreadParameter
    {
        /**
         * Constructor, we need to initialize a reference.
         *
         * \param flag A reference to a shutdown flag that we should listen to.
         */
        ThreadParameter( WBoolFlag const& flag ) // NOLINT no explicit
            : m_shutdownFlag( flag )
        {
        }

        /**
         * Pointer to the HARDI measurements
         */
        boost::shared_ptr< WValueSetBase > m_valueSet;

        /**
         * Indices of nonzero gradients
         */
        std::vector< size_t > m_validIndices;

        /**
         * Indices of zero gradients (this should be normal T2 measurement)
         */
        std::vector< size_t > m_S0Indexes;

        /**
         * Output data, the spherical harmonics coefficients
         */
        boost::shared_ptr< std::vector<double> > m_data;

        /**
         * The order of the calculated spherical harmonics
         */
        int m_order;

        /**
         * Transformation-Matrix for conversion from HARDI measurements to spherical harmonics coefficients
         * (see Descoteaux dissertation)
         */
        boost::shared_ptr< WMatrix< double > > m_TransformMatrix;

        /**
         * Gradients of all measurements (including )
         */
        std::vector< WVector3d > m_gradients;

        /**
         * Pointer to progess indicator
         */
        boost::shared_ptr< WProgress > m_progress;

        /**
         * Indicate if the is error calculation is done.
         */
        bool m_doErrorCalculation;

        /**
         * Indicate if the Funk-Radon-Transformation is done.
         */
        bool m_doFunkRadonTransformation;

        /**
         * Indicate if the residuals will be calculated.
         * The residuals are the reprojection error. This means the
         */
        bool m_doResidualCalculation;

        /**
         * The stored residuals.
         */
        boost::shared_ptr< std::vector<double> > m_dataResiduals;

        /**
         * The b-value used during the creation of the HARDI-data.
         */
        double m_bDiffusionWeightingFactor;

        /**
         * Indicate if the normalisation from 0 to 1 is done.
         */
        bool m_normalize;

        /**
         * A shutdownFlag that may tell the thread to stop.
         */
        WBoolFlag const& m_shutdownFlag;
    };
};

/**
 * Module for the creation of a spherical harmonic data set from raw HARDI data.
 * \ingroup modules
 */
template< typename T >
class WSphericalHarmonicsCoefficientsThread : public WThreadedRunner
{
public:
    /**
     * The constructor.
     * \param parameter collection of parameter and so on (description in more detail above)
     * \param range the range (start and end) of input data this thread should use
     */
    WSphericalHarmonicsCoefficientsThread( WSphericalHarmonicsCoefficientsThread<>::ThreadParameter parameter, std::pair< size_t, size_t > range );

    /**
     * The main function of the thread. Here the calculation of the spherical harmonics coefficients is done.
     */
    void threadMain();

    /**
     * Returns the average error of the thread.
     *
     * \return error measure
     */
    double getError() const;

private:
    double m_overallError;                                                  //!< accumulated error
    unsigned int m_errorCount;                                              //!< number of accumulated errors, necessary for average error calculation
    WSphericalHarmonicsCoefficientsThread<>::ThreadParameter m_parameter;   //!< collection of parameter and so on (description in more detail above)
    std::pair< size_t, size_t > m_range;                                    //!< the range (start and end) of input data this thread use
};

template< typename T >
WSphericalHarmonicsCoefficientsThread< T >::WSphericalHarmonicsCoefficientsThread(
    WSphericalHarmonicsCoefficientsThread<>::ThreadParameter parameter,
    std::pair< size_t, size_t > range )
    : m_parameter( parameter ),
      m_range( range )
{
}

template< typename T >
void WSphericalHarmonicsCoefficientsThread< T >::threadMain()
{
    m_errorCount = 0;
    m_overallError = 0.0;

    for( size_t i = m_range.first; i < m_range.second; i++ )
    {
        if( m_parameter.m_shutdownFlag() )
        {
            break;
        }

        // get measure vector
        boost::shared_ptr< WValueSet< T > > vs = boost::shared_dynamic_cast< WValueSet< T > >( m_parameter.m_valueSet );
        if( !vs )
        {
            throw WException( "Valueset pointer not valid." );
        }

        WValue< T > allMeasures( vs->getWValue( i ) );

        // extract measures for gradients != 0
        WValue< double > measures( m_parameter.m_validIndices.size() );
        unsigned int idx = 0;

        // find max S0 value
        double S0avg = 0.0;
        for( std::vector< size_t >::const_iterator it = m_parameter.m_S0Indexes.begin(); it != m_parameter.m_S0Indexes.end(); it++ )
        {
            S0avg += static_cast< double >( allMeasures[ *it ] );
        }
        S0avg /= m_parameter.m_S0Indexes.size();

        double minVal = 1e99;
        double maxVal = -1e99;
        for( std::vector< size_t >::const_iterator it = m_parameter.m_validIndices.begin(); it != m_parameter.m_validIndices.end(); it++, idx++ )
        {
            measures[ idx ] = S0avg <= 0.0 ? 0.0 : static_cast< double >( allMeasures[ *it ] ) / S0avg;
            if( measures[ idx ] < minVal )
            {
                minVal = measures[ idx ];
            }
            if( measures[ idx ] > maxVal )
            {
                maxVal = measures[ idx ];
            }
        }

        WValue< double > coefficients( ( *m_parameter.m_TransformMatrix ) * measures );

        if( m_parameter.m_doResidualCalculation || m_parameter.m_doErrorCalculation )
        {
            WSymmetricSphericalHarmonic currentSphericalHarmonic( coefficients );
            for( idx = 0; idx < m_parameter.m_validIndices.size(); idx++ )
            {
                double error = static_cast< double >( measures[ idx ] )
                              - currentSphericalHarmonic.getValue( WUnitSphereCoordinates( m_parameter.m_gradients[ idx ] ) );

                if( m_parameter.m_doResidualCalculation )
                {
                    m_parameter.m_dataResiduals->operator[]( m_parameter.m_validIndices.size() * i + idx ) = error;
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
        size_t l = ( m_parameter.m_order + 1 ) * ( m_parameter.m_order + 2 ) / 2;

        // normalization
        double scale = 1.0;
        if( m_parameter.m_normalize )
        {
            scale *= std::sqrt( 4.0 * piDouble ) / coefficients[ 0 ];
        }

        for( std::size_t j = 0; j < l; j++ )
        {
            m_parameter.m_data->operator[]( l * i + j ) = coefficients[ j ];
        }
    }
}

template< typename T >
double WSphericalHarmonicsCoefficientsThread< T >::getError() const
{
    if( m_errorCount == 0 ) return 0.0;
    return m_overallError / static_cast< double >( m_errorCount );
}

#endif  // WSPHERICALHARMONICSCOEFFICIENTSTHREAD_H
