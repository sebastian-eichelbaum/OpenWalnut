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

#ifndef WGEUTILS_GLSL
#define WGEUTILS_GLSL

#version 120

/**
 * 
 * This makes threshold lying between min and max to lie between 0 and 1 where 0 corresponds to min and 1 to max
 * 
 * \param threshold threshold value used for cutting
 * \param min minumum of original space
 * \param max maximum of original space
 * 
 * \return 
 */
float scaleZeroOne( in float threshold, in float min, in float max )
{
    return ( threshold - min ) / ( max - min );
}

/**
 * Evaluates a given value whether it is zero, or more exactly, if it can be seen as zero.
 * 
 * \param value the value to check
 * \param epsilon the epsilon to use. Default is 0.001. Much lower is not recommended since floating point precision in GLSL is not that high.
 * 
 * \return true if zero.
 */
bool isZero( in float value, in float epsilon )
{
    return ( abs( value ) <= epsilon );
}

/**
 * Evaluates a given value whether it is zero, or more exactly, if it can be seen as zero.
 * \note This version is needed for portability as the GLSL compiler on MacOS X does not allow argument default values in functions.
 *
 * \param value the value to check
 * \param epsilon the epsilon to use. Default is 0.001. Much lower is not recommended since floating point precision in GLSL is not that high.
 * 
 * \return true if zero.
 */
bool isZero( in float value )
{
    return ( abs( value ) <= 0.001 );
}

/**
 * Takes the largest absolute component of the vector and scales the whole vector with it. This ensures that the resulting vector always is in
 * [-1, 1] with its largest component to be -1 or 1.
 *
 * \param point the vector to scale
 *
 * \return the scaled vector
 */
vec2 scaleMaxToOne( vec2 point )
{
    float maxC = max( abs( point.x ), abs( point.y ) ) + 0.00001;
    return point / maxC;
}

/**
 * Takes the largest absolute component of the vector and scales the whole vector with it. This ensures that the resulting vector always is in
 * [-1, 1] with its largest component to be -1 or 1.
 *
 * \param point the vector to scale
 *
 * \return the scaled vector
 */
vec3 scaleMaxToOne( vec3 point )
{
    float maxC = max( abs( point.x ), max( abs( point.y ), abs( point.z ) ) ) + 0.00001;
    return point / maxC;
}

/**
 * Takes the largest absolute component of the vector and scales the whole vector with it. This ensures that the resulting vector always is in
 * [-1, 1] with its largest component to be -1 or 1.
 *
 * \param point the vector to scale
 *
 * \return the scaled vector
 */
vec4 scaleMaxToOne( vec4 point )
{
    float maxC = max( abs( point.x ), max( abs( point.y ), max( abs( point.z ), abs( point.w ) ) ) ) + 0.00001;
    return point / maxC;
}

#endif  // WGEUTILS_GLSL

