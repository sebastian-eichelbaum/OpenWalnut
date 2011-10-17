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
 * The total influence of SSAO.
 */
//uniform float u_ssaoTotalStrength = 2.5;     // lines
// uniform float u_ssaoTotalStrength = 2.5;     // brain lines
//uniform float u_ssaoTotalStrength = 2.25;     // brain lines video
//uniform float u_ssaoTotalStrength = 1.5;   // smallfibs
uniform float u_ssaoTotalStrength =2.0;   // smallfibs-corticospinal-tubes
//uniform float u_ssaoTotalStrength =2.5;   // smallfibs-corticospinal-lines
//uniform float u_ssaoTotalStrength = 2.0;   // deltawing tube
//uniform float u_ssaoTotalStrength = 3.0;   // deltawing lines

/**
 * The radius of the hemispshere in screen-space which gets scaled.
 */
//uniform float u_ssaoRadiusSS = 2.0;   // lines
//uniform float u_ssaoRadiusSS = 2.5;   // brain lines
//uniform float u_ssaoRadiusSS = 2.0;   // brain lines-video
//uniform float u_ssaoRadiusSS = 2.5; // smallfibs
uniform float u_ssaoRadiusSS = 2.5;   // brain lines -corticospinal
//uniform float u_ssaoRadiusSS = 1.0; // deltawing tube
//uniform float u_ssaoRadiusSS = 2.5; // deltawing lines

/**
 * The strength of the occluder influence in relation to the geometry density. The heigher the value, the larger the influence. Low values remove
 * the drop-shadow effect.
 */
uniform float u_ssaoDensityWeight = 2.0;    // video
//uniform float u_ssaoDensityWeight = 1.0;

/**
 * Calculate the screen-space ambient occlusion from normal and depth map.
 *
 * \param where the pixel to get SSAO for
 *
 * \return the SSAO factor
 */
float getSSAO( vec2 where )
{
    #define SCALERS 3   // all
    #define SAMPLES 32  // the numbers of samples to check on the hemisphere // all
    //#define SCALERS 5   // deltawing lines
    //#define SAMPLES 64  // the numbers of samples to check on the hemisphere // deltawing lines
    const float invSamples = 1.0 / float( SAMPLES );

    // Fall-off for SSAO per occluder. This hould be zero (or nearly zero) since it defines what is counted as before, or behind.
    const float falloff = 0.00001;

    // grab a random normal for reflecting the sample rays later on
    vec3 randNormal = normalize( ( texture2D( u_noiseSampler, where * u_texture3SizeX ).xyz * 2.0 ) - vec3( 1.0 ) );

    // grab the current pixel's normal and depth
    vec3 currentPixelSample = getNormal( where ).xyz;
    float currentPixelDepth = getDepth( where );

    // current fragment coords in screen space
    vec3 ep = vec3( where.xy, currentPixelDepth );

    // get the normal of current fragment
    vec3 normal = currentPixelSample.xyz;

    // the radius of the sphere is, in screen-space, half a pixel. So the hemisphere covers nearly one pixel. Scaling by depth somehow makes it
    // more invariant for zooming
    float radius = ( getZoom() * u_ssaoRadiusSS / float( u_texture0SizeX ) ) / ( 1.0 - currentPixelDepth );

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
            // TODO(ebaum): well, u_ssaoRadiusSS = 2 but radiusScaler is 0.5 the first time....
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
            float densityInfluence = scaler * scaler * u_ssaoDensityWeight;

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
    }

    // output the result
    return clamp( ( 1.0 - ( u_ssaoTotalStrength * occlusion ) ), 0, 1 );
}

float getSSAONaive( vec2 where )
{

    // this is taken from http://www.gamerendering.com/2009/01/14/ssao/

 float totStrength = 10.38;
 float strength = 1.07;
 float offset = 18.0;
 float falloff = 0.000002;
 float rad = 0.0035;
#define SAMPLES 16 // 10 is good
const float invSamples = 1.0/16.0;

/*uniform sampler2D rnm;
uniform sampler2D normalMap;
varying vec2 uv;*/


vec2 uv=where;

// these are the random vectors inside a unit sphere
    vec3 pSphere[16] = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));
