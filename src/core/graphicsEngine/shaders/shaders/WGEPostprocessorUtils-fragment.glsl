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

#version 130

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
 * The tangent data
 */
uniform sampler2D u_texture4Sampler;
#define u_tangentSampler u_texture4Sampler

/**
 * The white-noise 3 channel texture: sampler
 */
uniform sampler2D u_texture5Sampler;
#define u_noiseSampler u_texture5Sampler

/**
 * Texture in unit 6
 */
uniform sampler2D u_texture6Sampler;

/**
 * Texture in unit 7
 */
uniform sampler2D u_texture7Sampler;

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
 * Grabs and unscales the value inside the texture and returns it. Although this is implemented in WGETextureTools, we re-implement it
 * for having mip-map support
 *
 * \param texture the texture unit to use
 * \param point   the texture coordinates
 * \param minimum the minumum value of all values inside the texture
 * \param scale   the scaling value for all values inside the texture
 * \param lod the LOD level if using mip-maps.
 *
 * \note The minimum and scale values are normally transferred to the shader using uniforms, as the CPU scales the textures
 *
 * \return the value at the given point
 */
vec4 texture2DUnscaledLOD( sampler2D texture, vec2 point, float minimum, float scale, float lod = 0.0 )
{
    return ( scale * texture2DLod( texture, point, lod ) ) + vec4( minimum );
}


/**
 * Returns the original unprocessed color value at the specified point
 *
 * \param where the pixel to grab
 * \param lod the LOD level if using mip-maps.
 *
 * \return the color
 */
vec4 getColor( in vec2 where, in float lod = 0.0 )
{
    return texture2DLod( u_colorSampler, where, lod );
}

/**
 * Returns the original unprocessed color value at the current pixel.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 * \param lod the LOD level if using mip-maps.
 *
 * \return the color
 */
vec4 getColor( in float lod = 0.0 )
{
    return getColor( pixelCoord, lod );
}

/**
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1] and normalized The w component is 1.
 *
 * \param where the pixel to grab
 * \param lod the LOD level if using mip-maps.
 *
 * \return the normal
 */
vec4 getNormal( in vec2 where, in float lod = 0.0 )
{
    return normalize( texture2DUnscaledLOD( u_normalSampler, where, -1.0, 2.0, lod ).xyz ).xyzz * zeroOneList.xxxy + zeroOneList.yyyx;
}

/**
 * Grabs the normal at the current pixel. The returned normal has been de-scaled to [-1,1]. The w component is 1.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 * \param lod the LOD level if using mip-maps.
 *
 * \return the normal
 */
vec4 getNormal( in float lod = 0.0 )
{
    return getNormal( pixelCoord, lod );
}

/**
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1] and normalized The w component is 1.
 *
 * \param where the pixel to grab
 * \param lod the LOD level if using mip-maps.
 *
 * \return the normal
 */
vec4 getTangent( in vec2 where, in float lod = 0.0 )
{
    return normalize( texture2DUnscaledLOD( u_tangentSampler, where, -1.0, 2.0, lod ).xyz ).xyzz * zeroOneList.xxxy + zeroOneList.yyyx;
}

/**
 * Grabs the normal at the current pixel. The returned normal has been de-scaled to [-1,1]. The w component is 1.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \param lod the LOD level if using mip-maps.
 *
 * \return the normal
 */
vec4 getTangent( in float lod = 0.0 )
{
    return getTangent( pixelCoord, lod );
}

/**
 * Grabs the depth at the specified point.
 *
 * \param where the position where to grab it.
 * \param lod the LOD level if using mip-maps.
 *
 * \return the depth
 */
float getDepth( in vec2 where, in float lod = 0.0 )
{
    return texture2DLod( u_depthSampler, where, lod ).r;
}

/**
 * Grabs the depth at the current pixel.
 *
 * \note GLSL does not officially allow default values for function arguments which is why we need this additional function.
 *
 * \param lod the LOD level if using mip-maps.
 *
 * \return the depth
 */
float getDepth( in float lod = 0.0 )
{
    return getDepth( pixelCoord, lod );
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
    return texture2D( u_parameterSampler, pixelCoord ).r;
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

/**
 * Grab a vector from the noise texture. You should of course bind a RGB texture to u_noiseSampler.
 *
 * \param where position
 *
 * \return the unscaled vector in [-1,1] for each component
 */
vec3 getNoiseAsVector( in vec2 where )
{
    return texture2D( u_noiseSampler, where ).rgb * 2.0 - vec3( 1.0 );
}

/**
 * Grab a vector from the noise texture. You should of course bind a RGB texture to u_noiseSampler.
 *
 * \return the unscaled vector in [-1,1] for each component
 */
vec3 getNoiseAsVector()
{
    return getNoiseAsVector( pixelCoord );
}

#endif // WGEPOSTPROCESSORUTILS_FRAGMENT_GLSL

