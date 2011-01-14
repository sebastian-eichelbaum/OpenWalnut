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
    // where are we?
    vec2 texCoord = gl_TexCoord[0].st;

    // the final color will be put here
    vec4 color = vec4( 0.0 );
    float depth  = texture2D( u_texture2Sampler, texCoord ).r;

#ifdef WGE_POSTPROCESSOR_COLOR
    color = ( 1.0 - color.a ) * texture2D( u_texture0Sampler, texCoord ) + color;
#endif

#ifdef WGE_POSTPROCESSOR_DEPTH
    color = ( 1.0 - color.a ) * vec4( vec3( depth ), 1.0 ) + color;
#endif

#ifdef WGE_POSTPROCESSOR_EDGE

    /////////////////////////////////////////////////////////////////////////////////////////////
    // GETTING TEXELS
    //
    // Get surrounding texels; needed for ALL filters
    /////////////////////////////////////////////////////////////////////////////////////////////

    // get data of surrounding textels
    float offsetW = 1.0 / u_texture0SizeX;
    float offsetH = 1.0 / u_texture0SizeY;

    float c  = texture2D( u_texture2Sampler, texCoord ).r;
    float bl = texture2D( u_texture2Sampler, texCoord + vec2( -offsetW, -offsetH ) ).r;
    float l  = texture2D( u_texture2Sampler, texCoord + vec2( -offsetW,     0.0  ) ).r;
    float tl = texture2D( u_texture2Sampler, texCoord + vec2( -offsetW,  offsetH ) ).r;
    float t  = texture2D( u_texture2Sampler, texCoord + vec2(     0.0,   offsetH ) ).r;
    float tr = texture2D( u_texture2Sampler, texCoord + vec2(  offsetW,  offsetH ) ).r;
    float r  = texture2D( u_texture2Sampler, texCoord + vec2(  offsetW,     0.0  ) ).r;
    float br = texture2D( u_texture2Sampler, texCoord + vec2(  offsetW,  offsetH ) ).r;
    float b  = texture2D( u_texture2Sampler, texCoord + vec2(     0.0,  -offsetH ) ).r;

    vec4 col  = texture2D( u_texture0Sampler, texCoord );
    vec4 normal  = texture2D( u_texture1Sampler, texCoord );

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LAPLACE
    //
    // apply a standart laplace filter kernel
    /////////////////////////////////////////////////////////////////////////////////////////////

    // laplace filter kernel
    float edge = 1.0 * abs(
            0.0 * tl +  1.0 * t + 0.0 * tr +
            1.0 * l  +  -4.0 * c + 1.0 * r  +
            0.0 * bl +  1.0 * b + 0.0 * br
        );
    color = ( 1.0 - color.a ) * vec4( vec3( 16.0*edge ), 1.0 ) + color;
#endif

    // gl_FragColor = (1.-edge)*col + vec4( 1.0, 0.0, 0.0, 1.0 ) * edge;
    gl_FragColor = color;
    gl_FragDepth = depth;
}