//const vec3 pSphere[8] = vec3[](vec3(0.24710192, 0.6445882, 0.033550154),vec3(0.00991752, -0.21947019, 0.7196721),vec3(0.25109035, -0.1787317, -0.011580509),vec3(-0.08781511, 0.44514698, 0.56647956),vec3(-0.011737816, -0.0643377, 0.16030222),vec3(0.035941467, 0.04990871, -0.46533614),vec3(-0.058801126, 0.7347013, -0.25399926),vec3(-0.24799341, -0.022052078, -0.13399573));
//const vec3 pSphere[12] = vec3[](vec3(-0.13657719, 0.30651027, 0.16118456),vec3(-0.14714938, 0.33245975, -0.113095455),vec3(0.030659059, 0.27887347, -0.7332209),vec3(0.009913514, -0.89884496, 0.07381549),vec3(0.040318526, 0.40091, 0.6847858),vec3(0.22311053, -0.3039437, -0.19340435),vec3(0.36235332, 0.21894878, -0.05407306),vec3(-0.15198798, -0.38409665, -0.46785462),vec3(-0.013492276, -0.5345803, 0.11307949),vec3(-0.4972847, 0.037064247, -0.4381323),vec3(-0.024175806, -0.008928787, 0.17719103),vec3(0.694014, -0.122672155, 0.33098832));
//const vec3 pSphere[10] = vec3[](vec3(-0.010735935, 0.01647018, 0.0062425877),vec3(-0.06533369, 0.3647007, -0.13746321),vec3(-0.6539235, -0.016726388, -0.53000957),vec3(0.40958285, 0.0052428036, -0.5591124),vec3(-0.1465366, 0.09899267, 0.15571679),vec3(-0.44122112, -0.5458797, 0.04912532),vec3(0.03755566, -0.10961345, -0.33040273),vec3(0.019100213, 0.29652783, 0.066237666),vec3(0.8765323, 0.011236004, 0.28265962),vec3(0.29264435, -0.40794238, 0.15964167));

// grab a normal for reflecting the sample rays later on
   vec3 fres = normalize((texture2D(u_noiseSampler,uv*offset).xyz*2.0) - vec3(1.0));

   vec4 currentPixelSample = getNormal( uv );

   float currentPixelDepth = getDepth( uv );

   // current fragment coords in screen space
   vec3 ep = vec3(uv.xy,currentPixelDepth);
 // get the normal of current fragment
   vec3 norm = currentPixelSample.xyz;

   float bl = 0.0;
   // adjust for the depth ( not shure if this is good..)
   float radD = rad/currentPixelDepth;

   vec3 ray, se, occNorm;
   float occluderDepth, depthDifference, normDiff;

   for(int i=0; i<SAMPLES;++i)
   {
      // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
      ray = radD*reflect(pSphere[i],fres);

      // if the ray is outside the hemisphere then change direction
      se = ep + sign(dot(ray,norm) )*ray;

      // get the depth of the occluder fragment
      vec4 occluderFragment = getNormal( se.xy);
        occluderFragment.a = getDepth( se.xy );
      // get the normal of the occluder fragment
      occNorm = occluderFragment.xyz;

      // if depthDifference is negative = occluder is behind current fragment
      depthDifference = currentPixelDepth-occluderFragment.a;

      // calculate the difference between the normals as a weight

      normDiff = (1.0-dot(occNorm,norm));
      // the falloff equation, starts at falloff and is kind of 1/x^2 falling
      bl += step(falloff,depthDifference)*normDiff*(1.0-smoothstep(falloff,strength,depthDifference));
   }

   // output the result
   float ao = 1.0-totStrength*bl*invSamples;

    return ao;// 1.0;
}

/**
 * Calculate the screen-space ambient occlusion from normal and depth map for the current pixel.
 *
 * \return the SSAO factor
 */
float getSSAO()
{
    return getSSAONaive( pixelCoord );
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

vec4 getGGColor( vec2 where )
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    // get the 8-neighbourhood
    vec4 gaussedColorc  = getGaussedColor( where );
    vec4 gaussedColorbl = getGaussedColor( where + vec2( -offsetX, -offsetY ) );
    vec4 gaussedColorl  = getGaussedColor( where + vec2( -offsetX,     0.0  ) );
    vec4 gaussedColortl = getGaussedColor( where + vec2( -offsetX,  offsetY ) );
    vec4 gaussedColort  = getGaussedColor( where + vec2(     0.0,   offsetY ) );
    vec4 gaussedColortr = getGaussedColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorr  = getGaussedColor( where + vec2(  offsetX,     0.0  ) );
    vec4 gaussedColorbr = getGaussedColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorb  = getGaussedColor( where + vec2(     0.0,  -offsetY ) );

    // apply gauss filter
    vec4 gaussed = ( 1.0 / 16.0 ) * (
            1.0 * gaussedColortl +  2.0 * gaussedColort + 1.0 * gaussedColortr +
            2.0 * gaussedColorl  +  4.0 * gaussedColorc + 2.0 * gaussedColorr  +
            1.0 * gaussedColorbl +  2.0 * gaussedColorb + 1.0 * gaussedColorbr );
    return gaussed;
}

