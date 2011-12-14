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

#ifdef WGE_POSTPROCESSOR_LINEAO

#ifndef WGE_POSTPROCESSOR_LINEAO_SAMPLES
    #define WGE_POSTPROCESSOR_LINEAO_SAMPLES 32
#endif
#ifndef WGE_POSTPROCESSOR_LINEAO_SCALERS
    #define WGE_POSTPROCESSOR_LINEAO_SCALERS 3
#endif

/**
 * The total influence of SSAO.
 */
//uniform float u_lineaoTotalStrength = 2.5;     // lines
uniform float u_lineaoTotalStrength = 2.5;     // brain lines
//uniform float u_lineaoTotalStrength = 2.25;     // brain lines video
//uniform float u_lineaoTotalStrength = 1.5;   // smallfibs
//uniform float u_lineaoTotalStrength =2.0;   // smallfibs-corticospinal-tubes
//uniform float u_lineaoTotalStrength =2.5;   // smallfibs-corticospinal-lines
//uniform float u_lineaoTotalStrength = 2.0;   // deltawing tube
//uniform float u_lineaoTotalStrength = 3.0;   // deltawing lines

/**
 * The radius of the hemispshere in screen-space which gets scaled.
 */
//uniform float u_lineaoRadiusSS = 2.0;   // lines
uniform float u_lineaoRadiusSS = 2.5;   // brain lines
//uniform float u_lineaoRadiusSS = 2.0;   // brain lines-video
//uniform float u_lineaoRadiusSS = 2.5; // smallfibs
//uniform float u_lineaoRadiusSS = 2.5;   // brain lines -corticospinal
//uniform float u_lineaoRadiusSS = 1.0; // deltawing tube
//uniform float u_lineaoRadiusSS = 2.5; // deltawing lines

/**
 * The strength of the occluder influence in relation to the geometry density. The higher the value, the larger the influence. Low values remove
 * the drop-shadow effect.
 */
//uniform float u_lineaoDensityWeight = 2.0;    // video
uniform float u_lineaoDensityWeight = 1.0;

/**
 * Calculate the screen-space ambient occlusion LineAO for the given pixel.
 *
 * \param where the pixel to get SSAO for
 *
 * \return the LineAO factor
 */
float getLineAO( vec2 where )
{
    #define SCALERS WGE_POSTPROCESSOR_LINEAO_SCALERS  // how much hemispheres to sample?
    #define SAMPLES WGE_POSTPROCESSOR_LINEAO_SAMPLES  // the numbers of samples to check on the hemisphere
    const float invSamples = 1.0 / float( SAMPLES );

    // Fall-off for SSAO per occluder. This hould be zero (or nearly zero) since it defines what is counted as before, or behind.
    const float falloff = 0.00001;

    // grab a random normal for reflecting the sample rays later on
    vec3 randNormal = normalize( ( texture2D( u_noiseSampler, where * u_noiseSizeX ).xyz * 2.0 ) - vec3( 1.0 ) );

    // grab the current pixel's normal and depth
    vec3 currentPixelSample = getNormal( where ).xyz;
    float currentPixelDepth = getDepth( where );

    // current fragment coords in screen space
    vec3 ep = vec3( where.xy, currentPixelDepth );

    // get the normal of current fragment
    vec3 normal = currentPixelSample.xyz;

    // the radius of the sphere is, in screen-space, half a pixel. So the hemisphere covers nearly one pixel. Scaling by depth somehow makes it
    // more invariant for zooming
    float radius = ( getZoom() * u_lineaoRadiusSS / float( u_texture0SizeX ) ) / ( 1.0 - currentPixelDepth );

    // some temporaries needed inside the loop
    vec3 ray;                     // the current ray inside the sphere
    vec3 hemispherePoint;         // the point on the hemisphere
    vec3 occluderNormal;          // the normal at the point on the hemisphere

    float occluderDepth;          // the depth of the potential occluder on the hemisphere
    float depthDifference;        // the depth difference between the current point and the occluder (point on hemisphere)
    float normalDifference;       // the projection of the occluderNormal to the surface normal

    // accumulated occlusion
    float occlusion = 0.0;
    float radiusScaler = 0.0;     // we sample with multiple radii, so use a scaling factor here

    // this allows an adaptive radius
    float[SCALERS] rads;
    #define radScaleMin 0.5
    #define radScaleMax 3.0
    rads[0] = radScaleMin + 0.0 * ( radScaleMax / SCALERS );
#if ( SCALERS > 1 )
    rads[1] = radScaleMin + 1.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 2 )
    rads[2] = radScaleMin + 2.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 3 )
    rads[3] = radScaleMin + 3.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 4 )
    rads[4] = radScaleMin + 4.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 5 )
    rads[5] = radScaleMin + 5.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 6 )
    rads[5] = radScaleMin + 6.0 * ( radScaleMax / SCALERS );
#endif
#if ( SCALERS > 7 )
    rads[5] = radScaleMin + 7.0 * ( radScaleMax / SCALERS );
