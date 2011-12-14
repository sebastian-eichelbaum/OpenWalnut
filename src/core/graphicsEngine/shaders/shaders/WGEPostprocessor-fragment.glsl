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

#ifdef WGE_POSTPROCESSOR_SSAO

#ifndef WGE_POSTPROCESSOR_SSAO_SAMPLES
    #define WGE_POSTPROCESSOR_SSAO_SAMPLES 16
#endif

/**
 * Overall effect strength. In (0,100]
 */
uniform float u_ssaoTotalStrength = 2.0;

/**
 * Strength of occlusion caused by one occluder. In (0,1]
 */
uniform float u_ssaoStrength = 1.0;

/**
 * Radius in screen-space. This value influences whether local or global effects influence the occlusion. In (0, 100];
 */
uniform float u_ssaoRadius = 10.0;

/**
 * Defines a falloff at which the depthDifference is assumed to be 0 (occluder is on same height)
 */
uniform float u_ssaoFalloff = 0.0;

/**
 * Calculate SSAO effect for the specified pixel. Requires a noise texture in u_noiseSampler. This implementation is taken from
 * http://www.gamerendering.com/2009/01/14/ssao/ and slightly modified. We added a small hack to provide scaling-invariance to the effect.
 *
 * \param where coordinate in screenspace
 * \param radius additional radius scaler.
 *
 * \return AO
 */
float getSSAO( vec2 where, float radius )
{
    float totStrength = u_ssaoTotalStrength;
    float strength = u_ssaoStrength;
    float falloff = u_ssaoFalloff;
    float rad = radius * u_ssaoRadius;

    #define SAMPLES WGE_POSTPROCESSOR_SSAO_SAMPLES
    const float invSamples = 1.0/ float( SAMPLES );

    // grab a normal for reflecting the sample rays later on
    vec3 fres = normalize( ( texture2D( u_noiseSampler, where * u_noiseSizeX ).xyz * 2.0 ) - vec3( 1.0 ) );
    vec4 currentPixelSample = getNormal( where );
    float currentPixelDepth = getDepth( where );
    float radiusSS = ( getZoom() * u_ssaoRadius / float( u_texture0SizeX ) ) / ( 1.0 - currentPixelDepth );

    // current fragment coords in screen space
    vec3 ep = vec3( where.xy, currentPixelDepth );
    // get the normal of current fragment
    vec3 norm = currentPixelSample.xyz;

    float bl = 0.0;
    // adjust for the depth ( not shure if this is good..)
    float radD = rad/currentPixelDepth;

    vec3 ray, se;
    float occluderDepth, depthDifference, normDiff;

    for( int i = 0; i < SAMPLES; ++i )
    {
        // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it

        // grab a rand normal from the noise texture
        vec3 randSphereNormal = getNoiseAsVector( vec2( float( i ) / float( SAMPLES ), fres.y ) );
        ray =  radiusSS * reflect( randSphereNormal, fres );

        // if the ray is outside the hemisphere then change direction
        se = ep + sign( dot( ray, norm ) ) * ray;

        // get the depth of the occluder fragment
        vec4 occluderFragment = getNormal( se.xy );
        occluderFragment.a = getDepth( se.xy );

        // if depthDifference is negative = occluder is behind current fragment
        depthDifference = currentPixelDepth - occluderFragment.a;

        // calculate the difference between the normals as a weight
        normDiff = ( 1.0 - dot( occluderFragment.xyz, norm ) );
        // the falloff equation, starts at falloff and is kind of 1/x^2 falling
        bl += step( falloff, depthDifference ) * normDiff * ( 1.0 - smoothstep( falloff, strength, depthDifference ) );
    }

    // output the result
    return 1.0 - totStrength * bl * invSamples;
}

/**
 * Calculate the screen-space ambient occlusion from normal and depth map for the current pixel.
 *
 * \return the SSAO factor
 */
float getSSAO()
{
    return getSSAO( pixelCoord, 1.0 );
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
    float depth = getDepth();
    gl_FragDepth = depth;
    gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    if( depth > 0.99 )
    {
        discard;
    }

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
        gl_FragColor = vec4( getColor().rgb EDGEOP, 1.0 );
    #else
        // also do this for the effect-only output
        #ifdef WGE_POSTPROCESSOR_EDGE_BLACKEDGE
            #define EDGEOP 1.0 -
        #else
            // for white borders, add
            #define EDGEOP 0.0 +
        #endif
        gl_FragColor = vec4( vec3( EDGEOP getEdge() ), 1.0 );
    #endif
#endif

#ifdef WGE_POSTPROCESSOR_CEL
    // output the depth and final color.
    gl_FragColor = getCelShading();
#endif

#ifdef WGE_POSTPROCESSOR_SSAO
    // output color AND SSAO?
    #ifdef WGE_POSTPROCESSOR_OUTPUT_COMBINE
        gl_FragColor = vec4( getColor().rgb * getSSAO(), 1.0 );
    #else
        gl_FragColor = vec4( vec3( getSSAO() ), 1.0 );
    #endif
#endif
}

#endif // WGEPOSTPROCESSOR_FRAGMENT_GLSL

