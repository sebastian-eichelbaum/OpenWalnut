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

uniform int u_scalarDataSizeX;
uniform int u_scalarDataSizeY;
uniform int u_scalarDataSizeZ;

uniform vec3 u_aVec;
uniform vec3 u_bVec;
uniform float u_stepSize;

vec3 textPos( vec3 p )
{
    // compute texture coordinates from worldspace coordinates for texture access
    vec3 texturePosition = ( u_WorldTransform * vec4( p, 1.0 ) ).xyz;
    texturePosition.x /= u_scalarDataSizeX;
    texturePosition.y /= u_scalarDataSizeY;
    texturePosition.z /= u_scalarDataSizeZ;

    return texturePosition;
}

varying float d0;
varying float d1;
varying float d2;
varying float d3;

varying vec4 col;

/**
 * Vertex Main. Simply transforms the geometry.
 */
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0; // for distinguishing the verties of the quad
    gl_TexCoord[1] = gl_MultiTexCoord1; // for distinguishing the verties of the quad

    vec3 u_normA = normalize( u_aVec );
    vec3 u_normB = normalize( u_bVec );

    d0 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA * -1.0 + -u_normB ) * 0.5 * u_stepSize ) ).r;
    d1 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA + -u_normB ) * 0.5 * u_stepSize ) ).r;
    d2 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA +  u_normB ) * 0.5 * u_stepSize ) ).r;
    d3 = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz + ( u_normA * -1.0 +  u_normB ) * 0.5 * u_stepSize ) ).r;

    if( d0 > 1.0 || d1 > 1.0 || d2 > 1.0 || d3 > 1.0 )
    {
        col = vec4( 1.0, 0.0, 0.0 ,1.0 );
    }
    else
    {
        col = vec4( 0.0, 0.0, 1.0 ,1.0 );
    }

    float probability = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz ) ).r;

    gl_Position = gl_ModelViewProjectionMatrix * ( vec4( 1.0 * gl_TexCoord[0].xyz + gl_Vertex.xyz, 1.0 ) );
}
