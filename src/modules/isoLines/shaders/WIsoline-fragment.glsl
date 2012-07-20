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
    float s = 1.0 / 160.0;
    vec3 curPoint = gl_TexCoord[0].xyz;
    vec3 v1 = vec3(  -s,  -s, 0.0 );
    vec3 v2 = vec3( 0.0,  -s, 0.0 );
    vec3 v3 = vec3(   s,  -s, 0.0 );
    vec3 v4 = vec3(  -s, 0.0, 0.0 );
    vec3 v5 = vec3( 0.0, 0.0, 0.0 );
    vec3 v6 = vec3(   s, 0.0, 0.0 );
    vec3 v7 = vec3(  -s,   s, 0.0 );
    vec3 v8 = vec3( 0.0,   s, 0.0 );
    vec3 v9 = vec3(   s,   s, 0.0 );

    float d1  = texture3D( u_scalarDataSampler, curPoint + v1 ).r;
    float d2  = texture3D( u_scalarDataSampler, curPoint + v2 ).r;
    float d3  = texture3D( u_scalarDataSampler, curPoint + v3 ).r;
    float d4  = texture3D( u_scalarDataSampler, curPoint + v4 ).r;
    float d5  = texture3D( u_scalarDataSampler, curPoint + v5 ).r;
    float d6  = texture3D( u_scalarDataSampler, curPoint + v6 ).r;
    float d7  = texture3D( u_scalarDataSampler, curPoint + v7 ).r;
    float d8  = texture3D( u_scalarDataSampler, curPoint + v8 ).r;
    float d9  = texture3D( u_scalarDataSampler, curPoint + v9 ).r;

    float max_flt = 9.9e+20;
    vec3 infPoint = vec3( max_flt, max_flt, max_flt );

    // we may use the step(..) function here, but then the expressions wouldn't be so clear
    int isoHitEdge1  = int( u_isovalue <= d1 && u_isovalue >= d2 || u_isovalue >= d1 && u_isovalue <= d2 );
    int isoHitEdge2  = int( u_isovalue <= d2 && u_isovalue >= d3 || u_isovalue >= d2 && u_isovalue <= d3 );
    int isoHitEdge3  = int( u_isovalue <= d1 && u_isovalue >= d4 || u_isovalue >= d1 && u_isovalue <= d4 );
    int isoHitEdge4  = int( u_isovalue <= d2 && u_isovalue >= d5 || u_isovalue >= d2 && u_isovalue <= d5 );
    int isoHitEdge5  = int( u_isovalue <= d3 && u_isovalue >= d6 || u_isovalue >= d3 && u_isovalue <= d6 );
    int isoHitEdge6  = int( u_isovalue <= d4 && u_isovalue >= d5 || u_isovalue >= d4 && u_isovalue <= d5 );
    int isoHitEdge7  = int( u_isovalue <= d5 && u_isovalue >= d6 || u_isovalue >= d5 && u_isovalue <= d6 );
    int isoHitEdge8  = int( u_isovalue <= d4 && u_isovalue >= d7 || u_isovalue >= d4 && u_isovalue <= d7 );
    int isoHitEdge9  = int( u_isovalue <= d5 && u_isovalue >= d8 || u_isovalue >= d5 && u_isovalue <= d8 );
    int isoHitEdge10 = int( u_isovalue <= d6 && u_isovalue >= d9 || u_isovalue >= d6 && u_isovalue <= d9 );
    int isoHitEdge11 = int( u_isovalue <= d7 && u_isovalue >= d8 || u_isovalue >= d7 && u_isovalue <= d8 );
    int isoHitEdge12 = int( u_isovalue <= d8 && u_isovalue >= d9 || u_isovalue >= d8 && u_isovalue <= d9 );

    vec3 isoPoint1  = infPoint * ( 1 - isoHitEdge1  ) + ( isoHitEdge1  ) * ( curPoint + v1 + clamp( abs( d1 - u_isovalue ) / abs( d1 - d2 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint2  = infPoint * ( 1 - isoHitEdge2  ) + ( isoHitEdge2  ) * ( curPoint + v2 + clamp( abs( d2 - u_isovalue ) / abs( d2 - d3 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint3  = infPoint * ( 1 - isoHitEdge3  ) + ( isoHitEdge3  ) * ( curPoint + v1 + clamp( abs( d1 - u_isovalue ) / abs( d1 - d4 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint4  = infPoint * ( 1 - isoHitEdge4  ) + ( isoHitEdge4  ) * ( curPoint + v2 + clamp( abs( d2 - u_isovalue ) / abs( d2 - d5 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint5  = infPoint * ( 1 - isoHitEdge5  ) + ( isoHitEdge5  ) * ( curPoint + v3 + clamp( abs( d3 - u_isovalue ) / abs( d3 - d6 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint6  = infPoint * ( 1 - isoHitEdge6  ) + ( isoHitEdge6  ) * ( curPoint + v4 + clamp( abs( d4 - u_isovalue ) / abs( d4 - d5 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint7  = infPoint * ( 1 - isoHitEdge7  ) + ( isoHitEdge7  ) * ( curPoint + v5 + clamp( abs( d5 - u_isovalue ) / abs( d5 - d6 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint8  = infPoint * ( 1 - isoHitEdge8  ) + ( isoHitEdge8  ) * ( curPoint + v4 + clamp( abs( d4 - u_isovalue ) / abs( d4 - d7 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint9  = infPoint * ( 1 - isoHitEdge9  ) + ( isoHitEdge9  ) * ( curPoint + v5 + clamp( abs( d5 - u_isovalue ) / abs( d5 - d8 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint10 = infPoint * ( 1 - isoHitEdge10 ) + ( isoHitEdge10 ) * ( curPoint + v6 + clamp( abs( d6 - u_isovalue ) / abs( d6 - d9 ), 0.0, 1.0 ) * vec3( 0.0,   s, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint11 = infPoint * ( 1 - isoHitEdge11 ) + ( isoHitEdge11 ) * ( curPoint + v7 + clamp( abs( d7 - u_isovalue ) / abs( d7 - d8 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length
    vec3 isoPoint12 = infPoint * ( 1 - isoHitEdge12 ) + ( isoHitEdge12 ) * ( curPoint + v8 + clamp( abs( d8 - u_isovalue ) / abs( d8 - d9 ), 0.0, 1.0 ) * vec3(   s, 0.0, 0.0 ) ); // NOLINT, line length

    float minDistance = 9.9e+3;
    float distance;
    float newMinimum;

    distance = length( isoPoint1 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint2 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint3 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint4 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint5 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint6 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint7 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint8 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint9 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint10 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint11 - curPoint );
    minDistance = min( distance, minDistance );
    distance = length( isoPoint12 - curPoint );
    minDistance = min( distance, minDistance );

    if( minDistance < s )
    {
        gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );
    }
    else
    {
        discard;
    }
}
