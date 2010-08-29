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

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMProbTractDisplay-varyings.glsl"
#include "WGELighting-fragment.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

// The isovalue to use.
uniform float u_isovalue0;
uniform float u_isovalue1;
uniform float u_isovalue2;
uniform float u_isovalue3;

// Should the shader create some kind of isosurface instead of a volume rendering?
uniform bool u_isosurface;

// True if only the simple depth value should be used for coloring
uniform bool u_depthCueingOnly;

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha0;
uniform float u_alpha1;
uniform float u_alpha2;
uniform float u_alpha3;

// The color values to set
uniform vec4 u_isocolor0;
uniform vec4 u_isocolor1;
uniform vec4 u_isocolor2;
uniform vec4 u_isocolor3;

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
    vec3 r = v_ray + vec3( 0.0000001 );
    vec3 p = v_rayStart;

    // we need to ensure the vector components are not exactly 0.0

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max ( tBottom, tTop ) );
    return p + ( r * d );
}

float pointDistance( vec3 p1, vec3 p2 )
{
    return length( p1 - p2 );
}

void computeLight( inout vec4 col, in vec3 curPoint )
{
    vec3 pos = curPoint;
    pos.x = curPoint.x - 1.0;
    float fx1 = texture3D( tex0, pos ).r;
    pos.x = curPoint.x + 1.0;
    float fx2 = texture3D( tex0, pos ).r;
    pos.x = curPoint.x;
    pos.y = curPoint.y - 1.0;
    float fy1 = texture3D( tex0, pos ).r;
    pos.y = curPoint.y + 1.0;
    float fy2 = texture3D( tex0, pos ).r;
    pos.y = curPoint.y;
    pos.z = curPoint.z - 1.0;
    float fz1 = texture3D( tex0, pos ).r;
    pos.z = curPoint.z + 1.0;
    float fz2 = texture3D( tex0, pos ).r;
    pos.z = curPoint.z;
    vec3 normal;
    normal.x = ( fx1 - fx2 ) / 2.0;
    normal.y = ( fy1 - fy2 ) / 2.0;
    normal.z = ( fz1 - fz2 ) / 2.0;

    vec4 ambient = vec4(0.0);
    vec4 diffuse = vec4(0.0);
    vec4 specular = vec4(0.0);

    calculateLighting(-normal, gl_FrontMaterial.shininess, ambient, diffuse, specular);

    col = col + (ambient * col / 2.0) + (diffuse * col) + (specular * col / 2.0);

    col = clamp(col, 0.0, 1.0);
}

bool computeColor( in vec3 curPoint, in float isovalue, inout vec4 colorSoFar )
{
    if( abs( isovalue - u_isovalue0 ) >= 0.1 &&  abs( isovalue - u_isovalue1 ) >= 0.1 &&
        abs( isovalue - u_isovalue2 ) >= 0.1 &&  abs( isovalue - u_isovalue3 ) >= 0.1 )
    {
        return false;
    }
    // we need to know the depth value of the current point inside the cube
    // Therefore, the complete standard pipeline is reproduced here:

    // 1: transfer to world space and right after it, to eye space
    vec4 curPointProjected = gl_ModelViewProjectionMatrix * vec4( curPoint, 1.0 );

    // 2: scale to screen space and [0,1]
    // -> x and y is not needed
    curPointProjected.z /= curPointProjected.w;
    curPointProjected.z  = curPointProjected.z * 0.5 + 0.5 ;

    // 3: set depth value
    gl_FragDepth = curPointProjected.z;

    // 4: get alpha and color for the corresponding isolevel
    vec4 color;
    if( abs( isovalue - u_isovalue0 ) < 0.1 ) {
        color   = u_alpha0 * u_isocolor0;
        color.a = u_alpha0;
    }
    else if( abs( isovalue - u_isovalue1 ) < 0.1 ) {
        color   = u_alpha1 * u_isocolor1;
        color.a = u_alpha1;
    }
    else if( abs( isovalue - u_isovalue2 ) < 0.1 ) {
        color   = u_alpha2 * u_isocolor2;
        color.a = u_alpha2;
    }
    else if( abs( isovalue - u_isovalue3 ) < 0.1 ) {
        color   = u_alpha3 * u_isocolor3;
        color.a = u_alpha3;
    }
    else {
        return false;
    }

    computeLight( color, curPoint );

    // 5: set color
    colorSoFar = (1.0 - colorSoFar.a) * color + colorSoFar;
    return true;
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // please do not laugh, it is a very very very simple "isosurface" shader
    gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = gl_FragCoord.z;

    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;
    vec3 rayEnd = findRayEnd( totalDistance );

    // Isosurface Mode
    if ( u_isosurface )
    {
        // the point along the ray in cube coordinates
        vec3 curPoint = v_rayStart;

        // the current value inside the data
        float value;

        // the step counter
        int i = 0;
        float stepDistance = totalDistance / float( u_steps );
        vec4 colorSoFar = vec4( 0.0, 0.0, 0.0, 0.0 );
        bool hit = false;

        while ( i < u_steps && colorSoFar.a < 0.95 ) // we do not need to ch
        {
            // get current value
            value = texture3D( tex0, curPoint ).r;

            hit = computeColor( curPoint, value, colorSoFar ) || hit;

            curPoint += stepDistance * v_ray;

            // do not miss to count the steps already done
            i++;
        }
        gl_FragColor = colorSoFar;

        // the ray did never hit the surface --> discard the pixel
        if ( !hit )
        {
            discard;
        }
    }
}
