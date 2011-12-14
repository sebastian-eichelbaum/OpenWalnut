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

#ifndef WGEPOSTPROCESSORUTILS_FRAGMENT_GLSL
#define WGEPOSTPROCESSORUTILS_FRAGMENT_GLSL

#version 120

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input-Texture Uniforms
//  NOTE: do not put post-processor specific uniforms here!
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The texture Unit for the original color field
 */
uniform sampler2D u_texture0Sampler;
#define u_colorSampler u_texture0Sampler

/**
 * The texture Unit for the Normal Map
 */
uniform sampler2D u_texture1Sampler;
#define u_normalSampler u_texture1Sampler

/**
 * The additional parameters from the source
 */
uniform sampler2D u_texture2Sampler;
#define u_parameterSampler u_texture2Sampler

/**
 * The depth texture
 */
uniform sampler2D u_texture3Sampler;
#define u_depthSampler u_texture3Sampler

/**
 * The white-noise 3 channel texture: sampler
 */
uniform sampler2D u_texture5Sampler;
#define u_noiseSampler u_texture5Sampler

/**
 * The tangent data
 */
uniform sampler2D u_texture4Sampler;
#define u_tangentSampler u_texture4Sampler

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeX;
#define u_colorSizeX u_texture0SizeX

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeY;
#define u_colorSizeY u_texture0SizeY

/**
 * Offset to access the neighbouring pixel in a texture.
 */
float offsetX = 1.0 / u_texture0SizeX;

/**
 * Offset to access the neighbouring pixel in a texture.
 */
float offsetY = 1.0 / u_texture0SizeY;

/**
 * The white-noise 3 channel texture: size in x direction
 */
uniform int u_texture5SizeX;
#define u_noiseSizeX u_texture5SizeX

/**
 * The white-noise 3 channel texture: size in y direction
 */
uniform int u_texture5SizeY;
#define u_noiseSizeYX u_texture5SizeY

/**
 * The coordinate of the current pixel for texture lookup
 */
vec2 pixelCoord = gl_TexCoord[0].st;

/**
 * This is needed for some swizzle tricks
 */
const vec2 zeroOneList = vec2( 1.0, 0.0 );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions.
//  * Get color at certain point
//  * Get normal and depth at certain point
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Returns the original unprocessed color value at the specified point
 *
 * \param where the pixel to grab
 *
 * \return the color
 */
vec4 getColor( in vec2 where )
{
    return texture2D( u_colorSampler, where );
}

/**
 * Returns the original unprocessed color value at the current pixel.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \return the color
 */
vec4 getColor()
{
    return getColor( pixelCoord );
}

/**
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1] and normalized The w component is 1.
 *
 * \param where the pixel to grab
 *
 * \return the normal
 */
vec4 getNormal( in vec2 where )
{
    return normalize( texture2DUnscaled( u_normalSampler, where, -1.0, 2.0 ).xyz ).xyzz * zeroOneList.xxxy + zeroOneList.yyyx;
}

/**
 * Grabs the normal at the current pixel. The returned normal has been de-scaled to [-1,1]. The w component is 1.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \return the normal
 */
vec4 getNormal()
{
    return getNormal( pixelCoord );
}

/**
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1] and normalized The w component is 1.
 *
 * \param where the pixel to grab
 *
 * \return the normal
 */
vec4 getTangent( in vec2 where )
{
    return normalize( texture2DUnscaled( u_tangentSampler, where, -1.0, 2.0 ).xyz ).xyzz * zeroOneList.xxxy + zeroOneList.yyyx;
}

/**
 * Grabs the normal at the current pixel. The returned normal has been de-scaled to [-1,1]. The w component is 1.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \return the normal
 */
vec4 getTangent()
{
    return getNormal( pixelCoord );
}

/**
 * Grabs the depth at the specified point.
 *
 * \param where the position where to grab it.
 *
 * \return the depth
 */
float getDepth( in vec2 where )
{
    return texture2D( u_depthSampler, where ).r;
}

/**
 * Grabs the depth at the current pixel.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \return the depth
 */
float getDepth()
{
    return getDepth( pixelCoord );
}

/**
 * Returns the zoom factor for the current pixel if set by the output shader of the geometry getting post-processed here.
 *
 * \param where the pixel to grab
 *
 * \return the zoom factor
 */
float getZoom( in vec2 where )
{
    // TODO(ebaum): somehow remove this scaler
    return texture2D( u_parameterSampler, pixelCoord ).r * 10.0;
}

/**
 * Returns the zoom factor for the current pixel if set by the output shader of the geometry getting post-processed here.
 *
 * \return the zoom factor
 */
float getZoom()
{
    return getZoom( pixelCoord );
}

#endif // WGEPOSTPROCESSORUTILS_FRAGMENT_GLSL

