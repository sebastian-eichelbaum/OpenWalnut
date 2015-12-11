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

#include <iostream>
#include <vector>

#include <boost/random.hpp>

#include <core/common/exceptions/WPreconditionNotMet.h>

#include "WTuringPatternCreator.h"

WTuringPatternCreator::WTuringPatternCreator( boost::shared_ptr< WProgress > const progress, std::size_t numThreads )
    : m_numThreads( numThreads ),
      m_numIterations( 100 ),
      m_spotIrregularity( 0.1 ),
      m_spotSize( 0.1 ),
      m_progress( progress )
{
    WPrecond( numThreads > 0, "" );
}

WTuringPatternCreator::~WTuringPatternCreator()
{
}

void WTuringPatternCreator::setNumIterations( std::size_t iter )
{
    WPrecond( iter > 0, "Invalid number of iterations!" );

    m_numIterations = iter;
}

void WTuringPatternCreator::setSpotIrregularity( float irr )
{
    WPrecond( irr >= 0.0f && irr <= 1.0f, "Spot irregularity must be in [0,1]!" );

    m_spotIrregularity = irr;
}

void WTuringPatternCreator::setSpotSize( float size )
{
    WPrecond( size >= 0.0f && size <= 1.0f, "Spot size must be in [0,1]!" );

    m_spotSize = size;
}

boost::shared_ptr< std::vector< float > > WTuringPatternCreator::create( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ )
{
    // some constants, maybe change to parameters
    float const spotFactor = ( 0.02f + 0.58f * ( 1.0f - m_spotSize ) ) / 15.0f;
    float const d1 = 0.125f;
    float const d2 = 0.03125f;
    float const speed = 1.0f;

    boost::shared_ptr< std::vector< float > > concentration1( new std::vector< float >( sizeX * sizeY * sizeZ, 4.0f ) );
    std::vector< float > concentration2( sizeX * sizeY * sizeZ, 4.0f );
    std::vector< float > delta1( sizeX * sizeY * sizeZ, 0.0f );
    std::vector< float > delta2( sizeX * sizeY * sizeZ, 0.0f );
    std::vector< float > noise( sizeX * sizeY * sizeZ );

    boost::mt19937 generator( std::time( 0 ) );

    float noiseRange = 0.1f + 4.9f * m_spotIrregularity;

    boost::uniform_real< float > dist( 12.0f - noiseRange, 12.0f + noiseRange );
    boost::variate_generator< boost::mt19937&, boost::uniform_real< float > > rand( generator, dist );

    // initialize noise
    for( std::size_t k = 0; k < sizeX * sizeY * sizeZ; ++k )
    {
        noise[ k ] = rand();
    }

    // The threads will notify this condition when they are all done with their calculations for one iteration.
    boost::shared_ptr< WConditionSet > continueCondition( new WConditionSet() );
    continueCondition->setResetable( true, true );

    // We notify this condition to wake all threads and make them calculate for the next iteration.
    boost::shared_ptr< WConditionSet > waitCondition( new WConditionSet() );
    continueCondition->setResetable( true, true );

    // The threads use this counter to find out whether all of them have finished for the current iteration.
    WCounter counter;

    std::vector< boost::shared_ptr< PatternThread > > threads;

    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        threads.push_back( boost::shared_ptr< PatternThread >( new PatternThread( k, m_numThreads, continueCondition, waitCondition, &counter ) ) );
        threads[ k ]->setDomainSize( sizeX, sizeY, sizeZ );
        threads[ k ]->setSpotFactor( spotFactor );
        threads[ k ]->setDiffusionConstants( d1, d2 );
        threads[ k ]->setBufferPointers( concentration1.get(), &concentration2, &noise, &delta1, &delta2 );
        threads[ k ]->run();
    }

    // A short sleep to give the threads time to initialize themselves.
    boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

    // Progress is from 0 to number of voxels. We want it to show the percent of iterations done, though.
    double stepsPerIteration = sizeX * sizeY * sizeZ / m_numIterations;
    std::size_t currentStep = 0;

    // iterate
    for( std::size_t iter = 0; iter < m_numIterations; ++iter )
    {
        // let the threads calculate their parts of the simulation
        waitCondition->notify();  // notify the threads to start
        continueCondition->wait();  // wait for them to finish

        // applying the change in concentration is not too costly, we do it here
        for( std::size_t k = 0; k < sizeX * sizeY * sizeZ; ++k )
        {
            ( *concentration1 )[ k ] += speed * delta1[ k ];
            concentration2[ k ] += speed * delta2[ k ];
        }

        // update progress
        std::size_t newStep = static_cast< std::size_t >( iter * stepsPerIteration ) + 1;
        m_progress->increment( newStep - currentStep );
        currentStep = newStep;
    }

    for( std::size_t k = 0; k < m_numThreads; ++k )
    {
        threads[ k ]->finish();
    }
    waitCondition->notify();

    // Give the threads time to swedish.
    boost::this_thread::sleep( boost::posix_time::seconds( 1 ) );

    return concentration1;
}

