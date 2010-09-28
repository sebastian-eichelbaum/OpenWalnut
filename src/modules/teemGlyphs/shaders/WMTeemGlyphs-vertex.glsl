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

#include "WGELighting-vertex.glsl"

#include "WGETransformationTools.glsl"
// The ray's starting point in texture space
varying vec3 v_rayStart;

// The ray direction in texture space
varying vec3 v_ray;

// the Surface normal at this point
varying vec3 v_normal;

// The light source in local coordinates
varying vec3 v_lightSource;

void main()
{
    prepareLight();
    v_normal = gl_Normal;

    // transform the ray direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 1.0 );
    vec4 camPos    = vec4( 0.0, 0.0, 0.0, 1.0 );
    v_ray = worldToLocal( camLookAt, camPos ).xyz;

    // also get the coordinates of the light
    vec4 lpos = gl_LightSource[0].position;
    lpos = vec4( 0.0, 0.0, 1000.0, 1.0 );
    v_lightSource = worldToLocal( lpos ).xyz;
    gl_FrontColor = gl_Color;

    gl_Position = ftransform();
}
