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

varying vec4 myColor;
varying vec4 VaryingTexCoord0;

uniform int dimX, dimY, dimZ;

uniform sampler3D tex;
uniform int type;
uniform float threshold;
uniform int cMap;

uniform bool useTexture;
uniform bool useCullBox;
uniform bool insideCullBox;
uniform float cullBoxLBX;
uniform float cullBoxLBY;
uniform float cullBoxLBZ;
uniform float cullBoxUBX;
uniform float cullBoxUBY;
uniform float cullBoxUBZ;


#include "WGEColorMaps.glsl"
#include "WGEUtils.glsl"

float lookupTex()
{
    vec3 v = VaryingTexCoord0.xyz;
    v.x = v.x / ( float( dimX ) );
    v.y = v.y / ( float( dimY ) );
    v.z = v.z / ( float( dimZ ) );

    vec3 col1;
    col1.r = clamp( texture3D( tex, v ).r, 0.0, 1.0 );

    if( col1.r < threshold )
    {
        discard;
    }
    else
        return col1.r;
}

void checkCullBox()
{
    vec3 pos = VaryingTexCoord0.xyz;

    if( insideCullBox )
    {
        if( pos.x < cullBoxLBX || pos.x > cullBoxUBX )
            discard;
        if( pos.y < cullBoxLBY || pos.y > cullBoxUBY )
            discard;
        if( pos.z < cullBoxLBZ || pos.z > cullBoxUBZ )
            discard;
    }
    else
    {
        if( ( pos.x > cullBoxLBX && pos.x < cullBoxUBX )
             && ( pos.y > cullBoxLBY && pos.y < cullBoxUBY )
             && ( pos.z > cullBoxLBZ && pos.z < cullBoxUBZ ) )
            discard;
    }
}

/**
 * simple fragment shader that uses a texture on fibers
 */
void main()
{
    if( useCullBox )
        checkCullBox();

    vec4 color = myColor;

    if( useTexture )
    {
        float value = lookupTex();
        colorMap( color.rgb, value, cMap );
        color.a = 1.0;
    }

    gl_FragColor = color;
}

