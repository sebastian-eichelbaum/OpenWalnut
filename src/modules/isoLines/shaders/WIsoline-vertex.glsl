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

vec3 textPos( vec3 p )
{
    // compute texture coordinates from worldspace coordinates for texture access
    vec3 texturePosition = ( u_WorldTransform * vec4( p, 1.0 ) ).xyz;
    texturePosition.x /= u_scalarDataSizeX;
    texturePosition.y /= u_scalarDataSizeY;
    texturePosition.z /= u_scalarDataSizeZ;

    return texturePosition;
}

/**
 * Vertex Main. Simply transforms the geometry.
 */
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0; // for distinguishing the verties of the quad

    float probability = texture3D( u_scalarDataSampler, textPos( gl_Vertex.xyz ) ).r;

    gl_Position = gl_ModelViewProjectionMatrix * ( vec4( 0.95 * gl_TexCoord[0].xyz + gl_Vertex.xyz, 1.0 ) );
}
