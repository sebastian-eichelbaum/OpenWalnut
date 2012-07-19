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

uniform sampler3D u_scalarDataSampler;

uniform int u_texture0SizeX;

uniform float u_isovalue;

void main()
{
    vec4 col = vec4( texture3D( u_scalarDataSampler, gl_TexCoord[0].xyz ).xyz, 1.0 );

    float s = 0.001;
    vec3 curPoint = gl_TexCoord[0].xyz;

    float valueX1    = texture3D( u_scalarDataSampler, curPoint + vec3(   s, 0.0, 0.0 ) ).r;
    float valueX2    = texture3D( u_scalarDataSampler, curPoint - vec3(   s, 0.0, 0.0 ) ).r;
    float valueXY1   = texture3D( u_scalarDataSampler, curPoint + vec3(   s,   s, 0.0 ) ).r;
    float valueXY2   = texture3D( u_scalarDataSampler, curPoint - vec3(   s,   s, 0.0 ) ).r;
    float valueXY3   = texture3D( u_scalarDataSampler, curPoint + vec3(   s,  -s, 0.0 ) ).r;
    float valueXY4   = texture3D( u_scalarDataSampler, curPoint - vec3(   s,  -s, 0.0 ) ).r;
    float valueY1    = texture3D( u_scalarDataSampler, curPoint + vec3( 0.0,   s, 0.0 ) ).r;
    float valueY2    = texture3D( u_scalarDataSampler, curPoint - vec3( 0.0,   s, 0.0 ) ).r;
    float valueZ1X1  = texture3D( u_scalarDataSampler, curPoint + vec3(   s, 0.0,   s ) ).r;
    float valueZ1X2  = texture3D( u_scalarDataSampler, curPoint - vec3(   s, 0.0,   s ) ).r;
    float valueZ1XY1 = texture3D( u_scalarDataSampler, curPoint + vec3(   s,   s,   s ) ).r;
    float valueZ1XY2 = texture3D( u_scalarDataSampler, curPoint - vec3(   s,   s,   s ) ).r;
    float valueZ1XY3 = texture3D( u_scalarDataSampler, curPoint + vec3(   s,  -s,   s ) ).r;
    float valueZ1XY4 = texture3D( u_scalarDataSampler, curPoint - vec3(   s,  -s,   s ) ).r;
    float valueZ1Y1  = texture3D( u_scalarDataSampler, curPoint + vec3( 0.0,   s,   s ) ).r;
    float valueZ1Y2  = texture3D( u_scalarDataSampler, curPoint - vec3( 0.0,   s,   s ) ).r;
    float valueZ2X1  = texture3D( u_scalarDataSampler, curPoint + vec3(   s, 0.0,  -s ) ).r;
    float valueZ2X2  = texture3D( u_scalarDataSampler, curPoint - vec3(   s, 0.0,  -s ) ).r;
    float valueZ2XY1 = texture3D( u_scalarDataSampler, curPoint + vec3(   s,   s,  -s ) ).r;
    float valueZ2XY2 = texture3D( u_scalarDataSampler, curPoint - vec3(   s,   s,  -s ) ).r;
    float valueZ2XY3 = texture3D( u_scalarDataSampler, curPoint + vec3(   s,  -s,  -s ) ).r;
    float valueZ2XY4 = texture3D( u_scalarDataSampler, curPoint - vec3(   s,  -s,  -s ) ).r;
    float valueZ2Y1  = texture3D( u_scalarDataSampler, curPoint + vec3( 0.0,   s,  -s ) ).r;
    float valueZ2Y2  = texture3D( u_scalarDataSampler, curPoint - vec3( 0.0,   s,  -s ) ).r;

    int u1  = int( valueX1    < u_isovalue );
    int u2  = int( valueX2    < u_isovalue );
    int u3  = int( valueXY1   < u_isovalue );
    int u4  = int( valueXY2   < u_isovalue );
    int u5  = int( valueXY3   < u_isovalue );
    int u6  = int( valueXY4   < u_isovalue );
    int u7  = int( valueY1    < u_isovalue );
    int u8  = int( valueY2    < u_isovalue );
    int u9  = int( valueZ1X1  < u_isovalue );
    int u10 = int( valueZ1X2  < u_isovalue );
    int u11 = int( valueZ1XY1 < u_isovalue );
    int u12 = int( valueZ1XY2 < u_isovalue );
    int u13 = int( valueZ1XY3 < u_isovalue );
    int u14 = int( valueZ1XY4 < u_isovalue );
    int u15 = int( valueZ1Y1  < u_isovalue );
    int u16 = int( valueZ1Y2  < u_isovalue );
    int u17 = int( valueZ2X1  < u_isovalue );
    int u18 = int( valueZ2X2  < u_isovalue );
    int u19 = int( valueZ2XY1 < u_isovalue );
    int u20 = int( valueZ2XY2 < u_isovalue );
    int u21 = int( valueZ2XY3 < u_isovalue );
    int u22 = int( valueZ2XY4 < u_isovalue );
    int u23 = int( valueZ2Y1  < u_isovalue );
    int u24 = int( valueZ2Y2  < u_isovalue );

    int sum = u1 + u2 + u3 + u4 + u5 + u6 + u7 + u8 + u9 +
              u10 + u11 + u12 + u13 + u14 + u15 + u16 + u17 + u18 + u19 +
              u20 + u21 + u22 + u23 + u24;

    // BOTH LAYERS
    float eps = 0.05; // * length( grad );

//    if( abs( col.r - u_isovalue ) > eps )
    if( sum >= 22 || sum <= 2 )
    {
        discard;
    }

    gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 );
}
