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
#include "linearAlgebra/WLinearAlgebra.h"
#include "../WAssert.h"
#include "../WLimits.h"

bool testIntersectTriangle( const WPosition& p1, const WPosition& p2, const WPosition& p3, const WPlane& p )
{
    const WVector3d& normal = p.getNormal();
    const WPosition& planePoint = p.getPosition();

    double r1 = dot( normal, p1 - planePoint );
    double r2 = dot( normal, p2 - planePoint );
    double r3 = dot( normal, p3 - planePoint );

    // TODO(math): use signum here!
    if( std::abs( ( ( r1 > 0 ) - ( r1 < 0 ) ) + ( ( r2 > 0) - ( r2 < 0 ) ) + ( ( r3 > 0 ) - ( r3 < 0 ) ) ) == 3 )
    {
        return false;
    }
    return true;
}

bool intersectPlaneSegment( const WPlane& p,
                                   const WPosition& p1,
                                   const WPosition& p2,
                                   boost::shared_ptr< WPosition > pointOfIntersection )
{
    const WVector3d& normal = normalize( p.getNormal() );
    double const d = dot( normal, p.getPosition() );
    WAssert( pointOfIntersection.get(), "Place to store a point of intersection is not ready!" );
    *pointOfIntersection = p.getPosition();   // otherwise it would be undefined

    // at least one point is in plane (maybe the whole segment)
    if( std::abs( dot( normal, p1 - p.getPosition() ) ) <= 2 * wlimits::DBL_EPS )
    {
        *pointOfIntersection = p1;
        return true;
    }
    else if( std::abs( dot( normal, p2 - p.getPosition() ) ) <= 2 * wlimits::DBL_EPS )
    {
        *pointOfIntersection = p2;
        return true;
    }

    if( std::abs( dot( normal,  p1 - p2 ) ) < wlimits::DBL_EPS ) // plane and line are parallel
    {
        return false;
    }

    double const t = ( d - dot( normal, p2 ) ) / ( dot( normal, p1 - p2 ) );

    *pointOfIntersection = p2 + t * ( p1 - p2 );

    if( t >= -wlimits::DBL_EPS && t <= ( 1.0 + wlimits::DBL_EPS ) )
    {
        return true;
    }
    return false;
}

bool intersectPlaneLineNearCP( const WPlane& p, const WLine& l, boost::shared_ptr< WPosition > cutPoint )
{
    bool result = false;
    double minDistance = wlimits::MAX_DOUBLE;
    WAssert( cutPoint.get(), "Place to store a point of intersection is not ready!" );
    *cutPoint = WPosition( 0, 0, 0 );
    for( size_t i = 1; i < l.size(); ++i ) // test each segment
    {
        boost::shared_ptr< WPosition > cP( new WPosition( 0, 0, 0 ) );
        if( intersectPlaneSegment( p, l[i-1], l[i], cP ) )
        {
            result = true;
            double dist = length2( *cP - p.getPosition() );
            if( dist < minDistance )
            {
                minDistance = dist;
                *cutPoint = *cP;
            }
        }
    }
    return result;
}
