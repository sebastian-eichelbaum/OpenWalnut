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

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

varying in vec3 v_normal[];

/**
 * Main entry point of the geometry shader.
 */
void main()
{
    vec4 vertex = gl_PositionIn[0];
    vec3 tangent = normalize( v_normal[0] );
    vec3 view = vec3( 0.0, 0.0, -1.0 );
    vec4 normal = vec4( cross( tangent, view ), 0.0 );
    vec4 a = vec4( cross( normal.xyz, tangent ), 0.0 );

    gl_FrontColor = vec4( vec3(1.0), 1.0 );
    gl_BackColor = vec4( vec3(1.0), 1.0 );

    gl_Position = gl_ModelViewProjectionMatrix * ( vertex + ( -0.5 * normal ) + ( -0.5 * a ) ); EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix * ( vertex + ( -0.5 * normal ) + (  0.5 * a ) ); EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix * ( vertex + (  0.5 * normal ) + ( -0.5 * a ) ); EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix * ( vertex + (  0.5 * normal ) + (  0.5 * a ) ); EmitVertex();

    EndPrimitive();
}

