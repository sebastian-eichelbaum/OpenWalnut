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

// This is needed if the modulo operator % is required
#extension GL_EXT_gpu_shader4 : enable

#ifdef LOCALILLUMINATION_PHONG
#include "WGEShadingTools.glsl"
#endif

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
    float s = 0.02;
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
        return vec4( 0.0 ); //vec4( g.x, g.y, g.z, 1.0 );
    }

    // let the normal point towards the viewer. Technically this would be:
    // worldNormal *= sign( dot( worldNormal, vec3( 0.0, 0.0, 1.0 ) ) );
    // but as most of the components in the view vector are 0 we can use:
    worldNormal *= sign( worldNormal.z );

    // Phong:
    vec4 light = blinnPhongIllumination(
            0.1 * color.rgb,                              // material ambient
            1.0 * color.rgb,                                    // material diffuse
            0.5 * color.rgb,                              // material specular
            100.0,                                         // shinines
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
    float currentDistance = 0.02;   // accumulated distance along the ray

#ifdef JITTERTEXTURE_ENABLED
    // stochastic jittering can help to void these ugly wood-grain artifacts with larger sampling distances but might
    // introduce some noise artifacts.
    float jitter = 0.5 - texture2D( u_jitterSampler, gl_FragCoord.xy / float( u_jitterSizeX ) ).r;
    vec3 rayStart = v_rayStart + ( v_ray * v_sampleDistance * jitter );
#else
    vec3 rayStart = v_rayStart;
#endif

    vec3 rayEnd = findRayEnd( rayStart, totalDistance );

#ifdef MIP_ENABLED
    // There is no nice early ray termination, so this will slow things
    // down a bit.
    float maxdist = -1.0;
    float maxalpha = 0.0;
    // walk along the ray
    vec4 dst = vec4( 0.0 );
    vec3 maxRayPoint;
    while( currentDistance <= ( totalDistance - 0.02 )  )
    {
        // get current value, classify and illuminate
        vec3 rayPoint = rayStart + ( currentDistance * v_ray );
        float alpha = transferFunction( texture3D( u_volumeSampler, rayPoint ).r ).a;
        if(alpha > maxalpha)
        {
            maxRayPoint = rayPoint;
            maxdist = currentDistance;
            maxalpha = alpha;
        }

        // go to next value
        currentDistance += v_sampleDistance;
    }

    // Get color at brightest point taken from alpha value of texture
    // both depth projection and mip need this information.
    if( maxdist > 0.0 )
    {
       dst = localIllumination( maxRayPoint, transferFunction( texture3D( u_volumeSampler, maxRayPoint ).r ) );
       dst.a = 1.0;
    }
#ifdef DEPTH_PROJECTION_ENABLED
    // FIXME: This is technically not correct as our ray starts at the boundary of the volume and not
    // in the view plane. Therefore, we get artifacts showing the edges and corners of the box.

    // map depth value to color
    if( maxdist > 0.0 )
    {
        const float MAX_DISTANCE = 1.1; // FIXME: estimate reasonable maximum value here
        float normval = maxdist/MAX_DISTANCE;

        // if( normval > 0.5 )
        // {
        //     dst = vec4( 0., normval-0.5, ( normval - 0.5 )*2.0, 1. );
        // }
        // else
        // {
        //     dst = vec4( 2.* ( 0.5-normval ), normval, 0., 1. );
        // }
        dst = vec4( texture1D( u_transferFunctionSampler, normval ).rgb, 1.0 );
    }
    else
    {
        dst = vec4( 0., 0., 0., 0. );
    }
#endif
#else
    // walk along the ray
    vec4 dst = vec4( 0.0 );
    while( currentDistance <= ( totalDistance - 0.02 )  )
    {
        // do not dynamically branch every cycle for early-ray termination, so do n steps before checking alpha value
        for( int i = 0; i < 10; ++i )
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

    /*        vec3 planeNorm = -v_ray;

            vec3 pSphere[16] = vec3[](vec3(0.53812504, 0.18565957, -0.43192),vec3(0.13790712, 0.24864247, 0.44301823),vec3(0.33715037, 0.56794053, -0.005789503),vec3(-0.6999805, -0.04511441, -0.0019965635),vec3(0.06896307, -0.15983082, -0.85477847),vec3(0.056099437, 0.006954967, -0.1843352),vec3(-0.014653638, 0.14027752, 0.0762037),vec3(0.010019933, -0.1924225, -0.034443386),vec3(-0.35775623, -0.5301969, -0.43581226),vec3(-0.3169221, 0.106360726, 0.015860917),vec3(0.010350345, -0.58698344, 0.0046293875),vec3(-0.08972908, -0.49408212, 0.3287904),vec3(0.7119986, -0.0154690035, -0.09183723),vec3(-0.053382345, 0.059675813, -0.5411899),vec3(0.035267662, -0.063188605, 0.54602677),vec3(-0.47761092, 0.2847911, -0.0271716));
            float aoFactor = 0.0;
            vec3 g = getGradient( rayPoint );

            for( int aoI = 0; aoI < 16; ++aoI )
            {
                vec3 dir = reflect( pSphere[aoI], g );
                for( int aoS = 0; aoS < 16; ++aoS )
                {
                    vec3 samplePoint = rayPoint + ( dir * v_sampleDistance * float(aoI+1) );
                vec4 sampleColor = transferFunction( texture3D( u_volumeSampler, samplePoint ).r );
                aoFactor += sampleColor.a;// * ( 1.0 / 16.0 );
            }
*/
            // apply front-to-back compositing
            dst = ( 1.0 - dst.a ) * src + dst;
            //dst.rgb *= 0.15*aoFactor;

            // go to next value
            currentDistance += v_sampleDistance;
        }

        // early ray-termination
        if( dst.a >= 0.999 )
            break;
    }
#endif

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

