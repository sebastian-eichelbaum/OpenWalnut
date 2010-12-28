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

varying float tangent_dot_view;
varying vec3 tangentR3;
varying float s_param;
varying vec4 myColor;
varying vec4 VaryingTexCoord0;

uniform float u_thickness;

void main()
{
    VaryingTexCoord0 = gl_Vertex;

    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; //< make clipping planes work

    vec3 tangent;

    float thickness = 0.0001 * u_thickness;

    tangentR3 = gl_Normal;
    tangent = ( gl_ModelViewProjectionMatrix * vec4( gl_Normal, 0. ) ).xyz; //< transform our tangent vector
    s_param = gl_MultiTexCoord0.x; //< store texture coordinate for shader

    vec3 offsetNN = cross( normalize( tangent.xyz ), vec3( 0., 0., -1. ) );
    vec3 offset = normalize( offsetNN );
    tangent_dot_view = length( offsetNN );

    vec4 pos = ftransform(); //< transform position to eye space

    offset.x *= thickness;
    offset.y *= thickness;

    pos.xyz = ( offset * s_param ) + pos.xyz; //< add offset in y-direction (eye-space)

    myColor = gl_Color;

    gl_Position = pos; //< store final position
}
