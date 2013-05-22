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

#include <algorithm>
#include <vector>

#include <boost/random.hpp>

#include "../common/exceptions/WPreconditionNotMet.h"

#include "WGETexture.h"

#include "WGETextureUtils.h"

void wge::unbindTexture( osg::ref_ptr< osg::Node > node, size_t unit, size_t count )
{
    for( size_t i = unit; i < unit + count; ++i )
    {
        node->getOrCreateStateSet()->removeTextureAttribute( i, osg::StateAttribute::TEXTURE );
        node->getOrCreateStateSet()->removeTextureAttribute( i, osg::StateAttribute::TEXMAT );
    }
}

size_t wge::getMaxTexUnits()
{
    // GLint ret;
    // glGetIntegerv( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &ret );
    // Why do we not use these glGet things here? The answer is simple: The GLSL 1.20 Standard does not define a way to access more than 8
    // texture coordinate attributes.
    return 8;
}

osg::ref_ptr< WGETexture< osg::Texture1D > > wge::genWhiteNoiseTexture( size_t sizeX, size_t channels )
{
    // put it into an texture
    osg::ref_ptr< WGETexture1D > randTexture = new WGETexture1D( genWhiteNoiseImage( sizeX, 1, 1, channels ) );
    randTexture->setTextureWidth( sizeX );
    randTexture->setFilter( osg::Texture1D::MIN_FILTER, osg::Texture1D::NEAREST );
    randTexture->setFilter( osg::Texture1D::MAG_FILTER, osg::Texture1D::NEAREST );
    randTexture->setWrap( osg::Texture1D::WRAP_S, osg::Texture1D::REPEAT );

    return randTexture;
}

osg::ref_ptr< WGETexture< osg::Texture2D > > wge::genWhiteNoiseTexture( size_t sizeX, size_t sizeY, size_t channels )
{
    osg::ref_ptr< WGETexture2D > randTexture = new WGETexture2D( genWhiteNoiseImage( sizeX, sizeY, 1, channels ) );
    randTexture->setTextureWidth( sizeX );
    randTexture->setTextureHeight( sizeY );
    randTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    randTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
    randTexture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    randTexture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );

    return randTexture;
}

osg::ref_ptr< WGETexture< osg::Texture3D > > wge::genWhiteNoiseTexture( size_t sizeX, size_t sizeY, size_t sizeZ, size_t channels )
{
    osg::ref_ptr< WGETexture3D > randTexture = new WGETexture3D( genWhiteNoiseImage( sizeX, sizeY, sizeZ, channels ) );
    randTexture->setTextureWidth( sizeX );
    randTexture->setTextureHeight( sizeY );
    randTexture->setTextureDepth( sizeZ );
    randTexture->setFilter( osg::Texture2D::MIN_FILTER, osg::Texture2D::NEAREST );
    randTexture->setFilter( osg::Texture2D::MAG_FILTER, osg::Texture2D::NEAREST );
    randTexture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
    randTexture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
    randTexture->setWrap( osg::Texture2D::WRAP_R, osg::Texture2D::REPEAT );

    return randTexture;
}

osg::ref_ptr< osg::Image > wge::genWhiteNoiseImage( size_t sizeX, size_t sizeY, size_t sizeZ, size_t channels )
{
    WPrecond( ( channels == 1 ) || ( channels == 3 ) || ( channels == 4 ), "Invalid number of channels. Valid are: 1, 3 and 4."  );

    // create an osg::Image at first.
    std::srand( time( 0 ) );
    osg::ref_ptr< osg::Image > randImage = new osg::Image();
    GLenum type = GL_LUMINANCE;
    if( channels == 3 )
    {
        type = GL_RGB;
    }
    else if( channels == 4 )
    {
        type = GL_RGBA;
    }
    randImage->allocateImage( sizeX, sizeY, sizeZ, type, GL_UNSIGNED_BYTE );
    unsigned char *randomLuminance = randImage->data();  // should be 4 megs
    for( size_t i = 0; i < channels * sizeX * sizeY * sizeZ; ++i )
    {
        // - stylechecker says "use rand_r" but I am not sure about portability.
        unsigned char r = static_cast< unsigned char >( std::rand() % 255 );  // NOLINT - no we want std::rand instead of rand_r
        randomLuminance[ i ] = r;
    }

    return randImage;
}

