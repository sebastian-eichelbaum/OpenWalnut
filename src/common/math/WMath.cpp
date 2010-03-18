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

#include "WMath.h"
#include "WPlane.h"
#include "WPosition.h"
#include "WVector3D.h"

bool wmath::testIntersectTriangle( const wmath::WPosition& p1, const wmath::WPosition& p2, const wmath::WPosition& p3, const WPlane& p )
{
    const WVector3D& normal = p.getNormal();
    const WPosition& planePoint = p.getPosition();

    double r1 = normal.dotProduct( p1 - planePoint );
    double r2 = normal.dotProduct( p2 - planePoint );
    double r3 = normal.dotProduct( p3 - planePoint );

    if( std::abs( ( ( r1 > 0 ) - ( r1 < 0 ) ) + ( ( r2 > 0) - ( r2 < 0 ) ) + ( ( r3 > 0 ) - ( r3 < 0 ) ) ) == 3 )
    {
        return false;
    }
    return true;
}
