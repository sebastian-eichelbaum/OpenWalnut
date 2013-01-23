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
 * The size of the sprite
 */
uniform float u_pointSize = 1.0;

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
 * The vertex coordinates in world-space
 */
varying out vec3 v_vertex;

/**
 * The texture coordinates in [-1,1]
 */
varying out vec3 v_texCoord;

/**
 * The radius of a sphere in world-space
 */
varying out float v_worldSpaceRadius;

/**
 * The center point of the sphere
 */
varying out vec3 v_centerPoint;

/**
 * Depth of the center point
 */
varying out float v_centerVertexDepth;

/**
 * Deptj of the neareast point on the sphere towards the camera
 */
varying out float v_nearestVertexDepth;

/**
 * Difference between nearest depth and center point depth
 */
varying out float v_vertexDepthDiff;

/**
 * The world-space scaling factor
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
    // grab the info we got from the vertex array:
    vec3 vertex = gl_PositionIn[0].xyz;

    // the plane of the sprite is determined by the tangent ond two orthogonal vectors a and b
    // NOTE: the 1.0 corresponds to a radius of 1 (unit-sphere)
    vec3 a = vec3( 0.0, 1.0, 0.0 );
    vec3 b = vec3( 1.0, 0.0, 0.0 );

    gl_FrontColor = gl_FrontColorIn[0];

    v_worldScale = getModelViewScale();
    v_worldSpaceRadius = v_worldScale * u_pointSize;
    v_centerPoint = vertex;

    // calculate the depth using the point on the sphere most near to the cam
    vec4 nearestVertex = vec4( v_centerPoint.xy, v_centerPoint.z - v_worldSpaceRadius, 1.0 );
    vec4 nearestVertexProjected = gl_ProjectionMatrix * nearestVertex;
    v_nearestVertexDepth = ( 0.5 * nearestVertexProjected.z / nearestVertexProjected.w ) + 0.5;

    // depth at the center point
    vec4 centerVertex = vec4( v_centerPoint.xyz, 1.0 );
    vec4 centerVertexProjected = gl_ProjectionMatrix * centerVertex;
    v_centerVertexDepth = ( 0.5 * centerVertexProjected.z / centerVertexProjected.w ) + 0.5;

    v_vertexDepthDiff = v_centerVertexDepth - v_nearestVertexDepth;

    // vertex 1
    v_texCoord = ( -a ) + ( -b );
    v_vertex = ( v_centerPoint + v_worldSpaceRadius * v_texCoord );
    gl_Position = gl_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 2
    v_texCoord = ( -a ) + ( +b );
    v_vertex = ( v_centerPoint + v_worldSpaceRadius * v_texCoord );
    gl_Position = gl_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 3
    v_texCoord = ( +a ) + ( -b );
    v_vertex = ( v_centerPoint + v_worldSpaceRadius * v_texCoord );
    gl_Position = gl_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    // vertex 4
    v_texCoord = ( +a ) + ( +b );
    v_vertex = ( v_centerPoint + v_worldSpaceRadius * v_texCoord );
    gl_Position = gl_ProjectionMatrix * vec4( v_vertex, 1.0 );
    EmitVertex();

    EndPrimitive();
}

