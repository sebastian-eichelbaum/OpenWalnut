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

#ifndef WGEPOSTPROCESSORCOMBINER_FRAGMENT_GLSL
#define WGEPOSTPROCESSORCOMBINER_FRAGMENT_GLSL

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
 * The depth texture
 */
uniform sampler2D u_texture1Sampler;
#define u_depthSampler u_texture1Sampler


/**
 * Depth shading parameters
 */
uniform float u_depthShadeL = 0.75;
uniform float u_depthShadeU = 1.0;
uniform float u_depthThresholdL = 0.25;
uniform float u_depthThresholdU = 0.75;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Varying
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * The coordinate of the current pixel for texture lookup
 */
vec2 pixelCoord = gl_TexCoord[0].st;

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
 * \param lod the level of detail to get. 0 for unfiltered texture.
 *
 * \return the color
 */
vec4 getColor( in vec2 where, in float lod )
{
    return texture2DLod( u_colorSampler, where, lod );
}

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main. Apply the specified post-processors.
 */
void main()
{
    vec2 coord = pixelCoord;
    float depth = getDepth( coord );

    // interested in rendering only a part of the final texture?
#ifdef TILED
    // modify the pixel coordinate and assume a 3x3 tiled texture you want to use
    // -> pick the tiles accordingly by
    coord +=
        vec2( 0./3., 0./3. );
        //vec2( 1./3., 0./3. );
        //vec2( 2./3., 0./3. );
        //vec2( 0./3., 1./3. );
        //vec2( 1./3., 1./3. );
        //vec2( 2./3., 1./3. );
        //vec2( 0./3., 2./3. );
        //vec2( 1./3., 2./3. );
        //vec2( 2./3., 2./3. );
#endif

    // interested in having a color gradient in your final image?
#ifdef BG_GRADIENT
    if( depth > 0.99 )
    {
        vec3 colorBottom = vec3( 0.0 );
        vec3 colorTop  = vec3( 0.2, 0.2, 0.2 );
        gl_FragDepth = depth;
        gl_FragColor = vec4( mix( colorBottom, colorTop, coord.y ), 1.0 );
        return;
    }
#else
    // avoid drawing any pixels which are "empty" in the source data
    if( depth > 0.99 )
    {
        discard;
    }
#endif

    vec4 finalColor = getColor( coord );

#ifdef DEPTH_SHADING_ENABLED
    // Add some depth-based un-sharpness?
    float dNorm = clamp( ( depth - u_depthThresholdL ) / ( u_depthThresholdU - u_depthThresholdL ), 0.0, 1.0 );

    // Use this to experiment with camera focus and depth of field effects
    /*
    if( depth < u_depthThresholdU )
    {
        finalColor = getColor( coord, 0.0 );
    }
    else
    {
        float numLevels = 4.0;
        if( dNorm < 0.25 )
        {
            finalColor = getColor( coord, 0.0 );
        }
        else
        {
            if ( dNorm < 0.5 )
            {
                finalColor = getColor( coord, 1.0 );
            }
            else
            {
                if ( dNorm < 0.75 )
                {
                    finalColor = getColor( coord, 2.0 );
                }
                else
                {
                    finalColor = getColor( coord, 3.0 );
                }
            }
        }
    }
    */

    // Darken by depth? Scale quadratically.
    float dni = 2.0 * ( 1.0 - dNorm );
    finalColor.rgb *= clamp( dni * dni, u_depthShadeL, u_depthShadeU );
#endif  // DEPTH_SHADING_ENABLED

    // output the depth and final color.
    gl_FragColor = finalColor;
    gl_FragDepth = depth;
}

#endif // WGEPOSTPROCESSORCOMBINER_FRAGMENT_GLSL

