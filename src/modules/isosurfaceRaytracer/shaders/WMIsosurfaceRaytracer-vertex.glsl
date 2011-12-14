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

#include "WGEColormapping-vertex.glsl"

#include "WGETransformationTools.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "WMIsosurfaceRaytracer-varyings.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// scaling factor of the data in the texture. Needed for descaling.
uniform float u_texture0Scale;

// minimum value in texture. Needed for descaling.
uniform float u_texture0Min;

// The isovalue to use.
uniform float u_isovalue;

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
    colormapping();

    // scale isovalue to equal the texture data scaling.
    v_isovalue = ( u_isovalue - u_texture0Min ) / u_texture0Scale;

    // for easy access to texture coordinates
    gl_TexCoord[0] = gl_MultiTexCoord0;
    v_normal = gl_Normal;

    // in texture space, the starting point simply is the current surface point in texture space
    v_rayStart = gl_TexCoord[0].xyz; // this equals gl_Vertex!

    // transform the ray direction to texture space, which equals object space
    // Therefore use two points, as we transform a vector
    vec4 camLookAt = vec4( 0.0, 0.0, -1.0, 0.0 );
    v_ray = worldToLocal( camLookAt ).xyz;

#ifdef WGE_POSTPROCESSING_ENABLED
    // Keep track of scaling information. This is needed by some postprocessors.
    // TODO(ebaum): I do not yet understand this factor! Fix this.
    v_worldScale = 0.015 * getModelViewScale();
#endif

    // Simply project the vertex
    gl_Position = ftransform();
    gl_FrontColor = gl_Color;
}

