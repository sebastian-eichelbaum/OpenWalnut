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

#include "WPosition.h"
#include "WVector3D.h"
#include "WPlane.h"

/**
 * All helper functions of math module of OpenWalnut.
 */
namespace wmath
{
    /**
     * Checks if the triangle intersects with the given plane. If you are interessted in the points of
     * intersection if any \see intersection().
     *
     * \param p1 first point of the triangle
     * \param p2 second point of the triangle
     * \param p3 third point of the triangle
     * \param p The plane to test with
     *
     * \return True if both intersects otherwise false.
     */
    bool testIntersectTriangle( const wmath::WPosition& p1, const wmath::WPosition& p2, const wmath::WPosition& p3, const WPlane& p );
}

#endif  // WMATH_H
