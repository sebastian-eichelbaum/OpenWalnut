
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

#ifndef WGEPOSTPROCESSOR_FRAGMENT_GLSL
#define WGEPOSTPROCESSOR_FRAGMENT_GLSL

#version 120

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"

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
uniform sampler2D u_texture4Sampler;
#define u_noiseSampler u_texture4Sampler

/**
 * The tangent data
 */
uniform sampler2D u_texture5Sampler;
#define u_tangentSampler u_texture5Sampler

/**
 * The white-noise 3 channel texture: size in x direction
 */
uniform int u_texture3SizeX;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeX;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeY;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeZ;

/**
 * Offset to access the neighbouring pixel in a texture.
 */
float offsetX = 1.0 / u_texture0SizeX;

/**
 * Offset to access the neighbouring pixel in a texture.
 */
float offsetY = 1.0 / u_texture0SizeY;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Varying
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The final color is stored in this global variable
 */
vec4 color = vec4( 1.0 );

/**
 * If this value is used as in blend() and is 0.0 if no color has been set to the color variable. If it is 0.5, color has been set earlier. This
 * is imply needed to properly blend the colors together without the need of a "initial"-color.
 */
float colorSet = 0.0;   // is 0.5 if the color has been set earlier

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
//  * Blending utilities
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

/**
 * Blends the specified color to the current overall-color.
 *
 * \param newColor the new color to blend in.
 */
void blend( in vec4 newColor )
{
    color = mix( color, newColor, 1.0 - colorSet );
    colorSet = 0.5; // we definitely set the color.
}

/**
 * Scales the current color with the specified scaling.
 *
 * \param f the scaling factor
 */
void blendScale( in float f )
{
    color.rgb = mix( color.rgb * f, vec3( f ), 1.0 - ( 2.0 * colorSet ) );
    colorSet = 0.5; // we definitely set the color.
}

/**
 * Adds the specified color to the current color.
 *
 * \param newColor the color to add
 */
void blendAdd( in vec4 newColor )
{
    color = mix( color + newColor, newColor, 1.0 - ( 2.0 * colorSet ) );
    colorSet = 0.5; // we definitely set the color.
}

/**
 * Adds the specified value as vec4 to the current color.
 *
 * \param f the scaling factor
 */
