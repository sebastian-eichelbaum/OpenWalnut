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

#include "WGEUtils.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMDirectVolumeRendering-varyings.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;
uniform sampler1D TRANSFERFUNCTION_SAMPLER;
uniform sampler3D GRADIENTTEXTURE_SAMPLER;

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
 *
 * \return the end point
 */
vec3 findRayEnd( out float d )
{
    // we need to ensure the vector components are not exactly 0.0 since they are used for division
    vec3 r = v_ray + vec3( 0.000000001 );
    vec3 p = v_rayStart;

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
    return ( 2.0 * texture3D( GRADIENTTEXTURE_SAMPLER, position ).rgb ) + vec3( -1.0 );
#else
    float s = 0.01;
    float valueXP = texture3D( tex0, position + vec3( s, 0.0, 0.0 ) ).r;
    float valueXM = texture3D( tex0, position - vec3( s, 0.0, 0.0 ) ).r;
    float valueYP = texture3D( tex0, position + vec3( 0.0, s, 0.0 ) ).r;
    float valueYM = texture3D( tex0, position - vec3( 0.0, s, 0.0 ) ).r;
    float valueZP = texture3D( tex0, position + vec3( 0.0, 0.0, s ) ).r;
    float valueZM = texture3D( tex0, position - vec3( 0.0, 0.0, s ) ).r;

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
    return texture1D( TRANSFERFUNCTION_SAMPLER, value );
#else
    if ( isZero( value - 0.5, 0.01 ) )   // if not TF has been specified, at least show something
    {
        return vec4( 1.0, 0.0, 0.0, 0.1 );
    }
    else
    {
        return vec4( 0.0 );
    }
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
    // Phong:
    vec4 light = blinnPhongIllumination(
            0.1 * color.rgb,                              // material ambient
            color.rgb,                                    // material diffuse
            1.0 * color.rgb,                              // material specular
            10.0,                                         // shinines
            vec3( 1.0, 1.0, 1.0 ),                        // light diffuse
            vec3( 0.3, 0.3, 0.3 ),                        // light ambient
            normalize( -getGradient( position ) ),        // normal
            v_ray,                                        // view direction
            v_lightSource                                 // light source position
    );
    light.a = color.a;
    return light;
#else
    return color;   // no illumination. In this case, no performance overhead is needed as functions get inlined.
#endif
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;      // the maximal distance along the ray until the BBox ends
    float currentDistance = 0.0;    // accumulated distance along the ray
    vec3 rayEnd = findRayEnd( totalDistance );

    // walk along the ray
    vec4 dst = vec4( 0.0 );
    while ( currentDistance <= totalDistance )
    {
        // get current value, classify and illuminate
        vec3 rayPoint = v_rayStart + ( currentDistance * v_ray );
        vec4 src = localIllumination( rayPoint, transferFunction( texture3D( tex0, rayPoint ).r ) );

        // go to next value
        currentDistance += v_stepDistance;

        // apply front-to-back compositing
        dst = ( 1.0 - dst.a ) * src + dst;

        if ( dst.a >= 0.95 )
            break;
    }

    // have we hit something which was classified not to be transparent?
    // This is, visually, not needed but useful if volume rendere is used in conjunction with other geometry.
    // if ( isZero( dst.a ) )
    // {
    //    discard;
    // }

    // set final color
    float depth = gl_FragCoord.z;
    gl_FragColor = dst;
    gl_FragDepth = depth;                    // the depth of the last hit
}
