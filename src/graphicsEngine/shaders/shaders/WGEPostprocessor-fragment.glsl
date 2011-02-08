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

/**
 * The texture Unit for the Normal Map
 */
uniform sampler2D u_texture1Sampler;

/**
 * The depth texture
 */
uniform sampler2D u_texture2Sampler;

/**
 * The white-noise 3 channel texture: sampler
 */
uniform sampler2D u_texture3Sampler;

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
 * Grabs the normal at the specified point. The returned normal has been de-scaled to [-1,1] and normalized The w component is 1.
 *
 * \param where the pixel to grab
 *
 * \return the normal
 */
vec4 getNormal( in vec2 where )
{
    return normalize( texture2DUnscaled( u_texture1Sampler, where, -1.0, 2.0 ).xyz ).xyzz * zeroOneList.xxxy + zeroOneList.yyyx;
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

    // apply gauss filter
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
const float u_ssaoTotalStrength = 2.5; // 1.38;   // total strength - scaling the resulting AO

/**
 * The strength of the occluder influence in relation to the geometry density. The heigher the value, the larger the influence. Low values remove
 * the drop-shadow effect.
 */
const float u_ssaoDensityWeight = 1.0; //0.07;

/**
 * The radius of the hemispshere in screen-space which gets scaled.
 */
const float u_ssaoRadiusSS = 15.0;

/**
 * Calculate the screen-space ambient occlusion from normal and depth map.
 *
 * \param where the pixel to get SSAO for
 *
 * \return the SSAO factor
 */
float getSSAO( vec2 where )
{
    // NOTE: The code was inspired by http://www.gamerendering.com/2009/01/14/ssao/ but quite intensivly modified until now

    #define SAMPLES 32  // the numbers of samples to check on the hemisphere
    const float invSamples = 1.0 / float( SAMPLES );

    // Fall-off for SSAO per occluder. This hould be zero (or nearly zero) since it defines what is counted as before, or behind.
    const float falloff = 0.00001;

    // these are the random vectors inside a unit sphere
    vec3 randSphereNormals[32] = vec3[]( vec3(  0.53812504,   0.18565957,   -0.43192 ),      vec3(  0.13790712,   0.24864247,    0.44301823 ),
                                         vec3(  0.33715037,   0.56794053,   -0.005789503 ),  vec3( -0.6999805,   -0.04511441,   -0.0019965635 ),
                                         vec3(  0.06896307,  -0.15983082,   -0.85477847 ),   vec3(  0.056099437,  0.006954967,  -0.1843352 ),
                                         vec3( -0.014653638,  0.14027752,    0.0762037 ),    vec3(  0.010019933, -0.1924225,    -0.034443386 ),
                                         vec3( -0.35775623,  -0.5301969,    -0.43581226 ),   vec3( -0.3169221,    0.106360726,   0.015860917 ),
                                         vec3(  0.010350345, -0.58698344,    0.0046293875 ), vec3( -0.08972908,  -0.49408212,    0.3287904 ),
                                         vec3(  0.7119986,   -0.0154690035, -0.09183723 ),   vec3( -0.053382345,  0.059675813,  -0.5411899 ),
                                         vec3(  0.035267662, -0.063188605,   0.54602677 ),   vec3( -0.47761092,   0.2847911,    -0.0271716 ),
                                         vec3(  0.24710192,   0.6445882,     0.033550154 ),  vec3(  0.00991752,  -0.21947019,    0.7196721 ),
                                         vec3(  0.25109035,  -0.1787317,    -0.011580509 ),  vec3( -0.08781511,   0.44514698,    0.56647956 ),
                                         vec3( -0.011737816, -0.0643377,     0.16030222 ),   vec3(  0.035941467,  0.04990871,   -0.46533614 ),
                                         vec3( -0.058801126,  0.7347013,    -0.25399926 ),   vec3( -0.24799341,  -0.022052078,  -0.13399573 ),
                                         vec3( -0.13657719,   0.30651027,    0.16118456 ),   vec3( -0.14714938,   0.33245975,   -0.113095455 ),
                                         vec3(  0.030659059,  0.27887347,   -0.7332209 ),    vec3(  0.009913514, -0.89884496,    0.07381549 ),
                                         vec3(  0.040318526,  0.40091,       0.6847858 ),    vec3(  0.22311053,  -0.3039437,    -0.19340435 ),
                                         vec3(  0.36235332,   0.21894878,   -0.05407306 ),   vec3( -0.15198798,  -0.38409665,   -0.46785462 ) /*,
                                         vec3( -0.013492276, -0.5345803,     0.11307949 ),   vec3( -0.4972847,    0.037064247,  -0.4381323 ),
                                         vec3( -0.024175806, -0.008928787,   0.17719103 ),   vec3(  0.694014,    -0.122672155,   0.33098832 ),
                                         vec3( -0.010735935,  0.01647018,    0.0062425877 ), vec3( -0.06533369,   0.3647007,    -0.13746321 ),
                                         vec3( -0.6539235,   -0.016726388,  -0.53000957 ),   vec3(  0.40958285,   0.0052428036, -0.5591124 ),
                                         vec3( -0.1465366,    0.09899267,    0.15571679 ),   vec3( -0.44122112,  -0.5458797,     0.04912532 ),
                                         vec3(  0.03755566,  -0.10961345,   -0.33040273 ),   vec3(  0.019100213,  0.29652783,    0.066237666 ),
                                         vec3(  0.8765323,    0.011236004,   0.28265962 ),   vec3(  0.29264435,  -0.40794238,    0.15964167 ) */
                                         );


    // grab a random normal for reflecting the sample rays later on
    vec3 randNormal = normalize( ( texture2D( u_texture3Sampler, where * u_texture3SizeX ).xyz * 2.0 ) - vec3( 1.0 ) );

    // grab the current pixel's normal and depth
    vec3 currentPixelSample = getNormal( where ).xyz;
    float currentPixelDepth = getDepth( where );

    // current fragment coords in screen space
    vec3 ep = vec3( where.xy, currentPixelDepth );

    // get the normal of current fragment
    vec3 normal = currentPixelSample.xyz;

    // the radius of the sphere is, in screen-space, half a pixel. So the hemisphere covers nearly one pixel. Scaling by depth somehow makes it
    // more invariant for zooming
    float radius = ( u_ssaoRadiusSS / float( u_texture0SizeX ) ) / currentPixelDepth;

    // some temporaries needed inside the loop
    vec3 ray;                     // the current ray inside the sphere
    vec3 hemispherePoint;         // the point on the hemisphere
    vec3 occluderNormal;          // the normal at the point on the hemisphere

    float occluderDepth;          // the depth of the potential occluder on the hemisphere
    float depthDifference;        // the depth difference between the current point and the occluder (point on hemisphere)
    float normalDifference;       // the projection of the occluderNormal to the surface normal

    // accumulated occlusion
    float occlusion = 0.0;

    // Get SAMPLES-times samples on the hemisphere and check for occluders
    for( int i = 0; i < SAMPLES; ++i )
    {
        // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
        ray = radius * reflect( randSphereNormals[i], randNormal );

        // if the ray is outside the hemisphere then change direction
        hemispherePoint = ( sign( dot( ray, normal ) ) * ray ) + ep;

        // get the depth of the occluder fragment
        occluderDepth = getDepth( hemispherePoint.xy );

        // get the normal of the occluder fragment
        occluderNormal = getNormal( hemispherePoint.xy ).xyz;

        // if depthDifference is negative = occluder is behind the current fragment -> occluding this fragment
        depthDifference = currentPixelDepth - occluderDepth;

        // calculate the difference between the normals as a weight. This weight determines how much the occluder occludes the fragment
        normalDifference = 1.0 - dot( occluderNormal, normal );

        // the higher the depth difference, the less it should influence the occlusion value since large space between geometry normally allows
        // many light. It somehow can be described with "shadowiness". In other words, it describes the density of geometry and its influence to
        // the occlusion.
        float densityWeight = ( 1.0 - smoothstep( falloff, u_ssaoDensityWeight, depthDifference ) );

        // the following step function ensures that negative depthDifference values get clamped to 0, meaning that this occluder should have NO
        // influence on the occlusion value. A positive value (fragment is behind the occluder) increases the occlusion factor according to the
        // normal weight and density weight
        occlusion += normalDifference * densityWeight * step( falloff, depthDifference );
    }

    // output the result
    return ( 1.0 - ( u_ssaoTotalStrength * occlusion * invSamples ) );
}


/**
 * Calculate the screen-space ambient occlusion from normal and depth map for the current pixel.
 *
 * \return the SSAO factor
 */
float getSSAO()
{
    return getSSAO( pixelCoord );
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
    return smoothstep( 0.20, 0.5, invD );
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
    if ( depth > 0.99 )
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

#ifdef WGE_POSTPROCESSOR_SSAO
    blendScale( getSSAO() );
#endif

#ifdef WGE_POSTPROCESSOR_SSAOWITHPHONG
    float ao = getSSAO();
    ao = 2.0 * ( ao - 0.5 );
    float l = clamp( 0.6 * getPPLPhong( wge_DefaultLightIntensityLessDiffuse ) + 0.4 * ao, 0.0, 1.0 );
    blend( vec4( getColor().rgb * l, getColor().a ) );
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
    blend( getNormal() );
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

