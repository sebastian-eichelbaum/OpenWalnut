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

varying vec4 verpos;

/**
 * The normal.
 */
varying vec3 v_normal;

/**
 * The opacity specified by the user in [0,100]
 */
uniform float u_opacity;

/**
 * The center of the parameter interval to be selected
 */
uniform float u_parameterCenter;
/**
 * The width of the parameter interval to be selected
 */
uniform float u_parameterWidth;

#include "WGEShadingTools.glsl"

void main()
{
    vec4 col = gl_Color;
    float widthHalf = u_parameterWidth / 2.0;
    //if( col.r > u_parameterCenter + widthHalf || col.r < u_parameterCenter - widthHalf ) discard;
    //if( col.g > u_parameterCenter + widthHalf || col.g < u_parameterCenter - widthHalf) discard;
    if( verpos.x > u_parameterCenter + widthHalf || verpos.x < u_parameterCenter - widthHalf ) discard;

    // calculate lighting
    float light = blinnPhongIlluminationIntensity( normalize( -v_normal ) );
    col*=light;

    // finally, apply opacity
    col.a = u_opacity* 0.01;

    vec3 normal = normalize( v_normal );
    float dotNorm = dot( vec3( 0.0, 0.0, 1.0 ), normal );
    dotNorm *= sign( dotNorm );
    //col.a=1-dotNorm*dotNorm;
    gl_FragColor = col;
}

