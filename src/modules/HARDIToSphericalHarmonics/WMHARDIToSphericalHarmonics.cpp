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

#include "../../common/WProgress.h"
#include "../../common/WPreferences.h"
#include "../../common/math/WUnitSphereCoordinates.h"
#include "../../common/math/WMatrix.h"
#include "../../common/math/WSymmetricSphericalHarmonic.h"
#include "../../common/math/linearAlgebra/WLinearAlgebra.h"
#include "../../common/math/WLinearAlgebraFunctions.h"
#include "../../dataHandler/WDataHandler.h"
#include "../../dataHandler/WSubject.h"
#include "../../kernel/WKernel.h"

#include "../../graphicsEngine/algorithms/WMarchingCubesAlgorithm.h"

#include "WSphericalHarmonicsCoefficientsThread.h"

#include "WMHARDIToSphericalHarmonics.h"

// This line is needed by the module loader to actually find your module.
W_LOADABLE_MODULE( WMHARDIToSphericalHarmonics )

WMHARDIToSphericalHarmonics::WMHARDIToSphericalHarmonics():
    WModule()
{
    // WARNING: initializing connectors inside the constructor will lead to an exception.
    // Implement WModule::initializeConnectors instead.
}

WMHARDIToSphericalHarmonics::~WMHARDIToSphericalHarmonics()
{
    // cleanup
    removeConnectors();
}

boost::shared_ptr< WModule > WMHARDIToSphericalHarmonics::factory() const
{
    return boost::shared_ptr< WModule >( new WMHARDIToSphericalHarmonics() );
}

