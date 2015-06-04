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

///////////////////////////////////////////////////////////////////////////////////////////
// IMPORTANT:
//  Due to the fact that the graphics hardware limits the number of varying floats and the
//  fact that NVIDIA stores vec3 varyings as vec4 and mat3 as mat4 we get a lot of unused
//  floats. The most varying variables here just need three components but as said before
//  the hardware stores 4 floats so we use the 4th. float to transfer further data. By doing
//  this we can delay the problem of having to much variables and to less space to store
//  them.
//
//  To increase clearness there are USAGE comments on all varying variables that explain
//  for which data the unused component is used (to avoid unnesesary confusion).
//
///////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////
// 1: varyings
/////////////////////////////////////////////////////////////////////////////////////////////

// light direction
// USAGE:
// x,y,z components:        the light direction vector
// w component:             unused
// (4 varying floats)
varying vec4 v_lightDir;

// camera direction vector
// USAGE:
// x,y,z components:        the direction vector
// w component:             unused
varying vec4 v_planePoint;

// point on projection plane of current pixel
// USAGE:
// x,y,z components:        the point
// w component:             unused
varying vec4 v_viewDir;

// alpha and beta values describing the superquadric
// USAGE:
// x component:             2.0/alpha
// y component:             2.0/beta
// z component:             alpha/beta
// w component:             is !=0 when the glyph has to be dropped
// (4 varying floats)
varying vec4 v_alphaBeta;

/**
 * The scaling component of the modelview matrix.
 */
varying float v_worldScale;

varying mat4 v_glyphToWorld;

/////////////////////////////////////////////////////////////////////////////////////////////
// 2: uniforms
/////////////////////////////////////////////////////////////////////////////////////////////

// scale glyphs
uniform float u_scaling;

// fractional anisotropy threshold to skip some glyphs
uniform float u_faThreshold;

// eigenvector threshold
uniform float u_evThreshold;

// sharpnes parameter
uniform float u_gamma;

