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

