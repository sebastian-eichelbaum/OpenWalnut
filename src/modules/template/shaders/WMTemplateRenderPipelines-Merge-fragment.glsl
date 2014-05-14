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
 * The texture Unit for the edges
 */
uniform sampler2D u_texture1Sampler;

/**
 * Main. Calculates the Laplace Filter for each pixel.
 */
void main()
{
    // Grab texels and mix.
    vec4 color  = texture2D( u_texture0Sampler, gl_TexCoord[0].st );   // <- the cell shaded colors
    float edge  = texture2D( u_texture1Sampler, gl_TexCoord[1].st ).r;   // <- the edges
    vec4 edgeColor = vec4( 1.0 );

    // The nice thing is, the color input stores an alpha value which tells us whether the pixel was occupied or not. This way, we transport
    // transparence and "discards" from previous shaders properly.
    gl_FragColor = vec4( mix( color.rgb, edgeColor.rgb, edge ), color.a );
}

