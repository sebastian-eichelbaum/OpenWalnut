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

#include "WGEColormapping-vertex.glsl"

/**
 * The matrix describes the transformation of gl_Vertex to OpenWalnut Scene Space
 */
uniform mat4 u_WorldTransform;

/**
 * Vertex Main. Simply transforms the geometry. The work is done per fragment.
 */
void main()
{
    // Allow the colormapper to do some precalculations with the real vertex coordinate in ow-scene-space
    colormapping( u_WorldTransform * gl_Vertex );

    // transform position
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}