osg::ref_ptr< WGETexture< osg::Texture3D > > wge::genTuringNoiseTexture( std::size_t sizeX, std::size_t sizeY,
                                                                         std::size_t sizeZ, std::size_t channels )
{
    WPrecond( channels == 1 || channels == 3 || channels == 4, "Invalid number of channels. Valid are: 1, 3 and 4." );

    // some constants, maybe change to parameters
    float const spotIrregularity = 0.05f;   // 0.0 - 1.0
    std::size_t const iterations = 200;
    float const spotSize = 0.5;
    float const spotFactor = ( 0.02f + 0.58f * ( 1.0f - spotSize ) ) / 15.0f;
    float const d1 = 0.125f;
    float const d2 = 0.03125f;
    float const speed = 1.0f;

    osg::ref_ptr< osg::Image > img = new osg::Image;
    GLenum type = GL_LUMINANCE;
    if( channels == 3 )
    {
        type = GL_RGB;
    }
    else if( channels == 4 )
    {
        type = GL_RGBA;
    }

    std::vector< float > concentration1( sizeX * sizeY * sizeZ, 4.0 );
    std::vector< float > concentration2( sizeX * sizeY * sizeZ, 4.0 );
    std::vector< float > delta1( sizeX * sizeY * sizeZ, 0.0 );
    std::vector< float > delta2( sizeX * sizeY * sizeZ, 0.0 );
    std::vector< float > noise( sizeX * sizeY * sizeZ );

    boost::mt19937 generator( std::time( 0 ) );

    float noiseRange = 0.1f + 4.9f * spotIrregularity;

    boost::uniform_real< float > dist( 12.0 - noiseRange, 12.0 + noiseRange );
    boost::variate_generator< boost::mt19937&, boost::uniform_real< float > > rand( generator, dist );

    // initialization step
    for( std::size_t i = 0; i < sizeX; ++i )
    {
        for( std::size_t j = 0; j < sizeY; ++j )
        {
            for( std::size_t k = 0; k < sizeZ; ++k )
            {
                std::size_t idx = i + j * sizeX + k * sizeX * sizeY;
                noise[ idx ] = rand();
            }
        }
    }

    // iteration
    for( std::size_t iter = 0; iter < iterations; ++iter )
    {
        std::cout << "iterations: " << iter << std::endl;

        for( std::size_t i = 0; i < sizeX; ++i )
        {
            std::size_t iNext = ( i + 1 ) % sizeX;
            std::size_t iPrev = ( i + sizeX - 1 ) % sizeX;

            for( std::size_t j = 0; j < sizeY; ++j )
            {
                std::size_t jNext = ( j + 1 ) % sizeY;
                std::size_t jPrev = ( j + sizeY - 1 ) % sizeY;

                for( std::size_t k = 0; k < sizeZ; ++k )
                {
                    std::size_t kNext = ( k + 1 ) % sizeZ;
                    std::size_t kPrev = ( k + sizeZ - 1 ) % sizeZ;

                    std::size_t idx = i + j * sizeX + k * sizeX * sizeY;

                    // estimate change in concentrations
                    // we use a 3d laplace filter here instead of the 2d filter as in Eichelbaum et al.
                    float dc1 = 0.0;
                    dc1 += concentration1[ iPrev + j * sizeX + k * sizeX * sizeY ];
                    dc1 += concentration1[ iNext + j * sizeX + k * sizeX * sizeY ];
                    dc1 += concentration1[ i + jPrev * sizeX + k * sizeX * sizeY ];
                    dc1 += concentration1[ i + jNext * sizeX + k * sizeX * sizeY ];
                    dc1 += concentration1[ i + j * sizeX + kPrev * sizeX * sizeY ];
                    dc1 += concentration1[ i + j * sizeX + kNext * sizeX * sizeY ];
                    dc1 -= 6.0f * concentration1[ idx ];

                    float dc2 = 0.0;
                    dc2 += concentration2[ iPrev + j * sizeX + k * sizeX * sizeY ];
                    dc2 += concentration2[ iNext + j * sizeX + k * sizeX * sizeY ];
                    dc2 += concentration2[ i + jPrev * sizeX + k * sizeX * sizeY ];
                    dc2 += concentration2[ i + jNext * sizeX + k * sizeX * sizeY ];
                    dc2 += concentration2[ i + j * sizeX + kPrev * sizeX * sizeY ];
                    dc2 += concentration2[ i + j * sizeX + kNext * sizeX * sizeY ];
                    dc2 -= 6.0f * concentration2[ idx ];

                    // diffusion
                    delta1[ idx ] = spotFactor * ( 16.0f - concentration1[ idx ] * concentration2[ idx ] ) + d1 * dc1;
                    delta2[ idx ] = spotFactor * ( concentration1[ idx ] * concentration2[ idx ] - concentration2[ idx ] - noise[ idx ] ) + d2 * dc2;
                }
            }
        }

        for( std::size_t i = 0; i < sizeX; ++i )
        {
            for( std::size_t j = 0; j < sizeY; ++j )
            {
                for( std::size_t k = 0; k < sizeZ; ++k )
                {
                    std::size_t idx = i + j * sizeX + k * sizeX * sizeY;

                    concentration1[ idx ] += speed * delta1[ idx ];
                    concentration2[ idx ] += speed * delta2[ idx ];
                }
            }
        }
    }

    img->allocateImage( sizeX, sizeY, sizeZ, type, GL_UNSIGNED_BYTE );

    // find min and max
    float c1min = *std::min_element( concentration1.begin(), concentration1.end() );
    float c1max = *std::max_element( concentration1.begin(), concentration1.end() );

    // copy to image
    for( std::size_t i = 0; i < sizeX; ++i )
    {
        for( std::size_t j = 0; j < sizeY; ++j )
        {
            for( std::size_t k = 0; k < sizeZ; ++k )
            {
                std::size_t idx = i + j * sizeX + k * sizeX * sizeY;
                img->data()[ idx ] = 255.0f * ( concentration1[ idx ] - c1min ) / ( c1max - c1min );
            }
        }
    }

    return osg::ref_ptr< WGETexture< osg::Texture3D > >( new WGETexture< osg::Texture3D >( img ) );
}

