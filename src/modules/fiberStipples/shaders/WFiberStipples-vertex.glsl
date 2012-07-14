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

#include "WGETextureTools.glsl"

/**
 * These two uniforms are needed to transform the vectors out of their texture back to their original form
 * as they are stored in RBGA (for example allowing only values between 0..1 for components but no negative
 * ones).
 */
uniform float u_vectorsMin;
uniform float u_vectorsScale;

/**
 * The matrix describes the transformation of gl_Vertex to OpenWalnut Scene Space
 */
uniform mat4 u_WorldTransform;

uniform sampler3D u_vectorsSampler;
uniform sampler3D u_probTractSampler;

uniform int u_probTractSizeX;
uniform int u_probTractSizeY;
uniform int u_probTractSizeZ;

// vectors spanning the plane of the quad
uniform vec3 u_aVec;
uniform vec3 u_bVec;

varying vec3 diffusionDirection;
varying vec3 focalPoint1;
varying vec3 focalPoint2;

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;

    // compute texture coordinate
    vec3 texturePosition = vec3( gl_Vertex.x / float(u_probTractSizeX), gl_Vertex.y / float(u_probTractSizeY), gl_Vertex.z / float(u_probTractSizeZ) );

    // span quad incase of regions with high probablility
    texturePosition.y = ( u_WorldTransform[3] / u_probTractSizeY ).y;
    if( texture3D( u_probTractSampler, texturePosition ).r > 0.01 ) // rgb are the same
    {
         // transform position, the 4th component must be explicitly set, as otherwise they would have been scaled
         gl_Position = gl_ModelViewProjectionMatrix * ( vec4( gl_TexCoord[0].xyz + gl_Vertex.xyz, 1.0 ) );
    }
    else
    {
         gl_Position = ftransform(); // discard those vertices
    }

    // get principal diffusion direction
    diffusionDirection = abs( texture3DUnscaled( u_vectorsSampler, texturePosition, u_vectorsMin, u_vectorsScale ) ).xyz;
    diffusionDirection = normalize( diffusionDirection );

    // project into plane (given by two vectors aVec and bVec)
    vec3 normal = normalize( cross( aVec, bVec ) );
    vec3 projectedDirection = diffusionDirection - dot( diffusionDirection, normal ) * normal;

    vec3 middlePoint = vec3( 0.5, 0.5, 0.0 ); // in texture coordinates
    focalPoint1 = middlePoint + projectedDirection;
    focalPoint2 = middlePoint - projectedDirection;
}
