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

#version 130

#include "WGEColormapping-vertex.glsl"

/**
 * The normal.
 */
varying vec3 v_normal;

/**
 * This matrix transform the vertex in openwalnut space.
 */
uniform mat4 u_colorMapTransformation;

void main()
{
#ifdef COLORMAPPING_ENABLED
    // prepare colormapping
    colormapping( u_colorMapTransformation );
#endif

    // get normal
    v_normal = gl_NormalMatrix * gl_Normal;

    // apply standard pipeline
    gl_FrontColor = gl_Color;
    gl_Position = ftransform();
}
