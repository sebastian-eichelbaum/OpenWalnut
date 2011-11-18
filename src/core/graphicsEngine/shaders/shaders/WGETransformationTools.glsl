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

#ifndef WGETRANSFORMATIONTOOLS_GLSL
#define WGETRANSFORMATIONTOOLS_GLSL

#version 120

/**
 * Projects a given point to screen-space, where (0,0) is the lower left corner and (1,1) the upper right. The Depth
 * is stored in the returned vector's Z value.
 *
 * \param point the point to project
 *
 * \return the projected point, the Z value is the depth
 */
vec3 project( vec4 point )
{
    // 1: project
    vec4 pointProjected = gl_ModelViewProjectionMatrix * point;

    // 2: scale to screen space and [0,1]
    pointProjected.x /= pointProjected.w;
    pointProjected.x  = pointProjected.x * 0.5 + 0.5;
    pointProjected.y /= pointProjected.w;
    pointProjected.y  = pointProjected.y * 0.5 + 0.5;
    pointProjected.z /= pointProjected.w;
    pointProjected.z  = pointProjected.z * 0.5 + 0.5;

    return pointProjected.xyz;
}

/**
 * Projects a given vector to screen-space. It does not scale the final vector to [0,1] as project() would have done.
 *
 * \param vector the vector to project, the w component needs to be zero.
 *
 * \return the projected vector in unscaled screen-space ( [-1,1] per component )
 */
vec3 projectVector( vec4 vector )
{
    vec4 vec = vector;
    vec.w = 0.0;    // ensure w is zero

    vec4 vecP = gl_ModelViewProjectionMatrix * vec;

    return vecP.xyz;
}

/**
 * Projects a given vector to screen-space. It does not scale the final vector to [0,1] as project() would have done.
 *
 * \param vector the vector to project
 *
 * \return the projected vector in unscaled screen-space ( [-1,1] per component )
 *
 * \note This assumes the homogeneous coordinate to be 1.
 */
vec3 projectVector( vec3 vector )
{
    return projectVector( vec4( vector, 1.0 ) );
}

/**
 * Projects a given point to screen-space, where (0,0) is the lower left corner and (1,1) the upper right. The Depth
 * is stored in the returned vector's Z value.
 *
 * \param point the point to project
 *
 * \return the projected point, the Z value is the depth
 *
 * \note This projects a vec3 by applying 1.0 to the homogeneous coordinate
 */
vec3 project( vec3 point )
{
    return project( vec4( point, 1.0 ) );
}

/**
 * This function transforms a point which is in world space, to a point in the local coordinate system of the object
 * currently getting rendered.
 * \note you can use this method with point.w == 0.0 for vector projection too.
 *
 * \param point the point in world coordinates
 *
 * \return the point in local coordinates
 */
vec4 worldToLocal( vec4 point )
{
    return gl_ModelViewMatrixInverse * point;
}

/**
 * This function transforms a point which is in world space, to a point in the local coordinate system of the object
 * currently getting rendered.
 *
 * \param point the point in world coordinates
 *
 * \return the point in local coordinates
 *
 * \note This assumes the homogeneous part to be 1.0
 */
vec4 worldToLocal( vec3 point )
{
    return worldToLocal( vec4( point, 1.0 ) );
}

/**
 * This function transforms a vector which is in world space, to a point in the local coordinate system of the object
 * currently getting rendered.
 *
 * \param point1 the vector point in world coordinates
 * \param point2 the vector direction point in world coordinates
 *
 * \return the vector in local coordinates
 */
vec4 worldToLocal( vec4 point1, vec4 point2 )
{
    return worldToLocal( point1 - point2 );
}

/**
 * This function transforms a vector which is in world space, to a point in the local coordinate system of the object
 * currently getting rendered.
 *
 * \param point1 the vector point in world coordinates
 * \param point2 the vector direction point in world coordinates
 *
 * \return the vector in local coordinates
 *
 * \note This assumes the homogeneous part to be 1.0
 */
vec4 worldToLocal( vec3 point1, vec3 point2 )
{
    return worldToLocal( vec4( point1 - point2, 0.0 ) );
}

/**
 * This calculates the scaling factor of the modelview matrix. It transforms a vec4( 1.0, 1.0, 1.0, 0.0 ) vector and returns its length,
 * representing the scaling. This is mostly useful for screen-space based approaches which need some hint about zooming to provide consistent
 * look for all zoom levels.
 *
 * \return ModelView zoom
 */
float getModelViewScale()
{
    return length( ( gl_ModelViewMatrix * normalize( vec4( 1.0, 1.0, 1.0, 0.0 ) ) ).xyz );
}

#endif // WGETRANSFORMATIONTOOLS_GLSL

