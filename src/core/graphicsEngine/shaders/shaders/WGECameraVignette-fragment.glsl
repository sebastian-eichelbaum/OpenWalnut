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

/**
 * Intensity of effect
 */
uniform float u_vignetteIntensity = 0.5;

/**
 * Pixel position in [0,1]
 */
varying vec2 v_pos;

void main()
{
    // Basically, use a unit circle and shade alpha when nearing radius 1
    vec2 posInUnitCircle = 2.0 * ( v_pos - 0.5 );
    float xsqr = posInUnitCircle.x * posInUnitCircle.x;
    float ysqr = posInUnitCircle.y * posInUnitCircle.y;

    // intensity of the vignette
    float alpha = u_vignetteIntensity * ( xsqr * ysqr );

    // use texture coordinate to mix along the cylinder
    gl_FragColor = vec4( vec3( 0.0 ), alpha );
}
