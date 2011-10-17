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

#ifndef WGELIGHTING_FRAGMENT_GLSL
#define WGELIGHTING_FRAGMENT_GLSL

#version 120

varying vec3 normal;
varying vec4 vertex;
varying vec3 halfvec;

const vec4 AMBIENT_BLACK = vec4( 0.0, 0.0, 0.0, 1.0 );
const vec4 DEFAULT_BLACK = vec4( 0.0, 0.0, 0.0, 0.0 );

void directionalLight( in int i, in vec3 normal, in float shininess,
                      inout vec4 ambient, inout vec4 diffuse, inout vec4 specular )
{
    float nDotVP;
    float nDotHV;
    float pf;

    vec3 L = normalize( gl_LightSource[i].position.xyz - vertex.xyz );
    vec3 H = normalize( L + halfvec.xyz );

    nDotVP = max( 0.0, dot( normal, normalize( ( gl_LightSource[i].position.xyz ) ) ) );
    nDotHV = max( 0.0, dot( normal, H ) );

    if( nDotVP == 0.0 )
        pf = 0.0;
    else
        pf = pow( nDotHV, gl_FrontMaterial.shininess );

    ambient += gl_LightSource[i].ambient;
    diffuse += gl_LightSource[i].diffuse * nDotVP;
    specular += gl_LightSource[i].specular * pf;
}

void calculateLighting( in vec3 N, in float shininess, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular )
{
    directionalLight( 0, N, shininess, ambient, diffuse, specular );
}

#endif // WGELIGHTING_FRAGMENT_GLSL

