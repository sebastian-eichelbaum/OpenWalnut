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

#include "WGETextureTools.glsl"
#include "WGEShadingTools.glsl"

// The surface normal
varying vec3 v_normal;

// modelview matrix' scaling factor
varying float v_worldScale;

void main()
{
    vec4 col = gl_Color;

    // do light
    float light = blinnPhongIlluminationIntensity( wge_DefaultLightIntensityFullDiffuse, normalize( viewAlign( v_normal ) ) );

    // opacity of the surface
    col.rgb = light * col.rgb;

    // finally set the color and depth
    gl_FragColor = col;
}


