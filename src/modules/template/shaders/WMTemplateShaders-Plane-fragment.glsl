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

// For lighting functionality.
#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"

// This is updated by a callback:
uniform int u_animation;

// These uniforms where set automatically by wge::bindTexture:
uniform sampler2D u_textureSampler;
// The size in voxel dimensions
uniform int u_textureSizeX;
uniform int u_textureSizeY;

// The surface normal
varying vec3 v_normal;

// Normalized coordinate in the bounding volume of the sphere
varying vec3 v_normalizedVertex;

void main()
{
    vec4 col = texture2D( u_textureSampler, v_normalizedVertex.xy );

    // Now handle the different modes whe defined at C++ side
    float light = 1.0;
#ifdef BUMPMAPPING_ENABLED
    // Simplest bump-mapping. Make it look really ... like glibber
    vec2 grad = getGradient( u_textureSampler, v_normalizedVertex.xy, 1.0 / u_textureSizeX * 2 );
    vec3 normal = gl_NormalMatrix * vec3( grad, -1.0 );
    light = blinnPhongIlluminationIntensity( normalize( viewAlign( normal ) ) );
#endif

    float finalAlpha = 1.0;
#ifdef TRANSPARENTPLANE_ENABLED
    finalAlpha = col.a > 0.9 ? 1.0 : 0.5;
#endif

    // Apply light and square it. This ensures max sliminess
    col.rgb = light * light * col.rgb;

#ifdef LSD_ENABLED
    // Time based dimming of the color. u_animation is a tick counter we defined at C++ side.
    float dimmerR = 0.5 + 0.5 * sin( 1 * 3.1416 * mod( u_animation, 50.0 ) / 50.0 );
    float dimmerG = 0.5 + 0.5 * sin( 2 * 3.1416 * mod( u_animation, 50.0 ) / 50.0 );
    float dimmerB = 0.5 + 0.5 * sin( 3 * 3.1416 * mod( u_animation, 50.0 ) / 50.0 );

    col.rgb = vec3( dimmerR * col.r, dimmerG * col.g, dimmerB * col.b );
#endif

    // finally set the color and depth
    gl_FragColor = vec4( mix( gl_Color.rgb, col.rgb, col.a ), finalAlpha );
}

