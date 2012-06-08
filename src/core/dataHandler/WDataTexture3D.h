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

#ifndef WDATATEXTURE3D_H
#define WDATATEXTURE3D_H

#include <algorithm>
#include <limits>
#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include "../graphicsEngine/WGETexture.h"
#include "../graphicsEngine/WGETypeTraits.h"
#include "../common/WProperties.h"
#include "../common/WLogger.h"

#include "WValueSetBase.h"
#include "WGridRegular3D.h"

/**
 * Namespace provides some scaling functions for scaling data values to meet the OpenGL requirements.
 */
namespace WDataTexture3DScalers
{
    /**
     * Scales the specified value to the interval [0,1] using m_min and m_scale. As the method is inline, the additional parameters are no
     * problem.
     *
     * \param value the value to scale
     * \param minimum the min value
     * \param maximum the max value
     * \param scaler the scaler
     *
     * \return the value scaled to [0,1]
     *
     * \note Most integral types need to be scaled. See WGETypeTraits.h for details.
     */
    template < typename T >
    inline typename wge::GLType< T >::Type scaleInterval( T value, T minimum, T maximum, double scaler )
    {
        return static_cast< double >( std::min( std::max( value, minimum ), maximum ) - minimum ) / scaler;
    }

    /**
     * Byte data is transferred to texture mem as is without any scaling.
     *
     * \param value the value to scale
     *
     * \return the value
     */
    inline int8_t scaleInterval( int8_t value, int8_t /*minimum*/, int8_t /*maximum*/, double /*scaler*/ )
    {
        return value;
    }

    /**
     * Byte data is transferred to texture mem as is without any scaling.
     *
     * \param value the value to scale
     *
     * \return the value
     */
    inline uint8_t scaleInterval( uint8_t value, uint8_t /*minimum*/, uint8_t /*maximum*/, double /*scaler*/ )
    {
        return value;
    }
}

/**
 * This class allows simple creation of WGETexture3D by using a specified grid and value-set. One advantage: the
 * first call to the texture's update callback ensures texture creation. It is not created earlier.
 */
class WDataTexture3D: public WGETexture3D
{
public:
    /**
     * Constructor. Creates the texture. Just run it after graphics engine was initialized.
     *
     * \param valueSet  the value set to use
     * \param grid the grid to use
     */
     WDataTexture3D( boost::shared_ptr< WValueSetBase > valueSet, boost::shared_ptr< WGridRegular3D > grid );

    /**
     * Destructor.
     */
    virtual ~WDataTexture3D();

    /**
     * Returns the texture's bounding box. This is const. Although there exists the transformation() property, it is an information property and
     * can't be changed. This represents the underlying grid.
     *
     * \return the bounding box.
     */
    virtual WBoundingBox getBoundingBox() const;

protected:
    /**
     * Creates the texture data. This method creates the texture during the first update traversal using the value set and grid.
     */
    virtual void create();

private:
    /**
     * The value set from which the texture gets created.
     */
    boost::shared_ptr< WValueSetBase > m_valueSet;

    /**
     * The bounding box of the underlying grid.
     */
    WBoundingBox m_boundingBox;

    /**
     * The lock for securing createTexture.
     */
    boost::shared_mutex m_creationLock;

    /**
     * Creates a properly sized osg::Image from the specified source data.
     *
     * \param source the source data
     * \param components number of components
     * \tparam T the type of source data
     *
     * \return
     */
    template < typename T >
    osg::ref_ptr< osg::Image > createTexture( T* source, int components = 1 );
};

/**
 * Extend the wge utils namespace with additional methods relating WDataTexture3D.
 */
namespace wge
{
    /**
     * Binds the specified texture to the specified unit. It automatically adds several uniforms which then can be utilized in the shader:
     * - u_textureXUnit: the unit number (useful for accessing correct gl_TexCoord and so on)
     * - u_textureXSampler: the needed sampler
     * - u_textureXSizeX: width of the texture in pixels
     * - u_textureXSizeY: height of the texture in pixels
     * - u_textureXSizeZ: depth of the texture in pixels
     * If the specified texture is a WGETexture, it additionally adds u_textureXMin and u_textureXScale for unscaling.
     *
     * \param node where to bind
     * \param unit the unit to use
     * \param texture the texture to use.
     * \param prefix if specified, defines the uniform name prefix. (Sampler, Unit, Sizes, ...)
     * \tparam T the type of texture. Usually osg::Texture3D or osg::Texture2D.
     */
    void bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WDataTexture3D > texture,
                                           size_t unit = 0, std::string prefix = ""  );
}

