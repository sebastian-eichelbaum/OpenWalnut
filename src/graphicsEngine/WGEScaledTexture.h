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

#ifndef WGESCALEDTEXTURE_H
#define WGESCALEDTEXTURE_H

#include <string>

#include <boost/lexical_cast.hpp>

#include <osg/Node>
#include <osg/Texture>
#include <osg/Texture1D>
#include <osg/Texture2D>
#include <osg/Texture3D>

#include "WGEUtils.h"

/**
 * This calls serves a simple purpose: have a texture and its scaling information together which allows very easy binding of textures to nodes
 * with associated shaders. When this texture gets bind using the bindTo methods, uniforms get added containing needed scaling information.
 */
template < typename TextureType = osg::Texture >
class WGEScaledTexture: public TextureType
{
public:
    /**
     * Default constructor. Creates an empty instance of the texture.
     *
     * \param scale the scaling factor needed for de-scaling the texture values
     * \param min the minimum value allowing negative values too.
     */
    WGEScaledTexture( double scale = 1.0, double min = 0.0 );

    /**
     * Creates texture from given image. Scaling is set to identity.
     *
     * \param image the image to use as texture
     * \param scale the scaling factor needed for de-scaling the texture values
     * \param min the minimum value allowing negative values too.
     */
    WGEScaledTexture( osg::Image* image, double scale = 1.0, double min = 0.0 );

    /**
     * Copy the texture.
     *
     * \param texture the texture to copy
     * \param copyop
     */
    WGEScaledTexture( const WGEScaledTexture< TextureType >& texture, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * Destructor.
     */
    virtual ~WGEScaledTexture();

    /**
     * Get the minimum in the de-scaled value space.
     *
     * \return the minimum
     */
    double getMin() const;

    /**
     * Get the scaling factor for de-scaling the texture.
     *
     * \return the scale
     */
    double getScale() const;

    /**
     * Binds the texture to the specified node and texture unit. It also adds two uniforms: u_textureXMin and u_textureXScale, where X
     * is the unit number. This can be used in shaders to unscale it.
     *
     * \param node the node where to bind the texture to
     * \param unit the unit, by default 0
     */
    void bind( osg::ref_ptr< osg::Node > node, size_t unit = 0 );

protected:

private:

    /**
     * The minimum of each value in the texture in unscaled space.
     */
    double m_min;

    /**
     * The scaling factor to de-scale a [0-1] texture to original space.
     */
    double m_scale;
};

template < typename TextureType >
WGEScaledTexture< TextureType >::WGEScaledTexture( double scale, double min ):
    TextureType(),
    m_min( min ),
    m_scale( scale )
{
    // initialize members
}

template < typename TextureType >
WGEScaledTexture< TextureType >::WGEScaledTexture( osg::Image* image, double scale, double min ):
    TextureType( image ),
    m_min( min ),
    m_scale( scale )
{
    // initialize members
}

template < typename TextureType >
WGEScaledTexture< TextureType >::WGEScaledTexture( const WGEScaledTexture< TextureType >& texture, const osg::CopyOp& copyop ):
    TextureType( texture, copyop ),
    m_min( texture.m_min ),
    m_scale( texture.m_scale )
{
    // initialize members
}

template < typename TextureType >
WGEScaledTexture< TextureType >::~WGEScaledTexture()
{
    // cleanup.
}

template < typename TextureType >
double WGEScaledTexture< TextureType >::getMin() const
{
    return m_min;
}

template < typename TextureType >
double WGEScaledTexture< TextureType >::getScale() const
{
    return m_scale;
}

template < typename TextureType >
void WGEScaledTexture< TextureType >::bind( osg::ref_ptr< osg::Node > node, size_t unit )
{
    // let our utilities do the work
    wge::bindTexture( node, this, unit ); // to avoid recursive stuff -> explicitly specify the type
}

// Some convenience typedefs

/**
 * OSG's Texture1D with scaling features
 */
typedef WGEScaledTexture< osg::Texture1D > WGEScaledTexture1D;

/**
 * OSG's Texture2D with scaling features
 */
typedef WGEScaledTexture< osg::Texture2D > WGEScaledTexture2D;

/**
 * OSG's Texture3D with scaling features
 */
typedef WGEScaledTexture< osg::Texture3D > WGEScaledTexture3D;

#endif  // WGESCALEDTEXTURE_H

