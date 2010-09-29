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

varying vec4 VaryingTexCoord0;
varying vec4 VaryingTexCoord1;
varying vec4 VaryingTexCoord2;
varying vec4 VaryingTexCoord3;
varying vec4 VaryingTexCoord4;
varying vec4 VaryingTexCoord5;
varying vec4 VaryingTexCoord6;
varying vec4 VaryingTexCoord7;
varying vec4 VaryingTexCoord8;
varying vec4 VaryingTexCoord9;

uniform bool useLighting;
uniform bool useTexture;
uniform int opacity;

uniform sampler3D tex0;
uniform sampler3D tex1;
uniform sampler3D tex2;
uniform sampler3D tex3;
uniform sampler3D tex4;
uniform sampler3D tex5;
uniform sampler3D tex6;
uniform sampler3D tex7;
uniform sampler3D tex8;
uniform sampler3D tex9;

uniform float threshold0;
uniform float threshold1;
uniform float threshold2;
uniform float threshold3;
uniform float threshold4;
uniform float threshold5;
uniform float threshold6;
uniform float threshold7;
uniform float threshold8;
uniform float threshold9;


uniform int type0;
uniform int type1;
uniform int type2;
uniform int type3;
uniform int type4;
uniform int type5;
uniform int type6;
uniform int type7;
uniform int type8;
uniform int type9;

uniform float alpha0;
uniform float alpha1;
uniform float alpha2;
uniform float alpha3;
uniform float alpha4;
uniform float alpha5;
uniform float alpha6;
uniform float alpha7;
uniform float alpha8;
uniform float alpha9;

uniform int useCmap0;
uniform int useCmap1;
uniform int useCmap2;
uniform int useCmap3;
uniform int useCmap4;
uniform int useCmap5;
uniform int useCmap6;
uniform int useCmap7;
uniform int useCmap8;
uniform int useCmap9;


#include "WGEColorMaps.glsl"
#include "WGELighting-fragment.glsl"

void lookupTex( inout vec4 col, in int type, in sampler3D tex,  in float threshold, in vec3 v, in float alpha, in int cmap )
{
    vec3 col1 = vec3( 0.0 );

    col1 = clamp( texture3D( tex, v ).rgb, 0.0, 1.0 );

    if( ( col1.r + col1.g + col1.b ) / 3.0  - threshold <= 0.0 ) return;

    if( cmap != 0 )
    {
        if ( threshold < 1.0 )
        {
            col1.r = ( col1.r - threshold ) / ( 1.0 - threshold );
        }

        colorMap( col1, col1.r, cmap );
    }

    col.rgb = mix( col.rgb, col1.rgb, alpha );
}

void main()
{
    vec4 col = gl_Color;

    vec4 ambient = vec4( 0.0 );
    vec4 diffuse = vec4( 0.0 );
    vec4 specular = vec4( 0.0 );

    if ( useLighting )
        calculateLighting( -normal, gl_FrontMaterial.shininess, ambient, diffuse, specular );

    if( useTexture )
    {
        if ( type9 > 0 ) lookupTex( col, type9, tex9, threshold9, VaryingTexCoord9.xyz, alpha9, useCmap9 );
        if ( type8 > 0 ) lookupTex( col, type8, tex8, threshold8, VaryingTexCoord8.xyz, alpha8, useCmap8 );
        if ( type7 > 0 ) lookupTex( col, type7, tex7, threshold7, VaryingTexCoord7.xyz, alpha7, useCmap7 );
        if ( type6 > 0 ) lookupTex( col, type6, tex6, threshold6, VaryingTexCoord6.xyz, alpha6, useCmap6 );
        if ( type5 > 0 ) lookupTex( col, type5, tex5, threshold5, VaryingTexCoord5.xyz, alpha5, useCmap5 );
        if ( type4 > 0 ) lookupTex( col, type4, tex4, threshold4, VaryingTexCoord4.xyz, alpha4, useCmap4 );
        if ( type3 > 0 ) lookupTex( col, type3, tex3, threshold3, VaryingTexCoord3.xyz, alpha3, useCmap3 );
        if ( type2 > 0 ) lookupTex( col, type2, tex2, threshold2, VaryingTexCoord2.xyz, alpha2, useCmap2 );
        if ( type1 > 0 ) lookupTex( col, type1, tex1, threshold1, VaryingTexCoord1.xyz, alpha1, useCmap1 );
        if ( type0 > 0 ) lookupTex( col, type0, tex0, threshold0, VaryingTexCoord0.xyz, alpha0, useCmap0 );
    }
    if ( useLighting )
    {
        col = ( ambient * col / 2.0 ) + ( diffuse * col ) + ( specular * col / 2.0 );
    }
    col = vec4( 0.5, 0.5, 0.5, 1.0 );
    col = clamp( col, 0.0, 1.0 );
    float fa = clamp( ( gl_Color.b - 0.1 ) * 3., 0., 1. );
    float licBlend = ( 1.0 - gl_Color.g ) * fa;
    vec4 tempColor = vec4( gl_Color.r, gl_Color.r, gl_Color.r, ( 1.0 - gl_Color.g ) );
    vec4 licColor = clamp( tempColor * 1.8 - vec4( 0.4 ), 0., 1. );
    float noiseBlend = clamp( ( gl_Color.g - 0.6 ), 0., 1. ) * clamp( ( fa - 0.2 ), 0., 1. ) * 3.;
    vec4 noiseColor = vec4( gl_Color.a );

    gl_FragColor = ( ( noiseColor - vec4( 0.5 ) ) * col * noiseBlend ) + ( ( licColor - vec4( 0.5 ) ) * col * licBlend ) + col;
}
