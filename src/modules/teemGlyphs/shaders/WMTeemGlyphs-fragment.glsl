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

#include "WGEColorMaps.glsl"
#include "WGELighting-fragment.glsl"
#include "WGEShadingTools.glsl"

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
    vec4 col;

    float light = blinnPhongIlluminationIntensity(
        0.1,                                // material ambient
        0.75,                               // material diffuse
        1.3,                                // material specular
        10.0,                               // shinines
        1.0,                                // light diffuse
        0.75,                               // light ambient
        normalize( v_normal ),              // normal
        normalize( v_ray ),                 // view direction
        normalize( v_lightSource )          // light source position
        );

    col = light * gl_Color;
    col.a = 1.0;
    gl_FragColor = col;
}
