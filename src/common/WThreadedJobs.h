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

#include <string>

#include <boost/shared_ptr.hpp>

#include "WException.h"
#include "WFlag.h"

/**
 * \class WThreadedJobs
 *
 * A threaded functor base class for producer-consumer-style multithreaded computation.
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
     * \param input The input dataset.
     */
    WThreadedJobs( boost::shared_ptr< InputType > input ); // NOLINT

    /**
     * Destructor.
     */
    virtual ~WThreadedJobs();

    /**
     * The operation to be performed per job.
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
     * Abstract function that performs the actual computation.
     *
     * \param input The input data.
     * \param job The current job.
     */
    virtual void compute( boost::shared_ptr< InputType const > input, JobType const& job ) = 0;

private:
    //! the input
    boost::shared_ptr< InputType const > m_input;
};

template< class Input_T, class Job_T >
WThreadedJobs< Input_T, Job_T >::WThreadedJobs( boost::shared_ptr< InputType > input )
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
void WThreadedJobs< Input_T, Job_T >::operator() ( std::size_t, std::size_t, WBoolFlag const& shutdown )
{
    JobType job;
    while( getJob( job ) && !shutdown() )
    {
        compute( m_input, job );
    }
}

#endif  // WTHREADEDJOBS_H