#endif
    float fac = 0.0;
    float maxi = 0.0;

    // sample for different radii
    for( int l = 0; l < SCALERS; ++l )
    {
        float occlusionStep = 0.0;  // this variable accumulates the occlusion for the current radius
        radiusScaler += rads[ l ];    // increment radius each time.

        // Get SAMPLES-times samples on the hemisphere and check for occluders
        for( int i = 0; i < SAMPLES; ++i )
        {
            // grab a rand normal from the noise texture
            vec3 randSphereNormal = ( texture2D( u_noiseSampler, vec2( float( i ) / float( SAMPLES ),
                                                                       float( l + 1 ) / float( SCALERS ) ) ).rgb * 2.0 ) - vec3( 1.0 );

            // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
            vec3 hemisphereVector = reflect( randSphereNormal, randNormal );
            ray = radiusScaler * radius * hemisphereVector;
            ray = sign( dot( ray, normal ) ) * ray;

            // if the ray is outside the hemisphere then change direction
            hemispherePoint = ray + ep;

            // HACK! Somehow handle borders
            //if ( ( hemispherePoint.x < 0.0 ) || ( hemispherePoint.x > 0.65 ) )
            //{
            //    hemispherePoint.x *= -1.0;
            //}
            //if ( ( hemispherePoint.y < 0.0 ) || ( hemispherePoint.y > 0.575 ) )
            //{
            //    hemispherePoint.y *= -1.0;
            //}

            // get the depth of the occluder fragment
            occluderDepth = getDepth( hemispherePoint.xy );

            // get the normal of the occluder fragment
            occluderNormal = getNormal( hemispherePoint.xy ).xyz;

            // if depthDifference is negative = occluder is behind the current fragment -> occluding this fragment
            depthDifference = currentPixelDepth - occluderDepth;

            // calculate the difference between the normals as a weight. This weight determines how much the occluder occludes the fragment
            // TODO(ebaum): evaluate these alternatives: the first one is the surface direction and occluder surface direction
            float pointDiffuse = max( dot( hemisphereVector, normal ), 0.0 ); // this equals the diffuse term in Phong if lightDir is the
                                                                              // occluder's surface

            // TODO(ebaum): check normal again with another light
            vec3 t = getTangent( hemispherePoint.xy ).xyz;
            vec3 newnorm = normalize( cross( normalize( cross( t, normalize( hemisphereVector ) ) ), t ) );
            float occluderDiffuse = max( dot( newnorm, gl_LightSource[0].position.xyz ), 0.0 );

            // TODO(ebaum): better incorporate specular reflection
            vec3 H = normalize( gl_LightSource[0].position.xyz + normalize( hemisphereVector ) );
            float occluderSpecular = pow( max( dot( H, occluderNormal ), 0.0 ), 100.0 );

            // this second is as proposed for AO, the omega (hemisphere vector) and surface normal
            normalDifference = pointDiffuse * ( occluderSpecular + occluderDiffuse );
            normalDifference = 1.0 - normalDifference;

            // the higher the depth difference, the less it should influence the occlusion value since large space between geometry normally allows
            // many light. It somehow can be described with "shadowiness". In other words, it describes the density of geometry and its influence to
            // the occlusion.
            float scaler = 1.0 - ( l / ( float( SCALERS - 1 ) ) );
            float densityInfluence = scaler * scaler * u_lineaoDensityWeight;

            //  This reduces the occlusion if the occluder is far away
            float densityWeight = 1.0 - smoothstep( falloff, densityInfluence, depthDifference );
            // This is the same as:
            // float e0 = falloff;
            // float e1 = densityInfluence;
            // float r = ( depthDifference - e0 ) / ( e1 - e0 );
            // float densityWeight = 1.0 - smoothstep( 0.0, 1.0, r );

            // the following step function ensures that negative depthDifference values get clamped to 0, meaning that this occluder should have NO
            // influence on the occlusion value. A positive value (fragment is behind the occluder) increases the occlusion factor according to the
            // normal weight and density weight
            occlusionStep += invSamples * normalDifference * densityWeight * step( falloff, depthDifference );
        }

        // for this radius, add to total occlusion
        occlusion += occlusionStep / float( SCALERS );
        maxi = max( occlusionStep, maxi );
    }

    // output the result
    return clamp( ( 1.0 - ( u_lineaoTotalStrength * occlusion ) ), 0, 1 );
}

/**
 * Calculate the screen-space ambient occlusion  LineAO for the current pixel.
 *
 * \return the LineAO factor
 */
float getLineAO()
{
    return getLineAO( pixelCoord );
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

#ifdef WGE_POSTPROCESSOR_LINEAO
    // output color AND SSAO?
    #ifdef WGE_POSTPROCESSOR_OUTPUT_COMBINE
        gl_FragColor = vec4( getColor().rgb * getLineAO(), 1.0 );
    #else
        gl_FragColor = vec4( vec3( getLineAO() ), 1.0 );
    #endif
#endif
}

#endif // WGEPOSTPROCESSOR_FRAGMENT_GLSL

