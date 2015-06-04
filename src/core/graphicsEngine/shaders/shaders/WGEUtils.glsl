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

/**
 * Computes the distance from a line segment and a point.
 *
 * \param start Definig the start of the line segment.
 * \param end Defining the end of the line segment.
 * \param point For which the distance should be computed.
 *
 * \return Distance from given line segment (start,end) and point.
 */
float distancePointLineSegment( vec3 point, vec3 start, vec3 end )
{
    float segmentLength = length( start - end );
    if( segmentLength == 0.0 )
    {
        return distance( point, start );
    }

    // Consider the line extending the segment, parameterized as start + t (end - start):
    // We find projection of point p onto the line: t = [(p-v) . (w-v)] / |w-v|^2
    float t = dot( point - start, end - start ) / ( segmentLength * segmentLength );

    if( t < 0.0 )      // Beyond the 'v' end of the segment
    {
        return distance( point, start );
    }
    else if( t > 1.0 ) // Beyond the 'w' end of the segment
    {
        return distance( point, end );
    }
    else
    {
        vec3 projection = start + t * ( end - start );  // Projection falls on the segment
        return distance( point, projection );
    }
}

#endif  // WGEUTILS_GLSL

