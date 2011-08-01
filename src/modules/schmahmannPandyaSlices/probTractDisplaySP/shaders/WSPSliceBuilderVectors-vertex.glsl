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

uniform float u_glyphSize;
varying vec4 v_middlePoint;

void main()
{
    gl_TexCoord[0] = u_glyphSize * gl_MultiTexCoord0;
    gl_TexCoord[1] = u_glyphSize * gl_MultiTexCoord1;
    gl_TexCoord[2] = u_glyphSize * gl_MultiTexCoord2;
    vec4 v = gl_Vertex + vec4( gl_TexCoord[0].xyz, 0.0 );
    v_middlePoint =  gl_Vertex;

    gl_FrontColor = gl_Color;

    gl_Position = gl_ModelViewProjectionMatrix * v;
}
