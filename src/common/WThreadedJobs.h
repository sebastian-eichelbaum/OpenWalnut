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

#ifndef WTHREADEDJOBS_H
#define WTHREADEDJOBS_H

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>

#include "WException.h"
#include "WFlag.h"
#include "WLogger.h"

/**
 * \class WThreadedJobs
 *
 * A threaded functor base class for producer-consumer-style multithreaded computation.
 *
 * A job generator function produces jobs that are then distributed to the threads in
 * a first come first serve manner. The first template parameter is the type of the input data,
 * for example a WDataSetScalar. The second template parameter is the type of object that
 * represents the jobs.
 *
 * Both the getJob() and the compute() functions need to be implemented.
 *
 * \ingroup common
 */
template< class Input_T, class Job_T >
class WThreadedJobs
{
public:
    //! the input type
    typedef Input_T InputType;

    //! the job type
    typedef Job_T JobType;

    /**
     * Constructor.
     *
     * \param input The input.
     */
    WThreadedJobs( boost::shared_ptr< InputType const > input ); // NOLINT

    /**
     * Destructor.
     */
    virtual ~WThreadedJobs();

    /**
     * The threaded function operation. Pulls jobs and executes the \see compute()
     * function.
     *
     * \param id The thread's ID.
     * \param numThreads How many threads are working on the jobs.
     * \param shutdown A shared flag indicating the thread should be stopped.
     */
    void operator() ( std::size_t id, std::size_t numThreads, WBoolFlag const& shutdown );

    /**
     * Abstract function for the job aquisition.
     *
     * \param job The job (output).
     * \return false, iff no more jobs need to be processed.
     */
    virtual bool getJob( JobType& job ) = 0; // NOLINT

    /**
     * Abstract function that performs the actual computation per job.
     *
     * \param input The input data.
     * \param job The current job.
     */
    virtual void compute( boost::shared_ptr< InputType const > input, JobType const& job ) = 0;

protected:

    //! the input
    boost::shared_ptr< InputType const > m_input;
private:
};

template< class Input_T, class Job_T >
WThreadedJobs< Input_T, Job_T >::WThreadedJobs( boost::shared_ptr< InputType const > input )
    : m_input( input )
{
    if( !m_input )
    {
        throw WException( std::string( "Invalid input." ) );
    }
}

template< class Input_T, class Job_T >
WThreadedJobs< Input_T, Job_T >::~WThreadedJobs()
{
}

template< class Input_T, class Job_T >
void WThreadedJobs< Input_T, Job_T >::operator() ( std::size_t /* id */, std::size_t /* numThreads */, WBoolFlag const& shutdown )
{
    JobType job;
    while( getJob( job ) && !shutdown() )
    {
        compute( m_input, job );
    }
}

/**
 * Nearly the same class as WThreadedJobs, but this class is intended to be used for multithreaded operations on voxels and therefore it
 * uses Striping to partition the data. This is necessarry since if the threads are not operating on blocks, they slow down!
 */
template< class Input_T, class Job_T >
class WThreadedStripingJobs
{
public:
    //! the input type
    typedef Input_T InputType;

    //! the job type
    typedef Job_T JobType;

    /**
     * Constructor.
     *
     * \param input The input.
     */
    WThreadedStripingJobs( boost::shared_ptr< InputType const > input ); // NOLINT

    /**
     * Destructor.
     */
    virtual ~WThreadedStripingJobs();

    /**
     * The threaded function operation. Pulls jobs and executes the \see compute()
     * function.
     *
     * \param id The thread's ID.
     * \param numThreads How many threads are working on the jobs.
     * \param shutdown A shared flag indicating the thread should be stopped.
     */
    void operator() ( std::size_t id, std::size_t numThreads, WBoolFlag const& shutdown );

    /**
     * Abstract function that performs the actual computation per voxel.
     *
     * \param input The input data.
     * \param voxelNum The voxel number to operate on.
     */
    virtual void compute( boost::shared_ptr< InputType const > input, std::size_t voxelNum ) = 0;

protected:

    //! the input
    boost::shared_ptr< InputType const > m_input;
private:
};

template< class Input_T, class Job_T >
WThreadedStripingJobs< Input_T, Job_T >::WThreadedStripingJobs( boost::shared_ptr< InputType const > input )
    : m_input( input )
{
    if( !m_input )
    {
        throw WException( std::string( "Invalid input." ) );
    }
}

template< class Input_T, class Job_T >
WThreadedStripingJobs< Input_T, Job_T >::~WThreadedStripingJobs()
{
}

template< class Input_T, class Job_T >
void WThreadedStripingJobs< Input_T, Job_T >::operator() ( std::size_t id, std::size_t numThreads, WBoolFlag const& shutdown )
{
    WAssert( m_input, "Bug: operations of an invalid input requested." );
    size_t numElements = m_input->size();

    // partition the voxels via simple striping
    size_t start = numElements / numThreads * id;
    size_t end = ( id + 1 ) * ( numElements / numThreads );
    if( id == numThreads - 1 ) // last thread may have less elements to take care.
    {
        end = numElements;
    }

    for( size_t voxelNum = start; ( voxelNum < end ) && !shutdown(); ++voxelNum )
    {
        compute( m_input, voxelNum );
    }
}

#endif  // WTHREADEDJOBS_H
