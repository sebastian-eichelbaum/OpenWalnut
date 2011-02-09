//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

#ifndef WGELIGHTING_VERTEX_GLSL
#define WGELIGHTING_VERTEX_GLSL

#version 120

varying vec3 normal;
varying vec4 vertex;
varying vec3 halfvec;

const vec4 AMBIENT_BLACK = vec4( 0.0, 0.0, 0.0, 1.0 );
const vec4 DEFAULT_BLACK = vec4( 0.0, 0.0, 0.0, 0.0 );

/**
 * Computes normal and makes transformation to eye space.
 */
void prepareLight()
{
    // Calculate the normal
    normal = normalize( gl_NormalMatrix * gl_Normal ).xyz;

    // Transform the vertex position to eye space
    vertex = vec4( gl_ModelViewMatrix * gl_Vertex );

    vec3 L = normalize( gl_LightSource[0].position.xyz - vertex.xyz );

    halfvec = vec3( L + vec3( 0.0, 0.0, 1.0 ) );
}

#endif // WGELIGHTING_VERTEX_GLSL

