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
 * The white-noise 3 channel texture: sampler
 */
uniform sampler1D u_texture4Sampler;

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
//const float u_ssaoTotalStrength = 2.0; // 1.38;   // total strength - scaling the resulting AO
const float u_ssaoTotalStrength = 5.5; // 1.38;   // total strength - scaling the resulting AO

/**
 * The strength of the occluder influence in relation to the geometry density. The heigher the value, the larger the influence. Low values remove
 * the drop-shadow effect.
 */
const float u_ssaoDensityWeight = 1.0; //0.07;

/**
 * The radius of the hemispshere in screen-space which gets scaled.
 */
const float u_ssaoRadiusSS = 5.0;

/**
 * Calculate the screen-space ambient occlusion from normal and depth map.
 *
 * \param where the pixel to get SSAO for
 *
 * \return the SSAO factor
 */
float getSSAO( vec2 where )
{
    #define SCALERS 2
    #define SAMPLES 32  // the numbers of samples to check on the hemisphere
    const float invSamples = 1.0 / float( SAMPLES );

    // Fall-off for SSAO per occluder. This hould be zero (or nearly zero) since it defines what is counted as before, or behind.
    const float falloff = 0.00001;

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
    float radiusScaler = 0.0;     // we sample with multiple radii, so use a scaling factor here

    // sample for different radii
   for( int l = 1; l <= SCALERS; ++l )
    {
        float occlusionStep = 0.0;  // this variable accumulates the occlusion for the current radius
        radiusScaler += 1.0;    // increment radius each time.

        // Get SAMPLES-times samples on the hemisphere and check for occluders
        for( int i = 0; i < SAMPLES; ++i )
        {
            // grab a rand normal from the noise texture
            vec3 randSphereNormal = ( texture2D( u_texture3Sampler, vec2( float( i ) / float( SAMPLES ),
                                                                          float( l ) / float( SCALERS ) ) ).rgb * 2.0 ) - vec3( 1.0 );

            // get a vector (randomized inside of a sphere with radius 1.0) from a texture and reflect it
            ray = radiusScaler * radius * reflect( randSphereNormal, randNormal );

            // if the ray is outside the hemisphere then change direction
            hemispherePoint = ( sign( dot( ray, normal ) ) * ray ) + ep;

            // get the depth of the occluder fragment
            occluderDepth = getDepth( hemispherePoint.xy );

            // get the normal of the occluder fragment
            occluderNormal = 0.75 * getNormal( hemispherePoint.xy ).xyz;

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
            occlusionStep += invSamples * normalDifference * densityWeight * step( falloff, depthDifference );
        }

        // for this radius, add to total occlusion
        occlusion += occlusionStep / SCALERS;
    }

    // output the result
    return ( 1.0 - ( u_ssaoTotalStrength * occlusion ) );
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
    float lPhong = getPPLPhong( wge_DefaultLightIntensityLessDiffuse );
    float lKrueger = kruegerNonLinearIllumination( getNormal().xyz, 5.0 );
    float l =  ao + lPhong;//Krueger;
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

