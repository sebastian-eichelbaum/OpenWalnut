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
uniform sampler3D u_noiseSampler;
// The size in voxel dimensions
uniform int u_noiseSizeX;
uniform int u_noiseSizeY;
uniform int u_noiseSizeZ;

// The surface normal
varying vec3 v_normal;

// Normalized coordinate in the bounding volume of the sphere
varying vec3 v_normalizedVertex;

void main()
{
    vec4 col = gl_Color;

    // Use a utility function to get the gradient for us
    vec3 grad = getGradient( u_noiseSampler, v_normalizedVertex, 1.0 / u_noiseSizeX );

    // do light. Scale by a compile time constant.
    float light = LIGHT_INTENSITY * blinnPhongIlluminationIntensity( normalize( viewAlign( v_normal * grad ) ) );

    // Time based dimming of the color. The constant PI was set on C++-side as compile time constant.
    float dimmer = 0.5 + 0.75 * sin( PI * mod( u_animation, 100.0 ) / 100.0 );

    // Apply light. Pow()? -> it makes it look more "metallic".
    col.rgb = pow( light, 1.5 ) * col.rgb;

    // finally set the color and depth
    gl_FragColor = col;
}