vec4 getGGGColor( vec2 where )
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    // get the 8-neighbourhood
    vec4 gaussedColorc  = getGGColor( where );
    vec4 gaussedColorbl = getGGColor( where + vec2( -offsetX, -offsetY ) );
    vec4 gaussedColorl  = getGGColor( where + vec2( -offsetX,     0.0  ) );
    vec4 gaussedColortl = getGGColor( where + vec2( -offsetX,  offsetY ) );
    vec4 gaussedColort  = getGGColor( where + vec2(     0.0,   offsetY ) );
    vec4 gaussedColortr = getGGColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorr  = getGGColor( where + vec2(  offsetX,     0.0  ) );
    vec4 gaussedColorbr = getGGColor( where + vec2(  offsetX,  offsetY ) );
    vec4 gaussedColorb  = getGGColor( where + vec2(     0.0,  -offsetY ) );

    // apply gauss filter
    vec4 gaussed = ( 1.0 / 16.0 ) * (
            1.0 * gaussedColortl +  2.0 * gaussedColort + 1.0 * gaussedColortr +
            2.0 * gaussedColorl  +  4.0 * gaussedColorc + 2.0 * gaussedColorr  +
            1.0 * gaussedColorbl +  2.0 * gaussedColorb + 1.0 * gaussedColorbr );
    return gaussed;
}


vec4 getDOF( vec2 where )
{
    float targetDepth = getDepth( ( gl_TextureMatrix[0] * vec4( 0.5, 0.5, 0.0, 0.0 ) ).xy );

    float focalDistance = targetDepth;
    float focalRange = 0.1;

    float blur = clamp( abs( getDepth( where ) - focalDistance ) / focalRange, 0.0, 1.0 );
    vec4 gCol = getGGGColor( where );
    vec4 col = getColor( where );

    float fadeOut = getDepthFading( where );
    return vec4( fadeOut * mix( col.rgb, gCol.rgb, blur ), 1.0 );
}

vec4 getDOF()
{
    return getDOF( pixelCoord );
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

#ifdef WGE_POSTPROCESSOR_DOF
    blend( getDOF() );
#endif

#ifdef WGE_POSTPROCESSOR_PPLPHONG
    wge_LightIntensityParameter l = wge_DefaultLightIntensityFullDiffuse;
    // Paper Screenshots:
    // l.materialAmbient = 0.125;
    // l.materialShinines = 2000;

    // Video
    l.materialAmbient = 0.25;
    l.materialDiffuse = 0.75;
    l.materialShinines = 1000;

    // Smallfibs - Cortico
    l.materialAmbient = 0.25;
    l.materialDiffuse = 1.0;
    l.materialShinines = 500;

    blendScale( getPPLPhong( l ) );
#endif

#ifdef WGE_POSTPROCESSOR_SSAO
    //blendScale( 2.0 * ( getSSAO() - 0.2 ) );    // smallfibs
    blendScale( 1.5 * getSSAO() );    // brain
#endif

#ifdef WGE_POSTPROCESSOR_SSAOWITHPHONG
    // Teaser:
    float ao = getSSAO();
    float ao2 = 2. * ( ao - 0.5 );

    wge_LightIntensityParameter light = wge_DefaultLightIntensity;
    light.materialShinines = 1000.0;

    float lPhong = getPPLPhong( light );
    float lKrueger = kruegerNonLinearIllumination( getNormal().xyz, 4.0 );

    float invD = ( 1.0 - getDepth() );
    float df = 0.5 + 0.5*smoothstep( 0.2, 0.3, invD );

    float l = ( ao2 + ao*lPhong );
    l *= 0.75;
    blend( vec4( vec3( getColor().rgb * l * 1.0 ), 1.0 ) );

    /*blend( getColor() );
    blendScale( getPPLPhong( wge_DefaultLightIntensityFullDiffuse ) );
    blendScale( 2.0 * ( getSSAO() - 0.15 ) );*/
    //blendScale( getSSAO() );

    //blend( vec4( vec3( 0.5 * ao ), 1.0 ) );
    //blendScale( getDepthFading() );
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

