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

#include "WGETransformationTools.glsl"

#include "WMImageSpaceLIC-Transformation-varyings.glsl"

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    // for easy access to texture coordinates
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    // some light precalculations
    v_normal = gl_Normal;

    // also get the coordinates of the light
    vec4 lpos = gl_LightSource[0].position; // this simply doesn't work well with OSG
    lpos = vec4( 0.0, 0.0, 1000.0, 1.0 );
    v_lightSource = worldToLocal( lpos ).xyz;

    // transform the view direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camPos    = vec4( 0.0, 0.0, -1.0, 0.0 );
    v_viewDir = worldToLocal( camPos ).xyz;

    // transform position
    gl_Position = ftransform();

    // TODO(ebaum): make this nice
    VaryingTexCoord0 = gl_TexCoord[0];
    VaryingTexCoord1 = gl_TexCoord[0];
    VaryingTexCoord2 = gl_TexCoord[0];
    VaryingTexCoord3 = gl_TexCoord[0];
    VaryingTexCoord4 = gl_TexCoord[0];
    VaryingTexCoord5 = gl_TexCoord[0];
}

