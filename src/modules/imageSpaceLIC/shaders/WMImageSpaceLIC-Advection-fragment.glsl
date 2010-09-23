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
 * The texture Unit for the projected vectors
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the edges and noise
 */
uniform sampler2D u_texture1Sampler;

/**
 * The texture Unit for the last advection
 */
uniform sampler2D u_texture2Sampler;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture2SizeX;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture2SizeY;

/**
 * Size of advection texture in pixels
 */
uniform int u_texture2SizeZ;

/**
 * The blending ratio between noise and advected noise
 */
uniform float u_noiseRatio;

/**
 * Main. Calculates the Laplace Filter for each pixel.
 */
void main()
{
    vec2 texCoord = gl_TexCoord[0].st;
    
    // get some needed values
    float edge  = texture2D( u_texture1Sampler, texCoord ).r;
    float depth  = texture2D( u_texture1Sampler, texCoord ).g;
    float noise  = texture2D( u_texture1Sampler, texCoord ).b;
    vec2 vec = normalize( 2.0 * ( texture2D( u_texture0Sampler, texCoord ).rg - vec2( 0.5, 0.5 ) ) );

    // last advection:
    float last = texture2D( u_texture2Sampler, texCoord ).r;

    vec2 advectedPosition = gl_TexCoord[0].st - vec2( vec.x / u_texture2SizeX, vec.y / u_texture2SizeY );
    vec2 advectedPositionNeg = gl_TexCoord[0].st + vec2( vec.x / u_texture2SizeX, vec.y / u_texture2SizeY );


    float advectedNoise = texture2D( u_texture2Sampler, advectedPosition ).r;
    float advectedNoiseNeg = texture2D( u_texture2Sampler, advectedPositionNeg ).r;
    
    float n = ( advectedNoise  );
    if ( depth > 0.99 )
    {
        n = noise;
    }

    // finally, blend noise and old noise
    gl_FragColor = vec4( ( n * ( 1.0 - u_noiseRatio ) ) + ( noise * u_noiseRatio ) );
}

