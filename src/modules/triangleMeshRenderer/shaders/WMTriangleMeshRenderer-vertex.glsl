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

/**
 * The normal.
 */
varying vec3 v_normal;

/**
 * This matrix transform the vertex in openwalnut space.
 */
uniform mat4 u_colorMapTransformation;

// modelview matrix' scaling factor
varying float v_worldScale;

void main()
{
#ifdef COLORMAPPING_ENABLED
    // prepare colormapping
    colormapping( u_colorMapTransformation );
#endif

    // prepare light
    v_normal = gl_NormalMatrix * gl_Normal;

    // Calc the scaling factor in the MV matrix
    v_worldScale = getModelViewScale();

    gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}
