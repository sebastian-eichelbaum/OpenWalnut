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

#if defined ( _MSC_VER )
#include "float.h"
#endif

/**
 * Classes and functions of math module of OpenWalnut.
 */
namespace wmath
{
//   Pi constants - we dont use the macro M_PI, because it is not part of the C++-standard.
//   ref.: http://stackoverflow.com/questions/1727881/how-to-use-the-pi-constant-in-c
  /** the pi constant in float format */
  const float piFloat = boost::math::constants::pi<float>();
  /** the pi constant in double format */
  const double piDouble = boost::math::constants::pi<double>();

/**
 * Tests whether the number stored in the parameter is finite.
 * \param number the number to be tested
 */
inline int myIsfinite( double number )
{
#if defined( __linux__ ) || defined( __APPLE__ )
    // C99 defines isfinite() as a macro.
    return std::isfinite(number);
#elif defined( _WIN32 )
    // Microsoft Visual C++ and Borland C++ Builder use _finite().
    return _finite(number);
#else
    WAssert( false, "isfinite not provided on this platform or platform not known." );
#endif
}

/**
 * Calculates the odd factorial. This means 1*3*5* ... * border if border is odd, or 1*3*5* ... * (border-1) if border is even.
 * \param border the threshold for the factorial calculation.
 */
inline unsigned int oddFactorial( unsigned int border )
{
  unsigned int result = 1;
  for ( unsigned int i = 3; i <= border; i+=2 )
    result *= i;
  return result;
}

/**
 * Calculates the even factorial. This means 2*4*6 ... * \param border if border is even, or 2*4*6* ... * ( \param border - 1 ) if border is odd.
 * \param border the threshold for the factorial calculation.
 */
inline unsigned int evenFactorial( unsigned int border )
{
  unsigned int result = 1;
  for ( unsigned int i = 2; i <= border; i+=2 )
    result *= i;
  return result;
}
}

#endif  // WMATH_H
