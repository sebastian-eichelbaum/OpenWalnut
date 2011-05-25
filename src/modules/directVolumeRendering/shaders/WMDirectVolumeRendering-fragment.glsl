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

#ifdef LOCALILLUMINATION_PHONG
#include "WGEShadingTools.glsl"
#endif

// This is needed if the modulo operator % is required
#extension GL_EXT_gpu_shader4 : enable

#include "WGEUtils.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMDirectVolumeRendering-varyings.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D u_volumeSampler;
uniform sampler1D u_transferFunctionSampler;
uniform sampler3D u_gradientsSampler;
uniform sampler2D u_jitterSampler;
uniform int       u_jitterSizeX;
uniform int       u_samples;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Method uses the ray direction (varying) and the ray entry point in the cube (varying) to calculate the exit point. This is lateron needed to
 * get a proper maximum distance along the ray.
 *
 * \param d out - this value will contain the maximum distance along the ray untill the end of the cube
 * \param rayStart in - the start point of the ray in the volume
 *
 * \return the end point
 */
vec3 findRayEnd( in vec3 rayStart, out float d )
{
    // we need to ensure the vector components are not exactly 0.0 since they are used for division
    vec3 r = v_ray + vec3( 0.000000001 );
    vec3 p = rayStart;

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max( tBottom, tTop ) );
    return p + ( r * d );
}

/**
 * Returns the gradient vector at the given position.
 *
 * \param position the voxel for which to get the gradient
 *
 * \return the gradient, NOT normalized
 */
vec3 getGradient( in vec3 position )
{
#ifdef GRADIENTTEXTURE_ENABLED
    return ( 2.0 * texture3D( u_gradientsSampler, position ).rgb ) + vec3( -1.0 );
#else
    float s = 0.01;
    float valueXP = texture3D( u_volumeSampler, position + vec3( s, 0.0, 0.0 ) ).r;
    float valueXM = texture3D( u_volumeSampler, position - vec3( s, 0.0, 0.0 ) ).r;
    float valueYP = texture3D( u_volumeSampler, position + vec3( 0.0, s, 0.0 ) ).r;
    float valueYM = texture3D( u_volumeSampler, position - vec3( 0.0, s, 0.0 ) ).r;
    float valueZP = texture3D( u_volumeSampler, position + vec3( 0.0, 0.0, s ) ).r;
    float valueZM = texture3D( u_volumeSampler, position - vec3( 0.0, 0.0, s ) ).r;

    return vec3( valueXP - valueXM, valueYP - valueYM, valueZP - valueZM );
#endif
}

/**
 * Emulates the transfer function. This will be removed and replaced by a texture lookup.
 *
 * \param value the value to classify
 *
 * \return the color.
 */
vec4 transferFunction( float value )
{
#ifdef TRANSFERFUNCTION_ENABLED
    return texture1D( u_transferFunctionSampler, value );
#else
    // Example TF
    if( isZero( value - 0.5, 0.00005  ) )
        return vec4( 0.0 );
    vec4 c = vec4( 0.0 );
    if( value <= 0.5 )
    {
        c = vec4( 0.0, 0.37, 1.0, 2.0 * ( 0.5 - value ) );
    }
    else
    {
        vec4 cols[2];
        cols[0] = vec4( 1.0, 0.34, 0.34, 0.25 );
        cols[1] = vec4( 1.0, 0.75, 0.0, 0.25 );

        int i = int( 2.0 * ( value - 0.5 ) * 2.0 * 15.0 );
        int imod = i % 2;
        c = cols[ imod ];
    }
    return c;
#endif
}

/**
 * Evaluates the local illumination model at the given position in the volume.
 *
 * \param position the position inside the volume where to evaluate the illumination
 * \param color the color at the given position.
 *
 * \return the light color
 */
