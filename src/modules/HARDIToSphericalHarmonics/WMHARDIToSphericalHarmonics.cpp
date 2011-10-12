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

#include "core/common/WLimits.h"
#include "core/common/WAssert.h"

#include "core/common/WProgress.h"
#include "core/common/math/WUnitSphereCoordinates.h"
#include "core/common/math/WMatrix.h"
#include "core/common/math/WSymmetricSphericalHarmonic.h"
#include "core/common/math/linearAlgebra/WVectorFixed.h"
#include "core/common/math/WLinearAlgebraFunctions.h"
#include "core/dataHandler/WDataHandler.h"
#include "core/dataHandler/WSubject.h"
#include "core/kernel/WKernel.h"

#include "core/graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"

#include "WSphericalHarmonicsCoefficientsThread.h"

#include "WMHARDIToSphericalHarmonics.h"
#include "WMHARDIToSphericalHarmonics.xpm"

W_LOADABLE_MODULE( WMHARDIToSphericalHarmonics )

WMHARDIToSphericalHarmonics::WMHARDIToSphericalHarmonics():
    WModule()
{
}

WMHARDIToSphericalHarmonics::~WMHARDIToSphericalHarmonics()
{
    removeConnectors();
}

boost::shared_ptr< WModule > WMHARDIToSphericalHarmonics::factory() const
{
    return boost::shared_ptr< WModule >( new WMHARDIToSphericalHarmonics() );
}

const char** WMHARDIToSphericalHarmonics::getXPMIcon() const
{
    return WMHARDIToSphericalHarmonics_xpm;
}

const std::string WMHARDIToSphericalHarmonics::getName() const
{
    return "Spherical Harmonic Calculator";
}

const std::string WMHARDIToSphericalHarmonics::getDescription() const
{
    return "This module creates a Spherical Harmonics data set from raw HARDI data ";
}

void WMHARDIToSphericalHarmonics::moduleMain()
{
    m_moduleState.setResetable( true, true );
    m_moduleState.add( m_input->getDataChangedCondition() );
    m_moduleState.add( m_propCondition );

    ready();
    debugLog() << "Module is now ready.";

    debugLog() << "Entering main loop";
    while( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetRawHARDI > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if( dataChanged && dataValid )
        {
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // calculate order from SHCoefficients.size:
            // - this is done by reversing the R=(l+1)*(l+2)/2 formula from the Descoteaux paper
            double q = std::sqrt( 0.25 + 2.0 * static_cast<double>( m_dataSet->getNumberOfMeasurements() ) ) - 1.5;
            m_order->setMax( static_cast<unsigned int>( q ) % 2 == 1 ? static_cast<unsigned int>( q ) - 3 : static_cast<unsigned int>( q ) - 2 );

            // reset to order 2 if the input hardi data does not allow for higher orders
            m_order->ensureValidity( 2 );
        }

//         if( dataChanged && dataValid )
        if( dataValid )
        {
            debugLog() << "Data changed. Recalculating output.";

            // Calculate new data
            boost::shared_ptr< WDataSetSphericalHarmonics > newData;

            // **********************************************
            // * Determine usable gradients and its indices *
            // **********************************************
            // store indices for S0 signal
            std::vector< size_t > S0Indexes;
            // determine a set of indices of which the gradient is not zero
            debugLog() << "Determine usable gradients." << std::endl;
            std::vector< size_t > validIndices;
            for( size_t i = 0; i < m_dataSet->getNumberOfMeasurements(); i++ )
            {
                const WVector3d& grad = m_dataSet->getGradient( i );
                if( ( grad[ 0 ] != 0.0 ) || ( grad[ 1 ] != 0.0 ) || ( grad[ 2 ] != 0.0 ) )
                {
                    validIndices.push_back( i );
                }
                else
                {
                    S0Indexes.push_back( i );
                }
            }
            debugLog() << "Found " << validIndices.size() << " usable gradients." << std::endl;
            debugLog() << "Found " << S0Indexes.size() << " zero gradients." << std::endl;

            if( S0Indexes.size() == 0 )
            {
                errorLog() << "No entry with zero gradient. Can't get S0 (basis) signal.";
                continue;
            }

            // build vector with gradients != 0
            std::vector< WVector3d > gradients;
            for( std::vector< size_t >::const_iterator it = validIndices.begin(); it != validIndices.end(); it++ )
            {
                gradients.push_back( m_dataSet->getGradient( *it ) );
            }
            int order = m_order->get( true );

            WMatrix< double > transformMatrix(
                      WSymmetricSphericalHarmonic::getSHFittingMatrix( gradients,
                                                                       order,
                                                                       m_regularisationFactorLambda->get( true ),
                                                                       m_doFunkRadonTransformation->get( true ) ) );

            WSphericalHarmonicsCoefficientsThread<>::ThreadParameter parameter( m_shutdownFlag );
            parameter.m_valueSet = m_dataSet->getValueSet();
            parameter.m_validIndices = validIndices;
            parameter.m_S0Indexes = S0Indexes;
            parameter.m_order = order;
            parameter.m_TransformMatrix = boost::shared_ptr< WMatrix< double > >( new WMatrix< double >( transformMatrix ) );
            parameter.m_gradients = gradients;
            parameter.m_doFunkRadonTransformation = m_doFunkRadonTransformation->get( true );
            parameter.m_doResidualCalculation = m_doResidualCalculation->get( true );
            parameter.m_doErrorCalculation = m_doErrorCalculation->get( true ) || parameter.m_doResidualCalculation;
            // parameter.m_regularisationFactorLambda = m_regularisationFactorLambda->get( true );
            parameter.m_bDiffusionWeightingFactor = m_dataSet->getDiffusionBValue();
            parameter.m_normalize = m_doNormalisation->get( true );

            //to show progess
            parameter.m_progress = boost::shared_ptr< WProgress >( new WProgress( "Creating Spherical Harmonics",
                                                                   m_dataSet->getValueSet()->size() ) );
            m_progress->addSubProgress( parameter.m_progress );

            debugLog() << "Starting calculation.";

            HARDICalculation hc( parameter, m_multiThreaded->get( true ), m_dataSet->getGrid(), gradients );
            HARDICalculation::result_type res = m_dataSet->getValueSet()->applyFunction( hc );

            debugLog() << "Got results.";

            // notify output about new data
            m_output->updateData( res.first );

            // create final output data
            if( parameter.m_doResidualCalculation )
            {
                m_outputResiduals->updateData( res.second );
            }
        }
    }

    debugLog() << "Finished.";
}

