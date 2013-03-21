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

#ifndef WMATH_H
#define WMATH_H

#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "WLine.h"
#include "WPlane.h"
#include "linearAlgebra/WLinearAlgebra.h"

/**
 * Classes and functions of math module of OpenWalnut.
 */

// Pi constants - we don't use the macro M_PI, because it is not part of the C++-standard.
// ref.: http://stackoverflow.com/questions/1727881/how-to-use-the-pi-constant-in-c
/**
 * The pi constant in float format
 */
const float piFloat = boost::math::constants::pi<float>();

/**
 * The pi constant in double format
 */
const double piDouble = boost::math::constants::pi<double>();

/**
 * Checks if the triangle intersects with the given plane. If you are interested in the points of
 * intersection if any \see intersection().
 *
 * \param p1 first point of the triangle
 * \param p2 second point of the triangle
 * \param p3 third point of the triangle
 * \param p The plane to test with
 *
 * \return True if both intersects otherwise false.
 */
bool testIntersectTriangle( const WPosition& p1, const WPosition& p2, const WPosition& p3, const WPlane& p );

/**
 * Checks if the given segment intersects with the plane or not. Even if
 * just one endpoint intersects with the plane it should be returned as
 * point of intersection. If the segment is totally inside of that plane
 * the first endpoint (which was given: p1 ) should be returned in the
 * cutPoint parameter.
 *
 * \param p The plane to test with intersection
 * \param p1 The first endpoint of the line segment
 * \param p2 The second endpoint of the line segment
 * \param pointOfIntersection The point of intersection if any, otherwise 0,0,0
 *
 * \return True if an intersection was detected, false otherwise.
 */
bool intersectPlaneSegment( const WPlane& p,
        const WPosition& p1,
        const WPosition& p2,
        boost::shared_ptr< WPosition > pointOfIntersection );

/**
 * Checks a line (consecutive line segments) on intersection with a plane
 * and selects (if there are more than one point of intersection) the
 * closest to the base point of the plane.
 *
 * \param p The plane to test with intersection
 * \param l The line segments
 * \param cutPoint The return parameter for the point of intersection
 *
 * \return True if an intersection was detected, false otherwise.
 */
bool intersectPlaneLineNearCP( const WPlane& p, const WLine& l, boost::shared_ptr< WPosition > cutPoint );

/**
 * Computes the signum for the given value.
 *
 * \tparam Type for which must support operator< 0, and operator> 0
 * \param value To compute signum for
 *
 * \return The signum of the value so that signum( val ) * val == std::abs( val );
 */
template< typename T > int signum( const T& value );

/**
 * Calculates the odd factorial. This means 1*3*5* ... * border if border is odd, or 1*3*5* ... * (border-1) if border is even.
 * \param border the threshold for the factorial calculation.
 */
inline unsigned int oddFactorial( unsigned int border )
{
    unsigned int result = 1;
    for( unsigned int i = 3; i <= border; i+=2 )
    {
        result *= i;
    }
    return result;
}

/**
 * Calculates the even factorial. This means 2*4*6 ... * \param border if border is even, or 2*4*6* ... * ( \param border - 1 ) if border is odd.
 * \param border the threshold for the factorial calculation.
 */
inline unsigned int evenFactorial( unsigned int border )
{
    unsigned int result = 1;
    for( unsigned int i = 2; i <= border; i+=2 )
    {
        result *= i;
    }
    return result;
}

/**
 * Calculates the factorial i! for positive i.
 *
 * \note For i < 0, the result is undefined.
 *
 * \tparam The type of i.
 * \param i The input.
 * \return i!.
 */
template< typename T >
T factorial( T i )
{
    T res = static_cast< T >( 1 );
    if( i < res )
    {
        return res;
    }
    for( T k = res; k <= i; ++k )
    {
        res *= k;
    }
    return res;
}

/**
 * Checks if two values are equal within a given delta.
 *
 * \tparam The floating point type.
 * \param a The first value.
 * \param b The second value.
 * \param delta The tolerance parameter.
 * \return True if both values are equal within the delta, otherwise false.
 */
template< typename T >
T areEqual( T a, T b, T delta = T( 0 ) )
{
    return ( std::fabs( a - b ) <= delta );
}

template< typename T > inline int signum( const T& value )
{
    if( value < 0 )
    {
        return -1;
    }
    else if( value > 0 )
    {
        return 1;
    }
    return 0;
}

#endif  // WMATH_H
