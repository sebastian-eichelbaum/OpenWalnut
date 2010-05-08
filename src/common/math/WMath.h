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

#if defined ( _MSC_VER )
#include "float.h"
#endif

/**
 * Classes and functions of math module of OpenWalnut.
 */
namespace wmath
{
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
}

#endif  // WMATH_H
