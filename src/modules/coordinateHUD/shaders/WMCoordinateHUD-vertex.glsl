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

#version 130

void main()
{
    // keep color
    gl_FrontColor = gl_Color;

    // texture
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // get the scaling applied during transformation from model space to camera space
    vec4 vertexScale = gl_ModelViewMatrix * vec4( 1.0, 0.0, 0.0, 0.0 ); // <- needs to be of length 1
    float s = 1.0 / length( vertexScale );

    float size = 25.0;

    // project
    vec4 vertexInScreenSpace = gl_ModelViewProjectionMatrix * vec4( s * size * gl_Vertex.xyz, 0.0  );
    vertexInScreenSpace.w = 1.0;
    gl_Position = vec4( -0.75, -0.75, 0.0, 0.0 ) + vertexInScreenSpace;
}