WTuringPatternCreator::PatternThread::PatternThread( std::size_t id, std::size_t max,
                                                     boost::shared_ptr< WCondition > const mainThreadContinueCondition,
                                                     boost::shared_ptr< WCondition > const waitForMainThreadCondition,
                                                     WCounter* const counter )
    : WThreadedRunner(),
      m_id( id ),
      m_maxThreads( max ),
      m_sizeX( 2 ),
      m_sizeY( 2 ),
      m_sizeZ( 2 ),
      m_spotFactor( 0.5 ),
      m_diffusionConstant1( 0.5 ),
      m_diffusionConstant2( 0.5 ),
      m_mainThreadContinueCondition( mainThreadContinueCondition ),
      m_waitForMainThreadCondition( waitForMainThreadCondition ),
      m_counter( counter ),
      m_stop( false )
{
}

WTuringPatternCreator::PatternThread::~PatternThread()
{
}

void WTuringPatternCreator::PatternThread::setDiffusionConstants( float d1, float d2 )
{
    WPrecond( d1 >= 0.0 && d1 <= 1.0, "" );
    WPrecond( d2 >= 0.0 && d2 <= 1.0, "" );

    m_diffusionConstant1 = d1;
    m_diffusionConstant2 = d2;
}

void WTuringPatternCreator::PatternThread::setSpotFactor( float spotFactor )
{
    WPrecond( spotFactor > 0.0, "" );

    m_spotFactor = spotFactor;
}

void WTuringPatternCreator::PatternThread::setDomainSize( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ )
{
    WPrecond( sizeX > 0, "" );
    WPrecond( sizeY > 0, "" );
    WPrecond( sizeZ > 0, "" );

    m_sizeX = sizeX;
    m_sizeY = sizeY;
    m_sizeZ = sizeZ;
}

