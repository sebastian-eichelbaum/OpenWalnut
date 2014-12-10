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

#include <set>

#include <boost/shared_ptr.hpp>

#include "../../common/math/WLinearAlgebraFunctions.h"
#include "../../common/math/linearAlgebra/WVectorFixed.h"
#include "../../common/WAssert.h"
#include "../../common/WLimits.h"

#include "WPlane.h"

WPlane::WPlane( const WVector3d& normal, const WPosition& pos )
    : m_normal( normal ),
      m_pos( pos )
{
    setNormal( normal );
}

WPlane::WPlane( const WVector3d& normal, const WPosition& pos, const WVector3d& first, const WVector3d& second )
    : m_normal( normal ),
      m_pos( pos )
{
    setPlaneVectors( first, second );
    m_first = normalize( m_first );
    m_second = normalize( m_second );
}

WPlane::~WPlane()
{
}

bool WPlane::isInPlane( WPosition /* point */ ) const
{
    // TODO(math): implement this: pos + first*m + second*n == point ??
    return false;
}

void WPlane::resetPosition( WPosition newPos )
{
    // TODO(math): check if pos is in plane first!
    m_pos = newPos;
}


boost::shared_ptr< std::set< WPosition > > WPlane::samplePoints( double stepWidth, size_t numX, size_t numY ) const
{
    // idea: start from m_pos in m_first direction until boundary reached, increment in m_second direction from m_pos and start again
    boost::shared_ptr< std::set< WPosition > > result( new std::set< WPosition >() );

    // the plane has two directions m_first and m_second
    const WVector3d ycrement = stepWidth * m_second;
    const WVector3d xcrement = stepWidth * m_first;
    result->insert( m_pos );
    for( size_t i = 0; i < numY; ++i )
    {
        for( size_t j = 0; j < numX; ++j )
        {
            // NOTE: on some machines with older compilers, the size_t is not mapped to uint32_t or uint64_t. This
            // breaks our WMatrixFixed type promotion. So we use doubles directly.
            double id = i;
            double jd = j;
            result->insert( m_pos - id * ycrement - jd * xcrement );
            result->insert( m_pos + id * ycrement - jd * xcrement );
            result->insert( m_pos - id * ycrement + jd * xcrement );
            result->insert( m_pos + id * ycrement + jd * xcrement );
        }
    }
    return result;
}

WPosition WPlane::getPointInPlane( double x, double y ) const
{
    WVector3d sd= m_pos +
                    x * m_first
                    +
                    y * m_second;
    return sd;
}

void WPlane::setPlaneVectors( const WVector3d& first, const WVector3d& second )
{
    std::stringstream msg;
    msg << "The give two vectors are not perpendicular to plane. First: " << first << " second: " << second << " for plane normal: " << m_normal;
    WAssert( std::abs( dot( first, m_normal ) ) < wlimits::FLT_EPS && std::abs( dot( second, m_normal ) ) < wlimits::FLT_EPS, msg.str() );

    std::stringstream msg2;
    msg2 << "The given two vectors are not linear independent!: " << first << " and " << second;
    WAssert( linearIndependent( first, second ), msg2.str() );

    m_first = first;
    m_second = second;
}
