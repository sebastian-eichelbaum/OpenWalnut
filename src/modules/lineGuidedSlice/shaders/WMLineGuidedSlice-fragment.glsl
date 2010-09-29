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

uniform sampler3D tex0;
uniform sampler3D tex1;

uniform float threshold0;
uniform float threshold1;

uniform int type0;
uniform int type1;

uniform float alpha0;
uniform float alpha1;

void parameterColorMap( inout vec4 col, in sampler3D tex, in vec3 coords )
{
    vec3 val = vec3( 0.0 );
    val = clamp( texture3D( tex, coords ).rgb, 0.0, 1.0 );
    float value = val.x; // get a float
    clamp( 0.1, 0.9, value );
    value = ( value - 0.1 ) * 0.8; // scale to [0,1]
    col.r = 0.;
    col.g = value;
    col.b = 1.;
    col.a = 1.;
}

void main()
{
    vec4 col = vec4( 0.0, 0.0, 0.0, 1.0 );
    vec4 colFib = vec4( 0.0, 0.0, 0.0, 1.0 );
    vec4 colFA = vec4( 0.0, 0.0, 0.0, 1.0 );

    if( type1 > 0 )
    {
        parameterColorMap( colFA, tex1, VaryingTexCoord1.xyz );
    }
    col = clamp( colFA, 0.0, 1.0 );

    if( texture3D( tex0, VaryingTexCoord0.xyz ).r < 0.2 )
    {
//         if(
//             VaryingTexCoord0.x < 0.001 || VaryingTexCoord0.x > 0.9
//             || VaryingTexCoord0.y < 0.001 || VaryingTexCoord0.y > 0.3
//             || VaryingTexCoord0.z < 0.001 || VaryingTexCoord0.z > 0.9
//             )
            discard;
//         else
//             col =  vec4( 1, 1, 1, 1);
    }
    gl_FragColor = col;
}
