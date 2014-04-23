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

#version 120

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
/**
 * Chooses the type of rendering that should be performed within this shader
 */
uniform int u_renderingType;

#include "WGEShadingTools.glsl"

void main()
{
    vec4 col = gl_Color;
    //col.rgb = vec3( .7);
    float widthHalf = u_parameterWidth / 2.0;
    vec2 param = col.rg;

    // calculate lighting
    float light = blinnPhongIlluminationIntensity( normalize( -v_normal ) );
    col *= light;

    // finally, apply opacity
    col.a = u_opacity* 0.01;

    vec3 normal = normalize( v_normal );
    float dotNorm = dot( vec3( 0.0, 0.0, 1.0 ), normal );
    dotNorm *= sign( dotNorm );

    if( u_renderingType == 1 )
    {
        // Slab
        if( verpos.x > u_parameterCenter + widthHalf || verpos.x < u_parameterCenter - widthHalf ) discard;
    }
    else if( u_renderingType == 2 )
    {
        // Streamribbon
        if( param.s > u_parameterCenter + widthHalf || param.s < u_parameterCenter - widthHalf ) discard;
    }
    else if( u_renderingType == 3 )
    {
        // Timeribbon
        if( param.t > u_parameterCenter + widthHalf || param.t < u_parameterCenter - widthHalf) discard;
    }
    else if( u_renderingType == 4 )
    {
        // View-dependent transparency
        col.a = 1 - dotNorm * dotNorm;
    }
    else if( u_renderingType == 5 )
    {
        // Unnatural opposite effect
        col.a = dotNorm* dotNorm;
    }


    gl_FragColor = col;
}

