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

uniform sampler3D u_texture0Sampler;

uniform int u_texture0SizeX;

uniform float u_grayMatter;
uniform float u_whiteMatter;

uniform vec4 u_gmColor;
uniform vec4 u_wmColor;

void main()
{
    vec4 col = clamp( texture3D( u_texture0Sampler, gl_TexCoord[0].xyz ), 0.0, 1.0 );

    vec3 grad;
    {
        float s = 0.01;
        vec3 curPoint = gl_TexCoord[0].xyz;
        float valueXP = texture3D( u_texture0Sampler, curPoint + vec3( s, 0.0, 0.0 ) ).r;
        float valueXM = texture3D( u_texture0Sampler, curPoint - vec3( s, 0.0, 0.0 ) ).r;
        float valueYP = texture3D( u_texture0Sampler, curPoint + vec3( 0.0, s, 0.0 ) ).r;
        float valueYM = texture3D( u_texture0Sampler, curPoint - vec3( 0.0, s, 0.0 ) ).r;
        float valueZP = texture3D( u_texture0Sampler, curPoint + vec3( 0.0, 0.0, s ) ).r;
        float valueZM = texture3D( u_texture0Sampler, curPoint - vec3( 0.0, 0.0, s ) ).r;

        grad = vec3( valueXP - valueXM, valueYP - valueYM, valueZP - valueZM ); //v_ray;
    }

    // BOTH LAYERS
    float eps = 0.05 * length( grad );

    if( abs( col.r - u_grayMatter ) > eps && abs( col.r - u_whiteMatter ) > eps )
    {
        discard;
    }
    if( abs( col.r - u_grayMatter ) < eps )
    {
        col = u_gmColor;
    }
    else // if( abs( col.r - u_whiteMatter ) < eps )
    {
        col = u_wmColor;
    }

    gl_FragColor = col;
}
