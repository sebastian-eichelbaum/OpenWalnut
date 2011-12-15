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
#include "WGEPostprocessorUtils-fragment.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Uniforms
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// These are NOT specific to any postprocessor

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Varying
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Postprocessors
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef WGE_POSTPROCESSOR_EDGE

/**
 * The threshold used to clip away "unwanted" borders. Basically removes noise
 */
uniform float u_edgeEdgeThresholdLower = 0.25;
uniform float u_edgeEdgeThresholdUpper = 0.75;

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

    return smoothstep( u_edgeEdgeThresholdLower, u_edgeEdgeThresholdUpper, edge );
}

#endif

#ifdef WGE_POSTPROCESSOR_CEL

/**
 * The number of bins to use for cel shading
 */
uniform int u_celShadingBins = 2;

/**
 * Calculate the cel-shading of a specified color. The number of colors is defined by the u_celShadingSamples uniform.
 *
 * \param inColor the color to shade
 *
 * \return the cel-shaded color
 */
vec4 getCelShading( vec4 inColor )
{
    float samples = float( u_celShadingBins );
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

#endif

#ifdef WGE_POSTPROCESSOR_GAUSS

/**
 * Returns the gauss-smoothed color of the specified pixel from the input texture.
 *
 * \param where the pixel to grab
 * \param sampler the texture to gauss
 *
 * \return the color
 */
vec4 getGaussedColor( vec2 where, sampler2D sampler )
{
    // get the 8-neighbourhood
    vec4 gaussedColorc  = texture2D( sampler, where );
    vec4 gaussedColorbl = texture2D( sampler, where + vec2( -offsetX, -offsetY ) );
    vec4 gaussedColorl  = texture2D( sampler, where + vec2( -offsetX,     0.0  ) );
    vec4 gaussedColortl = texture2D( sampler, where + vec2( -offsetX,  offsetY ) );
    vec4 gaussedColort  = texture2D( sampler, where + vec2(     0.0,   offsetY ) );
    vec4 gaussedColortr = texture2D( sampler, where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorr  = texture2D( sampler, where + vec2(  offsetX,     0.0  ) );
    vec4 gaussedColorbr = texture2D( sampler, where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorb  = texture2D( sampler, where + vec2(     0.0,  -offsetY ) );

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
 * \param sampler the texture to gauss
 *
 * \return the color
 */
vec4 getGaussedColor( sampler2D sampler )
{
    return getGaussedColor( pixelCoord, sampler );
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main. Apply the specified post-processors.
 */
void main()
{
    // don't do this stuff for background pixel
    gl_FragData[0] = vec4( 1.0, 0.0, 0.0, 1.0 );

#ifdef WGE_POSTPROCESSOR_EDGE
    // output the depth and final color.
    #ifdef WGE_POSTPROCESSOR_OUTPUT_COMBINE

        // for black borders, multiply edge with color
        #ifdef WGE_POSTPROCESSOR_EDGE_BLACKEDGE
            #define EDGEOP * ( 1.0 - getEdge() )
        #else
            // for white borders, add
            #define EDGEOP + vec3( getEdge() )
        #endif

        // apply operation and output color
        gl_FragData[0] = vec4( getColor().rgb EDGEOP, 1.0 );
    #else
        // also do this for the effect-only output
        #ifdef WGE_POSTPROCESSOR_EDGE_BLACKEDGE
            #define EDGEOP 1.0 -
        #else
            // for white borders, add
            #define EDGEOP 0.0 +
        #endif
        gl_FragData[0] = vec4( vec3( EDGEOP getEdge() ), 1.0 );
    #endif
#endif

#ifdef WGE_POSTPROCESSOR_CEL
    // output the depth and final color.
    gl_FragData[0] = getCelShading();
#endif

#ifdef WGE_POSTPROCESSOR_GAUSS
    // output the depth and final color.
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT0
        gl_FragData[0] = getGaussedColor( pixelCoord, u_texture0Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT1
        gl_FragData[1] = getGaussedColor( pixelCoord, u_texture1Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT2
        gl_FragData[2] = getGaussedColor( pixelCoord, u_texture2Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT3
        gl_FragData[3] = getGaussedColor( pixelCoord, u_texture3Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT4
        gl_FragData[4] = getGaussedColor( pixelCoord, u_texture4Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT5
        gl_FragData[5] = getGaussedColor( pixelCoord, u_texture5Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT6
        gl_FragData[6] = getGaussedColor( pixelCoord, u_texture6Sampler );
    #endif
    #ifdef WGE_POSTPROCESSOR_GAUSS_UNIT7
        gl_FragData[7] = getGaussedColor( pixelCoord, u_texture7Sampler );
    #endif
#endif

#ifdef WGE_POSTPROCESSOR_MERGEOP
    // output the depth and final color.
    #ifdef WGE_POSTPROCESSOR_MERGEOP_CUSTOM
        vec4 color = vec4( 1.0 );
        %WGE_POSTPROCESSOR_MERGEOP_CODE%
        gl_FragData[0] = color;
    #else
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT0
            vec4 color = texture2D( u_texture0Sampler, pixelCoord );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT1
            color = mix( color, texture2D( u_texture1Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT2
            color = mix( color, texture2D( u_texture2Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT3
            color = mix( color, texture2D( u_texture3Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT4
            color = mix( color, texture2D( u_texture4Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT5
            color = mix( color, texture2D( u_texture5Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT6
            color = mix( color, texture2D( u_texture6Sampler, pixelCoord ), 0.5 );
        #endif
        #ifdef WGE_POSTPROCESSOR_MERGEOP_UNIT7
            color = mix( color, texture2D( u_texture7Sampler, pixelCoord ), 0.5 );
        #endif
        gl_FragData[0] = color;
    #endif
#endif
}

#endif // WGEPOSTPROCESSOR_FRAGMENT_GLSL

