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
 * The texture Unit for the colors
 */
uniform sampler2D u_texture0Sampler;

/**
 * Main. Calculates the cel shading effect for each pixel.
 */
void main()
{
    // Get input
    vec2 texCoord = gl_TexCoord[0].st;
    vec4 c = texture2D( u_texture0Sampler, gl_TexCoord[0].st );

    // Reduce colors and output again.
    float samples = 3;
    gl_FragColor = vec4(
                         vec3( int( ( c * samples ).r ),
                               int( ( c * samples ).g ),
                               int( ( c * samples ).b ) ) / samples, c.a );

    // IMPORTANT: we transport the alpha value of the original input pixel here.
}

