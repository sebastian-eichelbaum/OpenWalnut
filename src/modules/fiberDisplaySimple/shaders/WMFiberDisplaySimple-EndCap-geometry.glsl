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

#extension GL_EXT_geometry_shader4 : enable

#include "WGETransformationTools.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * A point on the plane.
 */
uniform vec3 u_planePoint;

/**
 * The normal vector of the plane
 */
uniform vec3 u_planeVector;

/**
 * The max distance allowed
 */
uniform float u_distance = 1.0;

/**
 * The size of the tube
 */
uniform float u_tubeSize;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

/**
 * The normal/tangent in scene-space
 */
varying in vec3 v_normal[];

/**
 * The output normal for the fragment shader in world-space
 */
varying out vec3 v_normalWorld;

/**
 * The vertex coordinates in world-space
 */
varying out vec4 v_vertex;

/**
 * The scaling component of the modelview matrix.
 */
varying out float v_worldScale;

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the geometry shader.
 */
void main()
{
#ifdef CLIPPLANE_ENABLED
    // define the plane
    vec3 n = normalize( u_planeVector );
    float d = dot( u_planePoint, n );
    float dist = dot( gl_PositionIn[0].xyz, n ) - d;

    if( abs( dist ) >= u_distance )
    {
        return;
    }
#endif  // CLIPPLANE_ENABLED

#if !( defined( ENDCAPS_ENABLED ) )
    // no endcaps?
    // do not emit any vertex
    return;
#endif  // ENDCAPS_ENABLED

    // discard the caps if in ribbon mode or non-zoomable mode. it not yet works.
#if !( defined( RIBBON_ENABLED ) || !defined( ZOOMABLE_ENABLED ) )

    // grab the info we got from the vertex array:
    vec3 tangent = normalize( v_normal[0] );
    vec4 vertex = gl_PositionIn[0];

    // camera view direction
    vec3 view = normalize( ( gl_ModelViewMatrixInverse * vec4( 0.0, 0.0, -1.0, 0.0 ) ).xyz );
    float angle = step( 0.0, sign( dot( view, tangent ) ) );

    // the plane of the sprite is determined by the tangent ond two orthogonal vectors a and b
    vec4 a = 0.2 * u_tubeSize * vec4( normalize( cross( tangent, view ) ), 0.0 );
    vec4 b = 0.2 * u_tubeSize * vec4( normalize( cross( tangent, a.xyz ) ), 0.0 );

    gl_FrontColor = gl_FrontColorIn[0];

    // create the quad
    v_normalWorld = ( gl_ModelViewMatrix * vec4( v_normal[0], 0.0 ) ).xyz;
    // let it point towards the camera
    v_normalWorld *= sign( dot( normalize( v_normalWorld ), vec3( 0.0, 0.0, 1.0 ) ) );

    v_worldScale = 0.1 * getModelViewScale();

    // vertex 1
    gl_TexCoord[0] = vec4( -1.0, -1.0, 0.0, angle );
    v_vertex = gl_ModelViewMatrix * ( vertex + ( -0.5 * a ) + ( -0.5 * b ) );
    gl_Position = gl_ProjectionMatrix * v_vertex;
    EmitVertex();

    // vertex 2
    gl_TexCoord[0] = vec4( -1.0, 1.0, 0.0, angle );
    v_vertex = gl_ModelViewMatrix * ( vertex + ( -0.5 * a ) + ( 0.5 * b ) );
    gl_Position = gl_ProjectionMatrix * v_vertex;
    EmitVertex();

    // vertex 3
    gl_TexCoord[0] = vec4( 1.0, -1.0, 0.0, angle );
    v_vertex = gl_ModelViewMatrix * ( vertex + ( 0.5 * a ) + ( -0.5 * b ) );
    gl_Position = gl_ProjectionMatrix * v_vertex;
    EmitVertex();

    // vertex 4
    gl_TexCoord[0] = vec4( 1.0, 1.0, 0.0, angle );
    v_vertex = gl_ModelViewMatrix * ( vertex + ( 0.5 * a ) + ( 0.5 * b ) );
    gl_Position = gl_ProjectionMatrix * v_vertex;
    EmitVertex();

    EndPrimitive();
#endif
}

