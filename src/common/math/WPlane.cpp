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
#include "../../common/math/WPosition.h"
#include "../../common/math/WVector3D.h"
#include "../../common/WAssert.h"
#include "WPlane.h"

WPlane::WPlane( const wmath::WVector3D& normal, const wmath::WPosition& pos )
    : m_normal( normal ),
      m_pos( pos )
{
    setNormal( normal );
}

WPlane::WPlane( const wmath::WVector3D& normal, const wmath::WPosition& pos, const wmath::WVector3D& first, const wmath::WVector3D& second )
    : m_normal( normal ),
      m_pos( pos )
{
    setPlaneVectors( first, second );
    m_first.normalize();
    m_second.normalize();
}

WPlane::~WPlane()
{
}

bool WPlane::isInPlane( wmath::WPosition /* point */ ) const
{
    // TODO(math): implement this: pos + first*m + second*n == point ??
    return false;
}

void WPlane::resetPosition( wmath::WPosition newPos )
{
    // TODO(math): check if pos is in plane first!
    m_pos = newPos;
}


boost::shared_ptr< std::set< wmath::WPosition > > WPlane::samplePoints( double stepWidth, size_t numX, size_t numY ) const
{
    // idea: start from m_pos in m_first direction until boundary reached, increment in m_second direction from m_pos and start again
    boost::shared_ptr< std::set< wmath::WPosition > > result( new std::set< wmath::WPosition >() );

    // the plane has two directions m_first and m_second
    const wmath::WVector3D ycrement = stepWidth * m_second;
    const wmath::WVector3D xcrement = stepWidth * m_first;
    result->insert( m_pos );
    for( size_t i = 0; i < numY; ++i )
    {
        for( size_t j = 0; j < numX; ++j )
        {
            result->insert( m_pos - i * ycrement - j * xcrement );
            result->insert( m_pos + i * ycrement - j * xcrement );
            result->insert( m_pos - i * ycrement + j * xcrement );
            result->insert( m_pos + i * ycrement + j * xcrement );
        }
    }
    return result;
}

// boost::shared_ptr< std::set< wmath::WPosition > > WPlane::samplePoints( const WGridRegular3D& grid, double stepWidth )
// {
//     // idea: start from m_pos in m_first direction until boundary reached, increment in m_second direction from m_pos and start again
//     boost::shared_ptr< std::set< wmath::WPosition > > result( new std::set< wmath::WPosition >() );
//
//     // the plane has two directions m_first and m_second
//     const wmath::WVector3D ycrement = stepWidth * m_second;
//     const wmath::WVector3D xcrement = stepWidth * m_first;
//     wmath::WPosition y_offset_up = m_pos;
//     wmath::WPosition y_offset_down = m_pos;
//     wmath::WPosition x_offset_right = m_pos;
//     wmath::WPosition x_offset_left = m_pos;
//     // TODO(math): assert( grid.encloses( m_pos ) );
//     while( grid.encloses( y_offset_up ) || grid.encloses( y_offset_down ) )
//     {
//         if( grid.encloses( y_offset_up ) ) // walk up
//         {
//             x_offset_right = y_offset_up;
//             x_offset_left = y_offset_up;
//             while( grid.encloses( x_offset_right ) || grid.encloses( x_offset_left ) )
//             {
//                 if( grid.encloses( x_offset_right ) )
//                 {
//                     result->insert( x_offset_right );
//                     x_offset_right += xcrement;
//                 }
//                 if( grid.encloses( x_offset_left ) )
//                 {
//                     result->insert( x_offset_left );
//                     x_offset_left -= xcrement;
//                 }
//             }
//             y_offset_up += ycrement;
//         }
//         if( grid.encloses( y_offset_down ) ) // walk down
//         {
//             x_offset_right = y_offset_down;
//             x_offset_left = y_offset_down;
//             while( grid.encloses( x_offset_right ) || grid.encloses( x_offset_left ) )
//             {
//                 if( grid.encloses( x_offset_right ) )
//                 {
//                     result->insert( x_offset_right );
//                     x_offset_right += xcrement;
//                 }
//                 if( grid.encloses( x_offset_left ) )
//                 {
//                     result->insert( x_offset_left );
//                     x_offset_left -= xcrement;
//                 }
//             }
//             y_offset_down -= ycrement;
//         }
//     }
//
//     return result;
// }

wmath::WPosition WPlane::getPointInPlane( double x, double y ) const
{
    return m_pos + x * m_first + y * m_second;
}

void WPlane::setPlaneVectors( const wmath::WVector3D& first, const wmath::WVector3D& second )
{
    std::stringstream msg;
    msg << "The give two vectors are not perpendicular to plane. First: " << first << " second: " << second << " for plane normal: " << m_normal;
    WAssert( std::abs( first.dotProduct( m_normal ) ) < wlimits::FLT_EPS && std::abs( second.dotProduct( m_normal ) ) < wlimits::FLT_EPS, msg.str() );

    std::stringstream msg2;
    msg2 << "The given two vectors are not linear independent!: " << first << " and " << second;
    WAssert( wmath::linearIndependent( first, second ), msg2.str() );

    m_first = first;
    m_second = second;
}
