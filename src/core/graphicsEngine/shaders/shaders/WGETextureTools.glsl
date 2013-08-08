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

#ifndef WGETEXTURETOOLS_GLSL
#define WGETEXTURETOOLS_GLSL

#version 130

#include "WGEUtils.glsl"

/**
 * Grabs and unscales the value inside the texture and returns it.
 *
 * \param texture the texture unit to use
 * \param point   the texture coordinates
 * \param minimum the minumum value of all values inside the texture
 * \param scale   the scaling value for all values inside the texture
 *
 * \note The minimum and scale values are normally transferred to the shader using uniforms, as the CPU scales the textures
 *
 * \return the value at the given point
 */
vec4 texture3DUnscaled( sampler3D texture, vec3 point, float minimum, float scale )
{
    return ( scale * texture3D( texture, point ) ) + vec4( minimum );
}

/**
 * Grabs and unscales the value inside the texture and returns it.
 *
 * \param texture the texture unit to use
 * \param point   the texture coordinates
 * \param minimum the minumum value of all values inside the texture
 * \param scale   the scaling value for all values inside the texture
 *
 * \note The minimum and scale values are normally transferred to the shader using uniforms, as the CPU scales the textures
 *
 * \return the value at the given point
 */
vec4 texture2DUnscaled( sampler2D texture, vec2 point, float minimum, float scale )
{
    return ( scale * texture2D( texture, point ) ) + vec4( minimum );
}

/**
 * Grabs and unscales the value inside the texture and returns it.
 *
 * \param texture the texture unit to use
 * \param point   the texture coordinates
 * \param minimum the minumum value of all values inside the texture
 * \param scale   the scaling value for all values inside the texture
 *
 * \note The minimum and scale values are normally transferred to the shader using uniforms, as the CPU scales the textures
 *
 * \return the value at the given point
 */
vec4 texture1DUnscaled( sampler1D texture, float point, float minimum, float scale )
{
    return ( scale * texture1D( texture, point ) ) + vec4( minimum );
}

/**
 * This method normalizes a given point/vector in a special way. It scales it so that the largest component is exactly 1.
 * This way the other components can profit from the whole precision in textures (where all values are clamped if >1).
 *
 * \param point the point to scale
 *
 * \return the scaled point, where max( { point.x, point.y, point.z, point.w } ) = 1
 */
vec4 textureNormalize( vec4 point )
{
    return 0.5 + ( 0.5 * scaleMaxToOne( point ) );
}

/**
 * This method normalizes a given point/vector in a special way. It scales it so that the largest component is exactly 1.
 * This way the other components can profit from the whole precision in textures (where all values are clamped if >1).
 *
 * \param point the point to scale
 *
 * \return the scaled point, where max( { point.x, point.y, point.z } ) = 1
 */
vec3 textureNormalize( vec3 point )
{
    return 0.5 + ( 0.5 * scaleMaxToOne( point ) );
}

/**
 * This method normalizes a given point/vector in a special way. It scales it so that the largest component is exactly 1.
 * This way the other components can profit from the whole precision in textures (where all values are clamped if >1).
 *
 * \param point the point to scale
 *
 * \return the scaled point, where max( { point.x, point.y, point.z } ) = 1
 */
vec2 textureNormalize( vec2 point )
{
    return 0.5 + ( 0.5 * scaleMaxToOne( point ) );
}

/**
 * Returns the discrete voxel coordinate in texture space. This is useful to get the nearest pixel/voxel for a given coordinate in [0,1].
 *
 * \param vSize the normalized size of the texture (usually this is a vec3( 1.0 / sizeX, 1.0 / sizeY, 1.0 / sizeZ ).
 * \param coord the coordinate to get the discrete coordinate for
 *
 * \return the coordinate
 */
vec3 getDiscreteVoxelCoordinate( in vec3 vSize, in vec3 coord )
{
    coord.x -= mod( coord.x, vSize.x );
    coord.y -= mod( coord.y, vSize.y );
    coord.z -= mod( coord.z, vSize.z );

    coord.x += 0.5 * vSize.x;
    coord.y += 0.5 * vSize.y;
    coord.z += 0.5 * vSize.z;

    return coord;
}

/**
 * Get the discrete coordinate of a neighbour coordinate relative to the given one. Useful for manual implementations of interpolation
 * algorithms.
 *
 * \param vSize the normalized size of the texture (usually this is a vec3( 1.0 / sizeX, 1.0 / sizeY, 1.0 / sizeZ ).
 * \param coord the coordinate to which the neighbour is relative
 * \param direction the direction. vec3( 1.0, 0.0, 0.0 ) returns the right neighbour.
 *
 * \return the neighbour coordinate.
 */
vec3 getNeighbourVoxel( in vec3 vSize, in vec3 coord, in vec3 direction )
{
    vec3 me = getDiscreteVoxelCoordinate( vSize, coord );
    return me + vec3( direction.x * vSize.x,
                      direction.y * vSize.y,
                      direction.z * vSize.z
                    );
}

#endif // WGETEXTURETOOLS_GLSL