void blendAdd( in float f )
{
    blendAdd( vec4( vec3( f ), 1.0 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Postprocessors
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Phong based Per-Pixel-Lighting.
 *
 * \param normal the surface normal. Normalized.
 * \param lightParams the ligh parameters
 *
 * \return the intensity.
 */
float getPPLPhong( in wge_LightIntensityParameter lightParams, in vec3 normal )
{
    return blinnPhongIlluminationIntensity( lightParams, normal );
}

/**
 * Phong based Per-Pixel-Lighting.
 *
 * \param normal the surface normal. Normalized.
 *
 * \return the intensity.
 */
float getPPLPhong( in vec3 normal )
{
    return blinnPhongIlluminationIntensity( normal );
}

/**
 * Phong based Per-Pixel-Lighting based on the specified color.
 *
 * \return the new lighten color.
 */
float getPPLPhong()
{
    return getPPLPhong( getNormal().xyz );
}

/**
 * Phong based Per-Pixel-Lighting based on the specified color.
 *
 * \param lightParams the ligh parameters
 *
 * \return the intensity.
 */
float getPPLPhong( in wge_LightIntensityParameter lightParams )
{
    return getPPLPhong( lightParams, getNormal().xyz );
}

/**
 * Apply laplace-filter to depth buffer. An edge is > 0.0.
 *
 * \return the edge
 */
float getEdge()
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    /////////////////////////////////////////////////////////////////////////////////////////////
    // GETTING TEXELS
    //
    // Get surrounding texels; needed for ALL filters
    /////////////////////////////////////////////////////////////////////////////////////////////

    float edgec  = getDepth( pixelCoord );
    float edgebl = getDepth( pixelCoord + vec2( -offsetX, -offsetY ) );
    float edgel  = getDepth( pixelCoord + vec2( -offsetX,     0.0  ) );
    float edgetl = getDepth( pixelCoord + vec2( -offsetX,  offsetY ) );
    float edget  = getDepth( pixelCoord + vec2(     0.0,   offsetY ) );
    float edgetr = getDepth( pixelCoord + vec2(  offsetX,  offsetY ) );
    float edger  = getDepth( pixelCoord + vec2(  offsetX,     0.0  ) );
    float edgebr = getDepth( pixelCoord + vec2(  offsetX,  offsetY ) );
    float edgeb  = getDepth( pixelCoord + vec2(     0.0,  -offsetY ) );

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LAPLACE
    //
    // apply a standart laplace filter kernel
    /////////////////////////////////////////////////////////////////////////////////////////////

    // laplace filter kernel
    float edge = 16.0 * abs(
            0.0 * edgetl +  1.0 * edget + 0.0 * edgetr +
            1.0 * edgel  +  -4.0 * edgec + 1.0 * edger  +
            0.0 * edgebl +  1.0 * edgeb + 0.0 * edgebr
        );

    // MPI Paper Hack, to make edges black and surrundings white
    // return 1 - edge;

    return edge;
}

/**
 * Returns the gauss-smoothed color of the specified pixel from the input color texture.
 *
 * \param where the pixel to grab
 *
 * \return the color
 */
vec4 getGaussedColor( vec2 where )
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    // get the 8-neighbourhood
    vec4 gaussedColorc  = getColor( where );
    vec4 gaussedColorbl = getColor( where + vec2( -offsetX, -offsetY ) );
    vec4 gaussedColorl  = getColor( where + vec2( -offsetX,     0.0  ) );
    vec4 gaussedColortl = getColor( where + vec2( -offsetX,  offsetY ) );
    vec4 gaussedColort  = getColor( where + vec2(     0.0,   offsetY ) );
    vec4 gaussedColortr = getColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorr  = getColor( where + vec2(  offsetX,     0.0  ) );
    vec4 gaussedColorbr = getColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorb  = getColor( where + vec2(     0.0,  -offsetY ) );

    // apply Gauss filter
    vec4 gaussed = ( 1.0 / 16.0 ) * (
            1.0 * gaussedColortl +  2.0 * gaussedColort + 1.0 * gaussedColortr +
            2.0 * gaussedColorl  +  4.0 * gaussedColorc + 2.0 * gaussedColorr  +
            1.0 * gaussedColorbl +  2.0 * gaussedColorb + 1.0 * gaussedColorbr );
    return gaussed;
}

/**
 * Returns the gauss-smoothed color of the current pixel from the input color texture.
 *
 * \return  the color
 */
vec4 getGaussedColor()
{
    return getGaussedColor( pixelCoord );
}

/**
 * Returns the gauss-smoothed depth of the specified ixel from the input color texture.
 *
 * \param where the point to grab
 *
 * \return the gaussed depth
 */
float getGaussedDepth( vec2 where )
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    // get the 8-neighbourhood
    float gaussedDepthc  = getDepth( where );
    float gaussedDepthbl = getDepth( where + vec2( -offsetX, -offsetY ) );
    float gaussedDepthl  = getDepth( where + vec2( -offsetX,     0.0  ) );
    float gaussedDepthtl = getDepth( where + vec2( -offsetX,  offsetY ) );
    float gaussedDeptht  = getDepth( where + vec2(     0.0,   offsetY ) );
    float gaussedDepthtr = getDepth( where + vec2(  offsetX,  offsetY ) );
    float gaussedDepthr  = getDepth( where + vec2(  offsetX,     0.0  ) );
    float gaussedDepthbr = getDepth( where + vec2(  offsetX,  offsetY ) );
    float gaussedDepthb  = getDepth( where + vec2(     0.0,  -offsetY ) );

    // apply Gauss filter
    float gaussed = ( 1.0 / 16.0 ) * (
            1.0 * gaussedDepthtl +  2.0 * gaussedDeptht + 1.0 * gaussedDepthtr +
            2.0 * gaussedDepthl  +  4.0 * gaussedDepthc + 2.0 * gaussedDepthr  +
            1.0 * gaussedDepthbl +  2.0 * gaussedDepthb + 1.0 * gaussedDepthbr );
    return gaussed;
}

