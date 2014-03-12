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

#version 130

#include "WGEColormapping-fragment.glsl"
#include "WGETextureTools.glsl"
#include "WGEPostprocessing.glsl"
#include "WGEShadingTools.glsl"

/**
 * The normal.
 */
varying vec3 v_normal;

/**
 * The opacity specified by the user in [0,100]
 */
uniform float u_opacity;

/**
 * True if in outline mode
 */
uniform bool u_outline;

/**
 * The colormap ratio specified by the user in [0,1]
 */
uniform float u_colormapRatio;

// modelview matrix' scaling factor
varying float v_worldScale;

void main()
{
    vec4 col = gl_Color;

#ifdef COLORMAPPING_ENABLED
    col = mix( colormapping(), col, u_colormapRatio );
#endif

    // calculate lighting
    float light = blinnPhongIlluminationIntensity( normalize( viewAlign( v_normal ) ) );
    col *= u_outline ? 1.0 : light;
    // apply opacity
    col.a = u_opacity * 0.01;

    // finally set the color and depth
    wgeInitGBuffer();
    wge_FragColor = col;
    wge_FragNormal = textureNormalize( normalize( v_normal ) );
    wge_FragZoom = v_worldScale;
    wge_FragTangent = textureNormalize( v_normal );
}
