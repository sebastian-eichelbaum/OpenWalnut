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

// Bottom Gradient
uniform vec4 u_bottom1Color = vec4( 0.65 );
uniform vec4 u_bottom2Color = vec4( 1.0 );

// Top Gradient
uniform vec4 u_top1Color = vec4( 1.0 );
uniform vec4 u_top2Color = vec4( 0.95 );

// Where to split between bottom and top
uniform float u_horizon = 33;

/**
 * Pixel position in [0,1]
 */
varying vec2 v_pos;

void main()
{
#ifdef WGE_PLAIN_COLOR
    discard;
#else
    float horizonLine = u_horizon / 100.0;
    // is 1 when below horizon line
    float bottom = float( v_pos.y <= horizonLine );
    float horizon = horizonLine + 0.0001; // NOTE: this prevents div by 0

    float topScale = ( v_pos.y - horizon ) / ( 1.0 - horizon );

    vec4 color = bottom           * mix( u_bottom1Color, u_bottom2Color, v_pos.y / horizon ) +
                 ( 1.0 - bottom ) * mix( u_top1Color, u_top2Color, topScale );

    // use texture coordinate to mix along the cylinder
    gl_FragColor = vec4( color.rgb, 1.0 );
    // we disabled depth write in WGEViewerEffect.cpp, but it does not seem to work ...
    gl_FragDepth = 0.9999999;
#endif
}