/**
 * Returns the gauss-smoothed depth of the current pixel from the input depth texture.
 *
 * \return the gaussed depth
 */
float getGaussedDepth()
{
    return getGaussedDepth( pixelCoord );
}

/**
 * Calculate the cel-shading of a specified color. The number of colors is defined by the u_celShadingSamples uniform.
 *
 * \param inColor the color to shade
 *
 * \return the cel-shaded color
 */
vec4 getCelShading( vec4 inColor )
{
    // TODO(ebaum): provide uniform for this
    float samples = 5.0;
    return vec4(
            vec3( int( ( inColor * samples ).r ),
                  int( ( inColor * samples ).g ),
                  int( ( inColor * samples ).b ) ) / samples, inColor.a );
}

/**
 * Calculate the cel-shading of the input color. The number of colors is defined by the u_celShadingSamples uniform.
 *
 * \return the cel-shaded color
 */
vec4 getCelShading()
{
    return getCelShading( getColor() );
}

/**
 * Returns a fade-out factor basing on pixel depth for the specified pixel
 *
 * \param where pixel coordinate
 *
 * \return the fade-out factor.
 */
float getDepthFading( vec2 where )
{
    // TODO(ebaum): add uniforms for this limits
    float invD = ( 1.0 - getDepth( where ) );
    return smoothstep( 0.2, 0.5, invD );
}

/**
 * Returns a fade-out factor basing on pixel depth for the current pixel.
 *
 * \return the fade-out factor.
 */
float getDepthFading()
{
    return getDepthFading( pixelCoord );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main. Apply the specified post-processors.
 */
void main()
{
    // don't do this stuff for background pixel
    float depth = getDepth();
    gl_FragDepth = depth;
    if( depth > 0.99 )
    {
        discard;
    }

    // NOTE: Although the GLSL compiler might not be the most intelligent one, it will most probably be smart enough the reduce many texture
    // fetch operations on the same sampler and same position to one fetch and provides the result in a variable. So it is not stupid to use
    // getColor or getNormal or getDepth many times on the same u,v coordinate.

#ifdef WGE_POSTPROCESSOR_COLOR
    blend( getColor() );
#endif

#ifdef WGE_POSTPROCESSOR_GAUSSEDCOLOR
    blend( getGaussedColor() );
#endif

#ifdef WGE_POSTPROCESSOR_PPLPHONG
    blendScale( getPPLPhong() );
#endif

#ifdef WGE_POSTPROCESSOR_CELSHADING
    blend( getCelShading() );
#endif

#ifdef WGE_POSTPROCESSOR_DEPTHFADING
    blendScale( getDepthFading() );
#endif

#ifdef WGE_POSTPROCESSOR_EDGE
    blendAdd( getEdge() );
#endif

#ifdef WGE_POSTPROCESSOR_DEPTH
    blendScale( depth );
#endif

#ifdef WGE_POSTPROCESSOR_GAUSSEDDEPTH
    blendScale( getGaussedDepth() );
#endif

#ifdef WGE_POSTPROCESSOR_NORMAL
    blend( abs( getNormal() ) );
#endif

#ifdef WGE_POSTPROCESSOR_CUSTOM
    vec4 customColor = vec4( 0.0 );
    %WGEPostprocessor-CustomCode%
    blend( customColor );
#endif

    // output the depth and final color.
    gl_FragColor = color;
}

#endif // WGEPOSTPROCESSOR_FRAGMENT_GLSL

