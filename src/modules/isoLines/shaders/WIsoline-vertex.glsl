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

/**
 * The matrix describes the transformation of gl_Vertex to OpenWalnut Scene Space
 */
uniform mat4 u_WorldTransform;

/**
 * Scalar dataset to draw contours from as texture
 */
uniform sampler3D u_scalarDataSampler;

/**
 * Isovalue
 */
uniform float u_isovalue;

uniform int u_scalarDataSizeX;
uniform int u_scalarDataSizeY;
uniform int u_scalarDataSizeZ;

uniform vec3 u_aVec;
uniform vec3 u_bVec;
uniform float u_stepSize;

varying vec3 hit0Pos;
varying vec3 hit1Pos;
varying vec3 hit2Pos;
varying vec3 hit3Pos;

varying float sumHits;

vec3 textPos( vec3 p )
{
    // compute texture coordinates from worldspace coordinates for texture access
    vec3 texturePosition = ( u_WorldTransform * vec4( p, 1.0 ) ).xyz;
    texturePosition.x /= u_scalarDataSizeX;
    texturePosition.y /= u_scalarDataSizeY;
    texturePosition.z /= u_scalarDataSizeZ;

    return texturePosition;
}

varying float edge0Hit_f;
varying float edge1Hit_f;
varying float edge2Hit_f;
varying float edge3Hit_f;

/**
 * Vertex Main. Simply transforms the geometry.
 */
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0; // for distinguishing the verties of the quad
    gl_TexCoord[1] = gl_MultiTexCoord1; // for distinguishing the verties of the quad

    vec3 u_normA = normalize( u_aVec );
    vec3 u_normB = normalize( u_bVec );

    float d0 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA * -1.0 + -u_normB ) * 0.5 * u_stepSize ) ).r;
    float d1 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA + -u_normB ) * 0.5 * u_stepSize ) ).r;
    float d2 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA +  u_normB ) * 0.5 * u_stepSize ) ).r;
    float d3 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA * -1.0 +  u_normB ) * 0.5 * u_stepSize ) ).r;

    // check which edges of the quad were hit
    edge0Hit_f = float( d0 >= u_isovalue && d1 <= u_isovalue || d0 <= u_isovalue && d1 >= u_isovalue );
    edge1Hit_f = float( d1 >= u_isovalue && d2 <= u_isovalue || d1 <= u_isovalue && d2 >= u_isovalue );
    edge2Hit_f = float( d2 >= u_isovalue && d3 <= u_isovalue || d2 <= u_isovalue && d3 >= u_isovalue );
    edge3Hit_f = float( d3 >= u_isovalue && d0 <= u_isovalue || d3 <= u_isovalue && d0 >= u_isovalue );

    // determine the position where the corresponding edge was hitten (in 0,1 clamped relative coordinates)
    hit0Pos = vec3( clamp( abs( d0 - u_isovalue ) / abs( d0 - d1 ), 0.0, 1.0 ), 0.0, 0.0 );
    hit1Pos = vec3( 1.0, clamp( abs( d1 - u_isovalue ) / abs( d1 - d2 ), 0.0, 1.0 ), 0.0 );
    hit2Pos = vec3( 1.0 - clamp( abs( d2 - u_isovalue ) / abs( d2 - d3 ), 0.0, 1.0 ), 1.0, 0.0 );
    hit3Pos = vec3( 0.0, 1.0 - clamp( abs( d3 - u_isovalue ) / abs( d0 - d3 ), 0.0, 1.0 ), 0.0 );

    sumHits = float( int( edge0Hit_f ) + int( edge1Hit_f ) * 2 + int( edge2Hit_f ) * 4 + int( edge3Hit_f ) * 8 );

    gl_Position = gl_ModelViewProjectionMatrix * ( vec4( 1.0 * gl_TexCoord[0].xyz + gl_Vertex.xyz, 1.0 ) );
}
