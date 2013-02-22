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

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#ifdef CLIPPLANE_ENABLED
/**
 * The distance to the plane
 */
varying float v_dist;
#endif

/**
 * The surface normal. Needed for nice lighting.
 */
varying vec3 v_normal;

/**
 * The line tangent.
 */
varying vec3 v_tangent;

/**
 * The normal parameterizing the surface in orthogonal tangent direction.
 */
varying vec3 v_biNormal;

/**
 * The actual, corrected vertex.
 */
varying vec4 v_vertex;

/**
 * The diameter of the tube in world-space.
 */
varying float v_diameter;

/**
 * This is the interpolated surface parameter describing the surface orthogonal to the tangent. 0 is the center of the strip and -1 and 1 the
 * borders.
 */
varying float v_surfaceParam;

/**
 * The scaling component of the modelview matrix.
 */
varying float v_worldScale;

#ifdef CLUSTER_FILTER_ENABLED
/**
 * This varying carries the current cluster color.
 */
varying vec3 v_clusterColor;
#endif

/**
 * Varying defines wether to discard the fragments or not.
 */
varying float v_discard;