void WMHARDIToSphericalHarmonics::connectors()
{
    // initialize connectors
    m_input = boost::shared_ptr< WModuleInputData < WDataSetRawHARDI > >(
        new WModuleInputData< WDataSetRawHARDI >( shared_from_this(), "in", "Dataset to compute the spherical harmonics for." ) );

    // add it to the list of connectors. Please note, that a connector NOT added via addConnector will not work as expected.
    addConnector( m_input );

    m_output = boost::shared_ptr< WModuleOutputData< WDataSetSphericalHarmonics > >(
            new WModuleOutputData< WDataSetSphericalHarmonics >( shared_from_this(), "out", "The grid with the calculated spherical harmonics." ) );

    addConnector( m_output );

    m_outputResiduals = boost::shared_ptr< WModuleOutputData< WDataSetRawHARDI > >(
            new WModuleOutputData< WDataSetRawHARDI >( shared_from_this(), "residualsOut", "The residual of the reprojection." ) );

    addConnector( m_outputResiduals );

    // call WModules initialization
    WModule::connectors();
}

void WMHARDIToSphericalHarmonics::properties()
{
    m_propCondition = boost::shared_ptr< WCondition >( new WCondition() );

    m_order                     = m_properties->addProperty( "Order of Spherical Harmonics",
                                                             "Order of Spherical Harmonics",
                                                             4,
                                                             m_propCondition );

    m_doFunkRadonTransformation = m_properties->addProperty( "Funk-Radon-Transformation",
                                                             "Apply the Funk-Radon-Transformation to the calculated Spherical Harmonics.",
                                                             false,
                                                             m_propCondition );

    m_doErrorCalculation = m_properties->addProperty( "Error Calculation",
                                                      "Calculate the reprojection error of the spherical harmonics.",
                                                      false,
                                                      m_propCondition );

    m_doResidualCalculation = m_properties->addProperty( "Residual Calculation",
                                                         "Indicating whether the reprojection errors is stored into a separate dataset.",
                                                         false,
                                                         m_propCondition );

    m_regularisationFactorLambda = m_properties->addProperty( "Regularisation Factor Lambda",
                                                              "Factor for Regularisation",
                                                              0.0,
                                                              m_propCondition );

    m_regularisationFactorLambda->setMin( 0.0 );
    m_regularisationFactorLambda->setMax( 100000.0 );

    m_multiThreaded = m_properties->addProperty( "Multi-Threaded", "Calculate spherical harmonics in multiple threads.", true, m_propCondition );

    m_doNormalisation = m_properties->addProperty( "Normalizsation",
                                                   "Normalise the HARDI measurements to 0 to 1 for each voxel.",
                                                   false,
                                                   m_propCondition );

// vista-daten
    // order    Gesamtfehler
    // 0        8,7%
    // 1        8,7%
    // 2        6,5%
    // 3        6,5%
    // 4        5,8%
    // 5        5,8%
    // 6        4,9%
    // 8        3,3%

// nifty-daten
    // order    Gesamtfehler
    // 0
    // 2        inf
    // 4        5,8%
    // 6        4,9%
    // 8        3,3%

    m_order->setMin( 0 );
    m_order->setMax( 4 );
    m_order->addConstraint( boost::shared_ptr< evenInt >( new evenInt ) );
}

bool WMHARDIToSphericalHarmonics::evenInt::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >,
                                       WPVBaseTypes::PV_INT value )
{
    return ( value % 2 == 0 );
}

boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint > WMHARDIToSphericalHarmonics::evenInt::clone()
{
    return boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint >( new WMHARDIToSphericalHarmonics::evenInt( *this ) );
}

WMHARDIToSphericalHarmonics::HARDICalculation::HARDICalculation( WSphericalHarmonicsCoefficientsThread<>::ThreadParameter threadParams,
                                                                 bool multiThreaded, boost::shared_ptr< WGrid > grid,
                                                                 std::vector< WVector3d > const& gradients )
    : m_parameter( threadParams ),
      m_multiThreaded( multiThreaded ),
      m_grid( grid ),
      m_gradients( gradients )
{
}

WMHARDIToSphericalHarmonics::HARDICalculation::~HARDICalculation()
{
}