const char** WMHARDIToSphericalHarmonics::getXPMIcon() const
{
    return HARDIToSphericalHarmonics_xpm;
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
    while ( !m_shutdownFlag() )
    {
        debugLog() << "Waiting ...";
        m_moduleState.wait();

        if ( m_shutdownFlag() )
        {
            break;
        }

        boost::shared_ptr< WDataSetRawHARDI > newDataSet = m_input->getData();
        bool dataChanged = ( m_dataSet != newDataSet );
        bool dataValid   = ( newDataSet );

        if ( dataChanged && dataValid )
        // this condition will become true whenever the new data is different from the current one or our actual data is NULL. This handles all
        // cases.
        {
            // The data is different. Copy it to our internal data variable:
            debugLog() << "Received Data.";
            m_dataSet = newDataSet;

            // calculate order from SHCoefficients.size:
            // - this is done by reversing the R=(l+1)*(l+2)/2 formula from the Descoteaux paper
            double q = std::sqrt( 0.25 + 2.0 * static_cast<double>( m_dataSet->getNumberOfMeasurements() ) ) - 1.5;
            m_order->setMax( static_cast<unsigned int>( q ) % 2 == 1 ? static_cast<unsigned int>( q ) - 3 : static_cast<unsigned int>( q ) - 2 );
            m_order->set( m_order->getMax()->getMax() );
            // TODO(philips): check max order calculation
            m_order->set( 2 );
        }

//         if ( dataChanged && dataValid )
        if ( dataValid )
        {
            debugLog() << "Data changed. Recalculating output.";

            // Calculate new data
            boost::shared_ptr< WDataSetSphericalHarmonics > newData;

            // **********************************************
            // * Determine usable gradients and its indices *
            // **********************************************
            // store indexes for S0 signal
            std::vector< size_t > S0Indexes;
            // determine a set of indices of which the gradient is not zero
            debugLog() << "Determine usable gradients." << std::endl;
            std::vector< size_t > validIndices;
            for ( size_t i = 0; i < m_dataSet->getNumberOfMeasurements(); i++ )
            {
              const WVector3d& grad = m_dataSet->getGradient( i );
              if ( (grad[0] != 0.0) || (grad[1] != 0.0) || (grad[2] != 0.0) )
                validIndices.push_back( i );
              else
                S0Indexes.push_back( i );
            }
            debugLog() << "Found " << validIndices.size() << " usable gradients." << std::endl;
            debugLog() << "Found " << S0Indexes.size() << " zero gradients." << std::endl;
// TODO(philips): check if the follow assert is necessary
//             WAssert( S0Index != -1, "No entry with zero gradient. Can't get S0 (basis) signal." );

            // build vector with gradients != 0
            std::vector< WVector3d > gradients;
            for ( std::vector< size_t >::const_iterator it = validIndices.begin(); it != validIndices.end(); it++ )
              gradients.push_back( m_dataSet->getGradient( *it ) );

            int order  = m_order->get( true );

            WMatrix_2 TransformMatrix(
                WSymmetricSphericalHarmonic::getSHFittingMatrix( gradients,
                                                                 order,
                                                                 m_regularisationFactorLambda->get( true ),
                                                                 m_doFunkRadonTransformation->get( true ) ) );

            double overallError = 0.0;
            boost::shared_ptr< WValueSet< int16_t > > valueSet = boost::shared_dynamic_cast< WValueSet< int16_t > >( m_dataSet->getValueSet() );
            WAssert( valueSet, "WValueSetBase of the HARDI data is not of type WValueSet< int16_t >" );
            // to store the spherical harmonics
            int dimension = ( order+1 ) * ( order + 2 ) / 2;
            size_t voxelCount = valueSet->size();

            // data vector stores spherical harmonics coefficients
            boost::shared_ptr< std::vector<double> > data
                = boost::shared_ptr< std::vector<double> >( new std::vector<double>( voxelCount * dimension ) );

            //to show progess
            boost::shared_ptr< WProgress > progress
                = boost::shared_ptr< WProgress >( new WProgress( "Creating Spherical Harmonics", valueSet->size() ) );

            m_progress->addSubProgress( progress );

            debugLog() << "Creating Spherical Harmonics ... ";
            const unsigned int threadCount = m_multiThreaded->get( true ) ?
                                     ( boost::thread::hardware_concurrency() == 0 ? 1 : boost::thread::hardware_concurrency() ) : 1;
//             const unsigned int threadCount = 1;

            // set thread parameter
            WSphericalHarmonicsCoefficientsThread::WThreadParameter parameter;
            parameter.m_valueSet = valueSet;
            parameter.m_validIndices = validIndices;
            parameter.m_S0Indexes = S0Indexes;
            parameter.m_data = data;
            parameter.m_order = order;
            parameter.m_TransformMatrix = boost::shared_ptr< WMatrix_2 >( new WMatrix_2( TransformMatrix ) );
            parameter.m_gradients = gradients;
            parameter.m_progress = progress;
            parameter.m_doFunkRadonTransformation = m_doFunkRadonTransformation->get( true );
            parameter.m_doResidualCalculation = m_doResidualCalculation->get( true );
            parameter.m_doErrorCalculation = m_doErrorCalculation->get( true ) || parameter.m_doResidualCalculation;
//             parameter.m_regularisationFactorLambda = m_regularisationFactorLambda->get( true );
            parameter.m_bDiffusionWeightingFactor = m_dataSet->getDiffusionBValue();
            parameter.m_normalize = m_doNormalisation->get( true );

            // data vector to store reprojection residuals
            if ( parameter.m_doResidualCalculation )
            {
              parameter.m_dataResiduals = boost::shared_ptr< std::vector<double> >(
                                new std::vector<double>( m_dataSet->getValueSet()->size() * parameter.m_validIndices.size() ) );
            }

//             const unsigned int threadCount = 1;
            std::pair< size_t, size_t > range;
            // create Threads
            for ( unsigned int i = 0; i < threadCount; i++ )
            {
              range.first = ( voxelCount / threadCount ) * i;
              range.second = ( i == ( threadCount-1 ) ) ? voxelCount : ( voxelCount / threadCount ) * ( i+1 );
              m_threads.push_back( new WSphericalHarmonicsCoefficientsThread( parameter, range ) );
              m_threads.back()->run();
            }

            debugLog() << "Calculation started ... waiting for finish";
            for ( unsigned int i = 0; i < m_threads.size(); i++ )
            {
              m_threads[ i ]->wait();
              overallError += m_threads[ i ]->getError();
              debugLog() << "Delete Thread";
              delete m_threads[ i ];
            }

            overallError /= static_cast<double>( m_threads.size() );
            m_threads.clear();
            progress->finish();
            debugLog() << "Done" << std::endl;
            debugLog() << "Gesamtfehler: " << overallError;

            // create final output data
            boost::shared_ptr< WValueSet<double> > sphericalHarmonicsData
                    = boost::shared_ptr< WValueSet<double> >( new WValueSet<double>( 1, dimension, data, W_DT_DOUBLE ) );

            newData = boost::shared_ptr< WDataSetSphericalHarmonics >(
                        new WDataSetSphericalHarmonics( sphericalHarmonicsData, m_dataSet->getGrid() ) );

            // notify output about new data
            m_output->updateData( newData );

            // create final output data
            if ( parameter.m_doResidualCalculation )
            {
                boost::shared_ptr< WValueSet<double> > residualsData = boost::shared_ptr< WValueSet<double> >(
                        new WValueSet<double>( 1, parameter.m_validIndices.size(), parameter.m_dataResiduals, W_DT_DOUBLE ) );

                boost::shared_ptr< WDataSetRawHARDI > newResidualData =
                          boost::shared_ptr< WDataSetRawHARDI >( new WDataSetRawHARDI( residualsData, m_dataSet->getGrid(),
                                                                                       boost::shared_ptr< std::vector< WVector3d > >(
                                                                                           new std::vector< WVector3d >( gradients ) ) ) );
                m_outputResiduals->updateData( newResidualData );
            }
        }
    }
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
                                                         "Indicating whether the reprojection errors is stored into a seperate dataset.",
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
    m_order->setMax( 0 );
    m_order->addConstraint( boost::shared_ptr< evenInt >( new evenInt ) );

    m_propCondition->subscribeSignal( boost::bind( &WMHARDIToSphericalHarmonics::stopThreads, this ) );
}


void WMHARDIToSphericalHarmonics::stopThreads()
{
  for ( std::vector< WSphericalHarmonicsCoefficientsThread* >::iterator it = m_threads.begin(); it != m_threads.end(); it++ )
  {
    ( *it )->requestStop();
  }
}

bool WMHARDIToSphericalHarmonics::evenInt::accept( boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT > >,
                                       WPVBaseTypes::PV_INT value )
{
    return ( value % 2 == 0 );
}

boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint > WMHARDIToSphericalHarmonics::evenInt::clone()
{
    // If you write your own constraints, you NEED to provide a clone function. It creates a new copied instance of the constraints with the
    // correct runtime type.
    return boost::shared_ptr< WPropertyVariable< WPVBaseTypes::PV_INT >::PropertyConstraint >( new WMHARDIToSphericalHarmonics::evenInt( *this ) );
}