void WTuringPatternCreator::PatternThread::threadMain()
{
    WPrecond( m_concentration1 != 0, "Invalid pointer!" );
    WPrecond( m_concentration2 != 0, "Invalid pointer!" );
    WPrecond( m_noise != 0, "Invalid pointer!" );
    WPrecond( m_delta1 != 0, "Invalid pointer!" );
    WPrecond( m_delta2 != 0, "Invalid pointer!" );

    std::size_t const numVoxels = m_sizeX * m_sizeY * m_sizeZ;

    // The start and end voxels for this thread. Our voxels to process are [ start, end - 1 ] .
    std::size_t start = m_id * ( numVoxels / m_maxThreads );
    std::size_t end = ( m_id + 1 ) * ( numVoxels / m_maxThreads );

    // If we are the last thread, make sure to not exceed the number of voxels (if the number of voxel is not divisible by the number of threads).
    if( m_id == m_maxThreads - 1 )
    {
        end = numVoxels;
    }

    while( !m_stop )
    {
        // Wait for the main (i.e. module-) thread to notify we are ready to start this iteration.
        m_waitForMainThreadCondition->wait();

        // If we are to stop, just return to destroy the thread.
        if( m_stop )
            return;

        for( std::size_t idx = start; idx < end; ++idx )
        {
            std::size_t i = idx % m_sizeX;
            std::size_t j = ( idx / m_sizeX ) % m_sizeY;
            std::size_t k = ( idx / m_sizeX ) / m_sizeY;

            std::size_t iNext = ( i + 1 ) % m_sizeX;
            std::size_t iPrev = ( i + m_sizeX - 1 ) % m_sizeX;

            std::size_t jNext = ( j + 1 ) % m_sizeY;
            std::size_t jPrev = ( j + m_sizeY - 1 ) % m_sizeY;

            std::size_t kNext = ( k + 1 ) % m_sizeZ;
            std::size_t kPrev = ( k + m_sizeZ - 1 ) % m_sizeZ;

            // estimate change in concentrations using 3d laplace filter
            float dc1 = 0.0;
            dc1 += ( *m_concentration1 )[ iPrev + j * m_sizeX + k * m_sizeX * m_sizeY ];
            dc1 += ( *m_concentration1 )[ iNext + j * m_sizeX + k * m_sizeX * m_sizeY ];
            dc1 += ( *m_concentration1 )[ i + jPrev * m_sizeX + k * m_sizeX * m_sizeY ];
            dc1 += ( *m_concentration1 )[ i + jNext * m_sizeX + k * m_sizeX * m_sizeY ];
            dc1 += ( *m_concentration1 )[ i + j * m_sizeX + kPrev * m_sizeX * m_sizeY ];
            dc1 += ( *m_concentration1 )[ i + j * m_sizeX + kNext * m_sizeX * m_sizeY ];
            dc1 -= 6.0f * ( *m_concentration1 )[ idx ];

            float dc2 = 0.0;
            dc2 += ( *m_concentration2 )[ iPrev + j * m_sizeX + k * m_sizeX * m_sizeY ];
            dc2 += ( *m_concentration2 )[ iNext + j * m_sizeX + k * m_sizeX * m_sizeY ];
            dc2 += ( *m_concentration2 )[ i + jPrev * m_sizeX + k * m_sizeX * m_sizeY ];
            dc2 += ( *m_concentration2 )[ i + jNext * m_sizeX + k * m_sizeX * m_sizeY ];
            dc2 += ( *m_concentration2 )[ i + j * m_sizeX + kPrev * m_sizeX * m_sizeY ];
            dc2 += ( *m_concentration2 )[ i + j * m_sizeX + kNext * m_sizeX * m_sizeY ];
            dc2 -= 6.0f * ( *m_concentration2 )[ idx ];

            // diffusion
            ( *m_delta1 )[ idx ] = m_spotFactor * ( 16.0f - ( *m_concentration1 )[ idx ] * ( *m_concentration2 )[ idx ] )
                                 + m_diffusionConstant1 * dc1;
            ( *m_delta2 )[ idx ] = m_spotFactor * ( ( *m_concentration1 )[ idx ] * ( *m_concentration2 )[ idx ]
                                                  - ( *m_concentration2 )[ idx ] - ( *m_noise )[ idx ] ) + m_diffusionConstant2 * dc2;
        }

        // Increase counter to tell the other threads we are done.
        // If we are the last to increment, we need to notify the main thread we are all done with this iteration.
        if( static_cast< std::size_t >( ++( *m_counter ) ) == m_maxThreads )
        {
            m_mainThreadContinueCondition->notify();
            m_counter->reset();   // Don't forget to reset the counter for the next iteration.
        }
    }
}

void WTuringPatternCreator::PatternThread::setBufferPointers( std::vector< float > const* concentration1, std::vector< float > const* concentration2,
                                                              std::vector< float > const* noise, std::vector< float >* delta1,
                                                              std::vector< float >* delta2 )
{
    WPrecond( concentration1 != 0, "Invalid pointer!" );
    WPrecond( concentration2 != 0, "Invalid pointer!" );
    WPrecond( noise != 0, "Invalid pointer!" );
    WPrecond( delta1 != 0, "Invalid pointer!" );
    WPrecond( delta2 != 0, "Invalid pointer!" );

    m_concentration1 = concentration1;
    m_concentration2 = concentration2;
    m_noise = noise;
    m_delta1 = delta1;
    m_delta2 = delta2;
}

void WTuringPatternCreator::PatternThread::finish()
{
    m_stop = true;
}

