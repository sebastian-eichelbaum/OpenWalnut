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

#ifndef WTURINGPATTERNCREATOR_H
#define WTURINGPATTERNCREATOR_H

#include <vector>

#include <boost/thread.hpp>

#include <core/common/WCounter.h>
#include <core/common/WConditionSet.h>
#include <core/common/WProgress.h>
#include <core/common/WThreadedRunner.h>

#include <core/graphicsEngine/WGETexture.h>

/**
 * Creates a pattern of evenly spaced dots via simulating a chemical reaction.
 *
 * The pattern is created on a regular 3D grid.
 */
class WTuringPatternCreator
{
public:
    /**
     * Constructor.
     *
     * \param progress The progress indicator to use.
     * \param numThreads The number of threads to use.
     */
    WTuringPatternCreator( boost::shared_ptr< WProgress > const progress, std::size_t numThreads = boost::thread::hardware_concurrency() );

    /**
     * Destructor.
     */
    ~WTuringPatternCreator();

    /**
     * Creates the 3D pattern and writes it into a vector of floats. This vector can simply be added to a valueset.
     *
     * \param sizeX The size of the grid in x-direction.
     * \param sizeY The size of the grid in y-direction.
     * \param sizeZ The size of the grid in z-direction.
     *
     * \return The vector containing the pattern data.
     */
    boost::shared_ptr< std::vector< float > > create( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ );

    /**
     * Sets the spotsize parameter.
     *
     * \param size A value between 0 and 1.
     */
    void setSpotSize( float size );

    /**
     * Sets the spot irregularity parameter.
     *
     * \param irr A value between 0 and 1.
     */
    void setSpotIrregularity( float irr );

    /**
     * Sets the number of iterations for the chemical simulation that creates the pattern.
     *
     * \param iter The number of iterations.
     */
    void setNumIterations( std::size_t iter );

private:
    /**
     * A thread calculating the reaction for a certain part of the domain.
     *
     * The thread is constructed once. Using the conditions provided to the constructor,
     * we can synchronize the pattern threads and the main thread.
     */
    class PatternThread : public WThreadedRunner
    {
    public:
        /**
         * Constructor.
         *
         * \param id The id of the thread. Must be smaller than max.
         * \param max The number of threads.
         * \param mainThreadContinueCondition The condition we use to tell the main thread we are done for the current iteration.
         * \param waitForMainThreadCondition The condition used to wait for the main thread to prepare the next iteration.
         * \param counter A counter used by all threads to signal how many have finished the current iteration.
         */
        PatternThread( std::size_t id, std::size_t max, boost::shared_ptr< WCondition > const mainThreadContinueCondition,
                       boost::shared_ptr< WCondition > const waitForMainThreadCondition, WCounter* const counter );

        /**
         * Destructor.
         */
        ~PatternThread();

        /**
         * Set the domain size.
         *
         * \param sizeX The size of the 3D regular domain in x-direction.
         * \param sizeY The size of the 3D regular domain in y-direction.
         * \param sizeZ The size of the 3D regular domain in z-direction.
         */
        void setDomainSize( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ );

        /**
         * Set the spot factor.
         *
         * \param spotFactor The spot factor.
         */
        void setSpotFactor( float spotFactor );

        /**
         * Set the diffusion constants.
         *
         * \param d1 First diffusion constant.
         * \param d2 Second diffusion constant.
         */
        void setDiffusionConstants( float d1, float d2 );

        /**
         * Set the pointers to the buffer all threads share.
         *
         * \param concentration1 The buffer for the concentration of the first substance.
         * \param concentration2 The buffer for the concentration of the second substance.
         * \param noise A constant buffer containing a random value per voxel.
         * \param delta1 A buffer that will store the change in concentration for the first substance.
         * \param delta2 A buffer that will store the change in concentration for the second substance.
         */
        void setBufferPointers( std::vector< float > const* concentration1, std::vector< float > const* concentration2,
                                std::vector< float > const* noise, std::vector< float >* delta1, std::vector< float >* delta2 );

        /**
         * The entry point for the thread.
         */
        virtual void threadMain();

        /**
         * Tells the thread to finish.
         *
         * Note that the waiting condition also needs to be notified for the thread to actually finish.
         */
        void finish();

    private:
        //! The number of this thread.
        std::size_t m_id;

        //! The maximum number of threads.
        std::size_t m_maxThreads;

        //! The size of the domain in x-direction.
        std::size_t m_sizeX;

        //! The size of the domain in y-direction.
        std::size_t m_sizeY;

        //! The size of the domain in z-direction.
        std::size_t m_sizeZ;

        //! The spot factor.
        float m_spotFactor;

        //! The first diffusion constant.
        float m_diffusionConstant1;

        //! The second diffusion constant.
        float m_diffusionConstant2;

        //! The buffer for the concentration of the first substance.
        std::vector< float > const* m_concentration1;

        //! The buffer for the concentration of the second substance.
        std::vector< float > const* m_concentration2;

        //! A constant buffer containing a random value per voxel.
        std::vector< float > const* m_noise;

        //! A buffer that will store the change in concentration for the first substance.
        std::vector< float >* m_delta1;

        //! A buffer that will store the change in concentration for the second substance.
        std::vector< float >* m_delta2;

        //! The condition used to tell the main thread we are done for this iteration.
        boost::shared_ptr< WCondition > m_mainThreadContinueCondition;

        //! The condition used by the main thread to notify we are ready to start the next iteration.
        boost::shared_ptr< WCondition > m_waitForMainThreadCondition;

        //! Counts the number of threads done with the current iteration.
        WCounter* const m_counter;

        //! Whether we are to stop the thread.
        bool m_stop;
    };

    //! The number of threads to use.
    std::size_t m_numThreads;

    //! The number of iterations for the simulation.
    float m_numIterations;

    //! The spot irregularity parameter.
    float m_spotIrregularity;

    //! The spot size parameter.
    float m_spotSize;

    //! The progress to increment.
    boost::shared_ptr< WProgress > m_progress;
};

#endif  // WTURINGPATTERNCREATOR_H
