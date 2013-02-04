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

#version 130

#include "WGEUtils.glsl"
#include "WGEShadingTools.glsl"

/**
 * The texture Unit for the advection texture
 */
uniform sampler2D u_texture0Sampler;

/**
 * The texture Unit for the edge, depht texture
 */
uniform sampler2D u_texture1Sampler;

/**
 * The texture Unit for the edge, depht texture
 */
uniform sampler2D u_texture2Sampler;

/**
 * Used to en-/disable lighting.
 */
uniform bool u_useLight;

/**
 * Define the light intensity here.
 */
uniform float u_lightIntensity;

/**
 * Used to en-/disable blending of edges.
 */
uniform bool u_useEdges;

/**
 * Edge color to use
 */
uniform vec4 u_useEdgesColor;

/**
 * Step function border for blending in the edges.
 */
uniform float u_useEdgesStep;

/**
 * If true, the depth is blended in
 */
uniform bool u_useDepthCueing;

/**
 * Ratio between colormap and advected noise
 */
uniform float u_cmapRatio;

/**
 * How intensive should contrast enhancement be?
 */
uniform bool u_useHighContrast;

/**
 * Main. Clips and Blends the final image space rendering with the previously acquired 3D information
 */
void main()
{
    vec2 texCoord = gl_TexCoord[0].st;
    float edge  = texture2D( u_texture1Sampler, texCoord ).r * ( u_useEdges ? 1.0 : 0.0 );
    float light  = texture2D( u_texture1Sampler, texCoord ).a * ( u_useLight ? 1.0 : 0.0 );
    float depth  = texture2D( u_texture1Sampler, texCoord ).g;
    float advected  = texture2D( u_texture0Sampler, texCoord ).r;
    vec3 cmap = texture2D( u_texture2Sampler, texCoord ).rgb;

    float u_contrastingS = u_useHighContrast ? 64.0 : 2.5;
    float u_contrastingP = u_useHighContrast ? 8 : 2.5;
    vec3 plainColor = u_cmapRatio * cmap + ( 1.0 - u_cmapRatio ) * vec3( u_contrastingS * pow( advected, u_contrastingP ) );

    vec4 col = vec4(
        mix( plainColor, u_useEdgesColor.xyz, smoothstep( 0, u_useEdgesStep, edge ) ) // plain color mapped advection texture with edges
         * clamp( 1.0 - ( float( u_useDepthCueing ) * depth * depth ), 0.4, 1.0 ) // scaled by depth if enabled
         * ( u_useLight ? light * 1.3 * u_lightIntensity: 1.0 ),    // NOTE: the 1.3 is here to keep compatibility with the old version
        1.0
    );

    gl_FragColor = col;
    gl_FragDepth = depth;
}

