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

#include "WGEShadingTools.glsl"
#include "WGEUtils.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMMultiIsosurfaceRaytracer-varyings.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

// The isovalue to use.
uniform float u_isovalue1 = 0.33;
uniform float u_isovalue2 = 0.80;

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha1 = 0.5;
uniform float u_alpha2 = 1.0;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

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

vec4 transferFunction( in float value )
{
    if ( isZero( value - u_isovalue1, 0.1 ) )
    {
        return vec4( 2.*value, 0.0, 0.0, 0.025 );
    }
    else if ( isZero( value - u_isovalue2, 0.1 ) )
    {
        return vec4( value, value, 0.0, 0.1 );
    }
    else
    {
        return vec4( 0.0 );
    }
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
    vec4 color = vec4( vec3( 0.0 ), 1.0 );  // the composited color
    float alpha = 1.0;
    float depth = gl_FragCoord.z;           // the depth of the last hit
    float value;                            // the current value inside the data
    float hit = 0.0;                        // this value will be != 0.0 if something has been hit
    while ( currentDistance <= totalDistance ) // we do not need to ch
    {
        // get current value
        vec3 rayPoint = rayEnd - ( currentDistance * v_ray );
        value = texture3D( tex0, rayPoint).r;

        vec4 vColor = transferFunction( value );    // classify at the sample point
        color.rgba = mix( color.rgba, vColor.rgba, vColor.a );  // compositing
        alpha *= ( 1.0 - vColor.a );                            // compositing: keep track of final alpha value of the background

        // go to next value
        currentDistance += v_stepDistance;

        // has there ever been something we hit?
        hit = max( hit, vColor.a );
        
        /*if ( !isZero( vColor.a, 0.1 ) )
        {
            // 1: transfer to world space and right after it, to eye space
            vec4 curPointProjected = gl_ModelViewProjectionMatrix * vec4( rayPoint, 1.0 );

            // 2: scale to screen space and [0,1]
            // -> x and y is not needed
            // curPointProjected.x /= curPointProjected.w;
            // curPointProjected.x  = curPointProjected.x * 0.5 + 0.5 ;
            // curPointProjected.y /= curPointProjected.w;
            // curPointProjected.y  = curPointProjected.y * 0.5 + 0.5 ;
            curPointProjected.z /= curPointProjected.w;
            curPointProjected.z  = curPointProjected.z * 0.5 + 0.5;

            // 3: set depth value
            gl_FragDepth = curPointProjected.z;
        }*/
    }

    // have we hit something which was classified not to be transparent?
    // This is, visually, not needed but useful if volume rendere is used in conjunction with other geometry.
    if ( isZero( hit ) )
    {
        discard;
    }

    // set final color
    color.a = 1.0 - alpha;
    gl_FragColor = color;
    gl_FragDepth = depth;
}