vec4 localIllumination( in vec3 position, in vec4 color )
{
#ifdef LOCALILLUMINATION_PHONG
    // get a gradient and get it to world-space
    vec3 g = getGradient( position );
    vec3 worldNormal = ( gl_ModelViewMatrix * vec4( g, 0.0 ) ).xyz;
    if( length( g ) < 0.01 )
    {
        return vec4( 0.0, 0.0, 0.0, 0.0 );
    }

    // let the normal point towards the viewer. Technically this would be:
    // worldNormal *= sign( dot( worldNormal, vec3( 0.0, 0.0, 1.0 ) ) );
    // but as the most of the components in the view vector are 0 we can use:
    worldNormal *= sign( worldNormal.z );

    // Phong:
    vec4 light = blinnPhongIllumination(
            0.1 * color.rgb,                              // material ambient
            1.0 * color.rgb,                                    // material diffuse
            0.5 * color.rgb,                              // material specular
            1000.0,                                         // shinines
            vec3( 1.0, 1.0, 1.0 ),                        // light diffuse
            vec3( 1.0, 1.0, 1.0 ),                        // light ambient
            normalize( worldNormal ),                     // normal
            vec3( 0.0, 0.0, 1.0 ),                        // view direction  // in world space, this always is the view-dir
            gl_LightSource[0].position.xyz                // light source position
    );
    light.a = color.a;
    return light;
#else
    return color;   // no illumination. In this case, no performance overhead is needed as functions get inlined.
#endif
}

/**
 * Calculates a^b. This is a faster, approximate implementation of GLSL's pow().
 *
 * \param a base
 * \param b exponent
 *
 * \return a^b.
 */
float fastpow( float a, float b )
{
    //return exp( log(a)*b );
    return pow( a, b );
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;      // the maximal distance along the ray until the BBox ends
    float currentDistance = 0.05;   // accumulated distance along the ray

#ifdef JITTERTEXTURE_ENABLED
    // stochastic jittering can help to void these ugly wood-grain artifacts with larger sampling distances but might
    // introduce some noise artifacts.
    float jitter = 0.5 - texture2D( u_jitterSampler, gl_FragCoord.xy / float( u_jitterSizeX ) ).r;
    vec3 rayStart = v_rayStart + ( v_ray * v_sampleDistance * jitter );
#else
    vec3 rayStart = v_rayStart;
#endif

    vec3 rayEnd = findRayEnd( rayStart, totalDistance );

    // walk along the ray
    vec4 dst = vec4( 0.0 );
    while( currentDistance <= ( totalDistance - 0.05 )  )
    {
        // do not dynamically branch every cycle for early-ray termination, so do n steps before checking alpha value
        for( int i = 0; i < 4; ++i )
        {
            // get current value, classify and illuminate
            vec3 rayPoint = rayStart + ( currentDistance * v_ray );
            vec4 src = localIllumination( rayPoint, transferFunction( texture3D( u_volumeSampler, rayPoint ).r ) );
            // associated colors needed
            src.rgb *= src.a;

#ifdef OPACITYCORRECTION_ENABLED
            // opacity correction
            src.r = 1.0 - fastpow( 1.0 - src.r, v_relativeSampleDistance );
            src.g = 1.0 - fastpow( 1.0 - src.g, v_relativeSampleDistance );
            src.b = 1.0 - fastpow( 1.0 - src.b, v_relativeSampleDistance );
            src.a = 1.0 - fastpow( 1.0 - src.a, v_relativeSampleDistance );
#endif

            // apply front-to-back compositing
            dst = ( 1.0 - dst.a ) * src + dst;

            // go to next value
            currentDistance += v_sampleDistance;
        }

        // early ray-termination
        if( dst.a >= 0.95 )
            break;
    }

    // have we hit something which was classified not to be transparent?
    // This is, visually, not needed but useful if volume renderer is used in conjunction with other geometry.
    // if( isZero( dst.a ) )
    // {
    //    discard;
    // }

    // get depth ... currently the frag depth.
    float depth = gl_FragCoord.z;

    // set final color
    gl_FragColor = dst;
    gl_FragDepth = depth;
}

