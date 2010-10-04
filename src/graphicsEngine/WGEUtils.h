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

#ifndef WGEUTILS_H
#define WGEUTILS_H

#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <osg/Array>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Camera>

#include "../common/WColor.h"
#include "../common/WAssert.h"
#include "../common/math/WPosition.h"
#include "../common/math/WMatrix.h"
#include "WExportWGE.h"

template < typename T > class WGEScaledTexture;

namespace wge
{
    /**
     * Transforms a direction given via two points into a RGB color.
     *
     * \param pos1 First point
     * \param pos2 Second point
     */
    WColor getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 );

    /**
     * Converts a WColor to an OSG compatible color
     *
     * \param color The color in WColor format
     * \return A color which may be used inside of OSG
     */
    osg::Vec4 osgColor( const WColor& color );

    /**
     * Converts a given WPosition into an osg::Vec3.
     *
     * \param pos The WPosition which should be converted
     *
     * \return The osg::Vec3 vector of pos
     */
    osg::Vec3 osgVec3( const wmath::WPosition& pos );

    /**
     * Converts a whole vector of WPositions into an osg::Vec3Array.
     *
     * \param posArray The given positions vector
     *
     * \return Refernce to the same vector but as osg::Vec3Array.
     */
    osg::ref_ptr< osg::Vec3Array > WGE_EXPORT osgVec3Array( const std::vector< wmath::WPosition >& posArray );

    /**
     * Converts screen coordinates into Camera coordinates.
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     */
    osg::Vec3 WGE_EXPORT unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * Conversion of WVector3D to osg::Vec3
     * \param v the vector to convert.
     */
    osg::Vec3 wv3D2ov3( wmath::WVector3D v );

    /**
     * creates the same color as the atlas colormap shader from the index
     *
     * \param index unsigned char that indexes the color
     * \return the color
     */
    WColor WGE_EXPORT createColorFromIndex( int index );

    /**
     * creates a rgb WColor from a HSV value
     * \param h hue
     * \param s saturation
     * \param v value
     * \return the color
     */
    WColor WGE_EXPORT createColorFromHSV( int h, float s = 1.0, float v = 1.0 );

    /**
     * creates the nth color of a partition of the hsv color circle
     *
     * \param n number of the color
     * \param parts partition size
     * \return the color
     */
    WColor WGE_EXPORT getNthHSVColor( int n, int parts = 10 );

    /**
     * This method converts an WMatrix to the corresponding osg::Matrixd.
     *
     * \param matrix the matrix to convert
     *
     * \return the osg matrix.
     */
    osg::Matrixd WGE_EXPORT toOSGMatrix( const wmath::WMatrix<double>& matrix );

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
     * \tparam T the type of texture. Usually osg::Texture3D or osg::Texture2D.
     */
    template < typename T >
    void WGE_EXPORT bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< T > texture, size_t unit = 0 );

    /**
     * Binds the specified texture to the specified unit. It automatically adds several uniforms which then can be utilized in the shader:
     * - u_textureXUnit: the unit number (useful for accessing correct gl_TexCoord and so on)
     * - u_textureXSampler: the needed sampler
     * - u_textureXSizeX: width of the texture in pixels
     * - u_textureXSizeY: height of the texture in pixels
     * - u_textureXSizeZ: depth of the texture in pixels
     * If the specified texture is a WGEScaledTexture, it additionally adds u_textureXMin and u_textureXScale for unscaling.
     *
     * \param node where to bind
     * \param unit the unit to use
     * \param texture the texture to use.
     * \tparam T the type of texture. Usually osg::Texture3D or osg::Texture2D.
     */
    template < typename T >
    void WGE_EXPORT bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WGEScaledTexture< T > > texture, size_t unit = 0 );
}

inline WColor wge::getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 )
{
    wmath::WPosition direction( ( pos2 - pos1 ) );
    direction.normalize();
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ) );
}

inline osg::Vec4 wge::osgColor( const WColor& color )
{
    return osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() );
}

inline osg::Vec3 wge::osgVec3( const wmath::WPosition& pos )
{
    return osg::Vec3( pos[0], pos[1], pos[2] );
}

inline osg::Vec3 wge::wv3D2ov3( wmath::WVector3D v )
{
    return osg::Vec3( v[0], v[1], v[2] );
}

inline osg::Matrixd WGE_EXPORT wge::toOSGMatrix( const wmath::WMatrix<double>& matrix )
{
    WAssert( ( matrix.getNbRows() == 3 || matrix.getNbRows() == 4 ) && ( matrix.getNbCols() == 3 || matrix.getNbCols() == 4 ),
             "Only 3x3 or 4x4 matrices allowed." );

    // handle 4x4 and 3x3 separately
    if ( matrix.getNbRows() == 4 )
    {
        return osg::Matrixd( matrix[ 0 ], matrix[ 4 ], matrix[ 8 ], matrix[ 12 ],
                             matrix[ 1 ], matrix[ 5 ], matrix[ 9 ], matrix[ 13 ],
                             matrix[ 2 ], matrix[ 6 ], matrix[ 10 ], matrix[ 14 ],
                             matrix[ 3 ], matrix[ 7 ], matrix[ 11 ], matrix[ 15 ]
                           );
    }
    else
    {
        return osg::Matrixd( matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], 0.0,
                             matrix[ 3 ], matrix[ 4 ], matrix[ 5 ], 0.0,
                             matrix[ 6 ], matrix[ 7 ], matrix[ 8 ], 0.0,
                             matrix[ 9 ], matrix[ 10 ], matrix[ 11 ], 1.0
                           );
    }
}

template < typename T >
void wge::bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< T > texture, size_t unit )
{
    std::string prefix = "u_texture" + boost::lexical_cast< std::string >( unit );

    osg::StateSet* state = node->getOrCreateStateSet();
    state->setTextureAttributeAndModes( unit, texture, osg::StateAttribute::ON );
    state->addUniform( new osg::Uniform( ( prefix + "Sampler" ).c_str(), static_cast< int >( unit ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "Unit" ).c_str(), static_cast< int >( unit ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeX" ).c_str(), static_cast< int >( texture->getTextureWidth() ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeY" ).c_str(), static_cast< int >( texture->getTextureHeight() ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "SizeZ" ).c_str(), static_cast< int >( texture->getTextureDepth() ) ) );
}

template < typename T >
void wge::bindTexture( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WGEScaledTexture< T > > texture, size_t unit )
{
    wge::bindTexture< T >( node, osg::ref_ptr< T >( texture ), unit );

    std::string prefix = "u_texture" + boost::lexical_cast< std::string >( unit );

    // add some additional uniforms containing scaling information
    osg::StateSet* state = node->getOrCreateStateSet();
    state->addUniform( new osg::Uniform( ( prefix + "Min" ).c_str(), static_cast< float >( texture->getMin() ) ) );
    state->addUniform( new osg::Uniform( ( prefix + "Scale" ).c_str(), static_cast< float >( texture->getScale() ) ) );
}

#endif  // WGEUTILS_H

