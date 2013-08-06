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

#version 130

#include "WGETransformationTools.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMDirectVolumeRendering-varyings.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

// The number of samples to use.
uniform int u_samples;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the vertex shader.
 */
void main()
{
    // for easy access to texture coordinates
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // in texture space, the starting point simply is the current surface point in texture space
    v_rayStart = gl_Vertex.xyz; // this equals gl_Vertex!

    // transform the ray direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 0.0 );
    vec4 camPos    = vec4( 0.0, 0.0, 0.0, 0.0 );
    v_ray = normalize( worldToLocal( camLookAt, camPos ).xyz );

    // to have equidistant sampling for each side of the box, use a fixed step size
    v_sampleDistance = 1.0 / float( u_samples );
    v_relativeSampleDistance = 128.0 /  float( u_samples );

    // Simply project the vertex
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
}

