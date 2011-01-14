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

#version 120

#include "WGETextureTools.glsl"

/**
 * The texture Unit for the original color field
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the Normal Map
 */
uniform sampler2D u_texture1Sampler;

/**
 * The depth texture
 */
uniform sampler2D u_texture2Sampler;

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
 * The final color is stored in this global variable
 */
vec4 color = vec4( 0.0 );

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
 * Returns the original unprocessed color value at the specified point
 *
 * \param where the pixel to grab
 *
 * \return the color
 */
vec4 getColor( in vec2 where )
{
    return texture2D( u_texture0Sampler, where );
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
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1]. The w component is 1.
 *
 * \param where the pixel to grab
 *
 * \return the normal
 */
vec4 getNormal( in vec2 where )
{
    const vec2 constantList = vec2( 1.0, 0.0 );
    return texture2DUnscaled( u_texture1Sampler, where, -1.0, 2.0 ).xyzw * constantList.xxxy + constantList.yyyx;
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
 * Grabs the depth at the specified point.
 *
 * \param where the position where to grab it.
 *
 * \return the depth
 */
float getDepth( in vec2 where )
{
    return texture2D( u_texture2Sampler, where ).r;
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
 * Main. Calculates the Laplace Filter for each pixel.
 */
void main()
{
    // where are we?

    // get data of surrounding textels using this offsets
    float offsetX = 1.0 / u_texture0SizeX;
    float offsetY = 1.0 / u_texture0SizeY;

#ifdef WGE_POSTPROCESSOR_PPLPHONG
    blend( getColor() );
#endif

#ifdef WGE_POSTPROCESSOR_SSAO
    blend( vec4( 1.0, 0.0, 0.0, 1.0 ) );
#endif

#ifdef WGE_POSTPROCESSOR_COLOR
    blend( getColor() );
#endif

#ifdef WGE_POSTPROCESSOR_GAUSSEDCOLOR
    // get the 8-neighbourhood
    vec4 gaussedColorc  = getColor( pixelCoord );
    vec4 gaussedColorbl = getColor( pixelCoord + vec2( -offsetX, -offsetY ) );
    vec4 gaussedColorl  = getColor( pixelCoord + vec2( -offsetX,     0.0  ) );
    vec4 gaussedColortl = getColor( pixelCoord + vec2( -offsetX,  offsetY ) );
    vec4 gaussedColort  = getColor( pixelCoord + vec2(     0.0,   offsetY ) );
    vec4 gaussedColortr = getColor( pixelCoord + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorr  = getColor( pixelCoord + vec2(  offsetX,     0.0  ) );
    vec4 gaussedColorbr = getColor( pixelCoord + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorb  = getColor( pixelCoord + vec2(     0.0,  -offsetY ) );

    // apply gauss filter
    vec4 gaussed = ( 1.0 / 16.0 ) * (
            1.0 * gaussedColortl +  2.0 * gaussedColort + 1.0 * gaussedColortr +
            2.0 * gaussedColorl  +  4.0 * gaussedColorc + 2.0 * gaussedColorr  +
            1.0 * gaussedColorbl +  2.0 * gaussedColorb + 1.0 * gaussedColorbr );

    blend( gaussed );
#endif

#ifdef WGE_POSTPROCESSOR_EDGE

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
    float edge = 1.0 * abs(
            0.0 * edgetl +  1.0 * edget + 0.0 * edgetr +
            1.0 * edgel  +  -4.0 * edgec + 1.0 * edger  +
            0.0 * edgebl +  1.0 * edgeb + 0.0 * edgebr
        );
    blend( vec4( vec3( 16.0 * edge ), 1.0 ) );
#endif

#ifdef WGE_POSTPROCESSOR_DEPTH
    blend( vec4( vec3( getDepth() ), 1.0 ) );
#endif

#ifdef WGE_POSTPROCESSOR_NORMAL
    blend( getNormal() );
#endif

#ifdef WGE_POSTPROCESSOR_CUSTOM
    blend( vec4( 1.0, 0.0, 0.0, 1.0 ) );
#endif

    // output the depth and final color.
    gl_FragColor = color;
    gl_FragDepth = getDepth();
}

