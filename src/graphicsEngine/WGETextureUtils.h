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

#ifndef WGETEXTUREUTILS_H
#define WGETEXTUREUTILS_H

#include <string>

#include <osg/Node>
#include <osg/StateSet>

#include <boost/lexical_cast.hpp>

#include "WGEPropertyUniform.h"
#include "WExportWGE.h"

template < typename T > class WGETexture;

namespace wge
{
    /**
     * Binds the specified texture to the specified unit. It automatically adds several uniforms which then can be utilized in the shader:
     * - u_textureXUnit: the unit number (useful for accessing correct gl_TexCoord and so on)
     * - u_textureXSampler: the needed sampler
     * - u_textureXSizeX: width of the texture in pixels
     * - u_textureXSizeY: height of the texture in pixels
     * - u_textureXSizeZ: depth of the texture in pixels
     *
     * \param node where to bind
     * \param unit the unit to use
     * \param texture the texture to use.
     * \param prefix if specified, defines the uniform name prefix. (Sampler, Unit, Sizes, ...)
     * \tparam T the type of texture. Usually osg::Texture3D or osg::Texture2D.
     */
    template < typename T >
    void WGE_EXPORT bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< T > texture, size_t unit = 0, std::string prefix = "" );

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
    template < typename T >
    void WGE_EXPORT bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WGETexture< T > > texture, size_t unit = 0, std::string prefix = ""  );
}

template < typename T >
void wge::bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< T > texture, size_t unit, std::string prefix )
{
    if ( prefix == "" )
    {
        prefix = "u_texture" + boost::lexical_cast< std::string >( unit );
    }

    osg::StateSet* state = node->getOrCreateStateSet();
    state->setTextureAttributeAndModes( unit, texture, osg::StateAttribute::ON );
    state->addUniform( new osg::Uniform( ( prefix + "Sampler" ).c_str(), static_cast< int >( unit ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "Unit" ).c_str(), static_cast< int >( unit ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeX" ).c_str(), static_cast< int >( texture->getTextureWidth() ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeY" ).c_str(), static_cast< int >( texture->getTextureHeight() ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeZ" ).c_str(), static_cast< int >( texture->getTextureDepth() ) ) );
}

template < typename T >
void wge::bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WGETexture< T > > texture, size_t unit, std::string prefix )
{
    if ( prefix == "" )
    {
        prefix = "u_texture" + boost::lexical_cast< std::string >( unit );
    }

    wge::bindTexture< T >( node, osg::ref_ptr< T >( texture ), unit, prefix );

    // add some additional uniforms containing scaling information
    texture->applyUniforms( prefix, node->getOrCreateStateSet() );
}

#endif  // WGETEXTUREUTILS_H


