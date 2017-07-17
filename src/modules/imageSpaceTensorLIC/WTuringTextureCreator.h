//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WTURINGTEXTURECREATOR_H
#define WTURINGTEXTURECREATOR_H

#include <vector>

#include <boost/thread.hpp>

#include <core/common/WThreadedRunner.h>
#include <core/graphicsEngine/WGETexture.h>

/**
 * Class to create a Turing noise texture using multiple threads if needed.
 */
class WTuringTextureCreator
{
public:
    /**
     * Constructor. Initializes threading but does not yet start texture creation.
     *
     * \param numThreads the number of threads to use. By default, this is the number of threads natively supported by
     * the CPU.
     */
    WTuringTextureCreator( std::size_t numThreads = boost::thread::hardware_concurrency() );

    /**
     * Destructor.
     */
    ~WTuringTextureCreator();

    /**
     * Create the Turing noise texture of arbitrary size. This function is synchronous and returns a valid texture.
     *
     * \param sizeX the size along the x axis.
     * \param sizeY the size along the y axis.
     * \param sizeZ the size along the z axis.
     *
     * \return the texture.
     */
    osg::ref_ptr< WGETexture3D > create( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ );

    /**
     * Define the size of the spots inside the texture.
     *
     * \param size the size in [0,1], 1 meaning larger spots.
     */
    void setSpotSize( float size );

    /**
     * The irregularity of the spots.
     *
     * \param irr the irregularity in [0,1]; 0 producing a very regular grid of spots, whereas 1 causes the spots to be
     * rather irregular.
     */
    void setSpotIrregularity( float irr );

    /**
     * The number of iterations to use for calculating the texture.
     *
     * \param iter the number of iterations. 100 is recommended, lower values create a noisy texture, higher values
     * cause the spots to be rather smooth.
     */
    void setNumIterations( std::size_t iter );

private:
    /**
     * The thread calculating the Turing concentration diffusion in a given range.
     */
    class TextureThread : public WThreadedRunner
    {
    public:
        /**
         * Create a calculation thread. This does not start calculation. It uses both parameters to define the memory
         * range to work in.
         *
         * \param id the id of this thread. Must be in [0, max).
         * \param max the maximum number of threads.
         */
        TextureThread( std::size_t id, std::size_t max );

        /**
         * Destructor.
         */
        ~TextureThread();

        /**
         * Target texture size.
         *
         * \param sizeX size in X
         * \param sizeY size in Y
         * \param sizeZ size in Z
         */
        void setTextureSize( std::size_t sizeX, std::size_t sizeY, std::size_t sizeZ );

        /**
         * The factor influencing size and shape of a spot.
         *
         * \param spotFactor the factor. Refer to the Turing Reaction Diffusion algorithm. It mainly relates to the spot
         * size.
         */
        void setSpotFactor( float spotFactor );

        /**
         * Diffusion constants according to Turing Reaction Diffusion algorithm.
         *
         * \param d1 Diffusion constant of fluid 1
         * \param d2 Diffusion constant of fluid 2
         */
        void setDiffusionConstants( float d1, float d2 );

        /**
         * Define the memory to work in.
         *
         * \param concentration1 the shared memory for concentration values of fluid 1
         * \param concentration2 the shared memory for concentration values of fluid 2
         * \param noise the input noise.
         * \param delta1 the shared memory representing the concentration change of fluid 1
         * \param delta2 the shared memory representing the concentration change of fluid 2
         */
        void setBufferPointers( std::vector< float > const* concentration1, std::vector< float > const* concentration2,
                                std::vector< float > const* noise, std::vector< float >* delta1, std::vector< float >* delta2 );

        /**
         * The actual thread function running the algorithm.
         */
        virtual void threadMain();

    private:
        /**
         * ID of the thread.
         */
        std::size_t m_id;

        /**
         * Number of all spawned threads.
         */
        std::size_t m_maxThreads;

        /**
         * Size of the resulting texture.
         */
        std::size_t m_sizeX;

        /**
         * Size of the resulting texture.
         */
        std::size_t m_sizeY;

        /**
         * Size of the resulting texture.
         */
        std::size_t m_sizeZ;

        /**
         * The factor defining the spot size and shape.
         */
        float m_spotFactor;

        /**
         * The amount of diffusion of fluid 1.
         */
        float m_diffusionConstant1;

        /**
         * The amount of diffusion of fluid 2.
         */
        float m_diffusionConstant2;

        //! the shared memory for concentration values of fluid 1
        std::vector< float > const* m_concentration1;

        //! the shared memory for concentration values of fluid 2
        std::vector< float > const* m_concentration2;

        //! the input noise.
        std::vector< float > const* m_noise;

        //! the shared memory representing the concentration change of fluid 1
        std::vector< float >* m_delta1;

        //! the shared memory representing the concentration change of fluid 2
        std::vector< float >* m_delta2;
    };

    /**
     * Number of iterations. 100 by default.
     */
    float m_numIterations;

    /**
     * Spot irregularity. 0.1 by default.
     */
    float m_spotIrregularity;

    /**
     * Spot size. 0.1 by default.
     */
    float m_spotSize;

    /**
     * The thread pool.
     */
    std::vector< boost::shared_ptr< TextureThread > > m_threads;
};

#endif  // WTURINGTEXTURECREATOR_H
