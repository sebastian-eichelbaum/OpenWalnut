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

uniform int u_vertexShift;
uniform vec3 u_vertexShiftDirection;

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    // Calculate the real vertex coordinate in openwalnut-scene-space
    vec4 vertex = ( vec4( u_vertexShiftDirection.xyz, 0.0 ) * u_vertexShift ) + gl_Vertex;

    // for easy access to texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * vertex;

    gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