template < typename T >
osg::ref_ptr< osg::Image > WDataTexture3D::createTexture( T* source, int components )
{
    // get lock
    boost::unique_lock< boost::shared_mutex > lock( m_creationLock );

    // get the current scaling info
    T min = static_cast< T >( minimum()->get() );
    double scaler = scale()->get();
    T max = min + static_cast< T >( scaler );

    typedef typename wge::GLType< T >::Type TexType;
    GLenum type = wge::GLType< T >::TypeEnum;

    wlog::debug( "WDataTexture3D" ) << "Resolution: " << getTextureWidth() << "x" << getTextureHeight() << "x" << getTextureDepth();
    wlog::debug( "WDataTexture3D" ) << "Channels: " << components;
    // NOTE: the casting is needed as if T == uint8_t -> it will be interpreted as ASCII code -> bad.
    wlog::debug( "WDataTexture3D" ) << "Value Range: [" << static_cast< float >( min ) << "," << static_cast< float >( max ) <<
                                                       "] - Scaler: " << scaler;
    osg::ref_ptr< osg::Image > ima = new osg::Image;

    size_t nbVoxels = getTextureWidth() * getTextureHeight() * getTextureDepth();

    if( components == 1)
    {
        // OpenGL just supports float textures
        ima->allocateImage( getTextureWidth(), getTextureHeight(), getTextureDepth(), GL_LUMINANCE_ALPHA, type );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for( unsigned int i = 0; i < nbVoxels; ++i )
        {
            data[ 2 * i ] = WDataTexture3DScalers::scaleInterval( source[i], min, max, scaler );
            // NOTE: this is done to avoid ugly black borders when interpolation is active.
            data[ ( 2 * i ) + 1] = wge::GLType< T >::FullIntensity() * ( source[i] != min );
        }
    }
    else if( components == 2)
    {
        // OpenGL just supports float textures
        ima->allocateImage( getTextureWidth(), getTextureHeight(), getTextureDepth(), GL_RGBA, type );
        ima->setInternalTextureFormat( GL_RGBA );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for( unsigned int i = 0; i < nbVoxels; ++i )
        {
            data[ ( 4 * i ) ]     = WDataTexture3DScalers::scaleInterval( source[ ( 2 * i ) ], min, max, scaler );
            data[ ( 4 * i ) + 1 ] = WDataTexture3DScalers::scaleInterval( source[ ( 2 * i ) + 1 ], min, max, scaler );
            data[ ( 4 * i ) + 2 ] = 0;
            data[ ( 4 * i ) + 3 ] = wge::GLType< T >::FullIntensity();
        }
    }
    else if( components == 3)
    {
        // OpenGL just supports float textures
        ima->allocateImage( getTextureWidth(), getTextureHeight(), getTextureDepth(), GL_RGBA, type );
        ima->setInternalTextureFormat( GL_RGBA );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for( unsigned int i = 0; i < nbVoxels; ++i )
        {
            data[ ( 4 * i ) ]     = WDataTexture3DScalers::scaleInterval( source[ ( 3 * i ) ], min, max, scaler );
            data[ ( 4 * i ) + 1 ] = WDataTexture3DScalers::scaleInterval( source[ ( 3 * i ) + 1 ], min, max, scaler );
            data[ ( 4 * i ) + 2 ] = WDataTexture3DScalers::scaleInterval( source[ ( 3 * i ) + 2 ], min, max, scaler );
            data[ ( 4 * i ) + 3 ] = wge::GLType< T >::FullIntensity();
        }
    }
    else if( components == 4)
    {
        // OpenGL just supports float textures
        ima->allocateImage( getTextureWidth(), getTextureHeight(), getTextureDepth(), GL_RGBA, type );
        ima->setInternalTextureFormat( GL_RGBA );
        TexType* data = reinterpret_cast< TexType* >( ima->data() );

        // Copy the data pixel wise and convert to float
        for( unsigned int i = 0; i < nbVoxels; ++i )
        {
            data[ ( 4 * i ) ]     = WDataTexture3DScalers::scaleInterval( source[ ( 4 * i ) ], min, max, scaler );
            data[ ( 4 * i ) + 1 ] = WDataTexture3DScalers::scaleInterval( source[ ( 4 * i ) + 1 ], min, max, scaler );
            data[ ( 4 * i ) + 2 ] = WDataTexture3DScalers::scaleInterval( source[ ( 4 * i ) + 2 ], min, max, scaler );
            data[ ( 4 * i ) + 3 ] = WDataTexture3DScalers::scaleInterval( source[ ( 4 * i ) + 3 ], min, max, scaler );
        }
    }
    else
    {
        wlog::error( "WDataTexture3D" ) << "Did not handle dataset ( components != 1,2,3 or 4 ).";
    }

    // done, unlock
    lock.unlock();

    return ima;
}

#endif  // WDATATEXTURE3D_H

