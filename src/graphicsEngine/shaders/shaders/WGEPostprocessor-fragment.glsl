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
 * The texture Unit for the original color field
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the Normal Map
 */
uniform sampler2D u_texture1Sampler;

/**
 * The depth texture
 */
uniform sampler2D u_texture2Sampler;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeX;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeY;

/**
 * Size of texture in pixels
 */
uniform int u_texture0SizeZ;

/**
 * Main. Calculates the Laplace Filter for each pixel.
 */
void main()
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // GETTING TEXELS
    //
    // Get surrounding texels; needed for ALL filters
    /////////////////////////////////////////////////////////////////////////////////////////////

    // get data of surrounding textels
    float offsetW = 2.0 / u_texture0SizeX;
    float offsetH = 2.0 / u_texture0SizeY;

    vec2 texCoord = gl_TexCoord[0].st;
    float c  = texture2D( u_texture1Sampler, texCoord ).a;
    float bl = texture2D( u_texture1Sampler, texCoord + vec2( -offsetW, -offsetH ) ).a;
    float l  = texture2D( u_texture1Sampler, texCoord + vec2( -offsetW,     0.0  ) ).a;
    float tl = texture2D( u_texture1Sampler, texCoord + vec2( -offsetW,  offsetH ) ).a;
    float t  = texture2D( u_texture1Sampler, texCoord + vec2(     0.0,   offsetH ) ).a;
    float tr = texture2D( u_texture1Sampler, texCoord + vec2(  offsetW,  offsetH ) ).a;
    float r  = texture2D( u_texture1Sampler, texCoord + vec2(  offsetW,     0.0  ) ).a;
    float br = texture2D( u_texture1Sampler, texCoord + vec2(  offsetW,  offsetH ) ).a;
    float b  = texture2D( u_texture1Sampler, texCoord + vec2(     0.0,  -offsetH ) ).a;

    vec4 col  = texture2D( u_texture0Sampler, texCoord );
    float depth  = texture2D( u_texture2Sampler, texCoord ).r;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LAPLACE
    //
    // apply a standart laplace filter kernel
    /////////////////////////////////////////////////////////////////////////////////////////////

    // laplace filter kernel
    float edge = 4.0 * abs(
            0.0 * tl +  1.0 * t + 0.0 * tr +
            1.0 * l  +  -4.0 * c + 1.0 * r  +
            0.0 * bl +  1.0 * b + 0.0 * br
        );

    gl_FragColor = (1.-edge)*col + vec4( 1.0, 0.0, 0.0, 1.0 ) * edge;//vec4( vec3( nd.a ), 1.0 );
    gl_FragDepth = depth;
}

