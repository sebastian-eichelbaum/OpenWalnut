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

#include <algorithm>
#include <vector>

#include "../common/WLimits.h"
#include "WLine.h"
#include "WPosition.h"

namespace wmath
{
WLine::WLine( const std::vector< WPosition > &points )
    : WMixinVector< wmath::WPosition >( points )
{
}

WLine::WLine()
    : WMixinVector< wmath::WPosition >()
{
}

void WLine::reverseOrder()
{
    std::reverse( begin(), end() );
}

double WLine::pathLength() const
{
    double length = 0;
    // incase of size() <= 1 the for loop will not run!
    for( size_t i = 1; i < size(); ++i )
    {
        length += ( at( i - 1 ) - at( i ) ).norm();
    }
    return length;
}

void WLine::resample( size_t numPoints )
{
    if( size() != numPoints && size() > 0 && numPoints > 0 )
    {
        const double newSegmentLength = pathLength() / ( numPoints - 1 );
        WLine newLine;
        newLine.reserve( numPoints );

        double remainingLength = 0.0;
        newLine.push_back( front() );
        for( size_t i = 1; i < size(); ++i )
        {
            remainingLength += ( at( i - 1 ) - at( i ) ).norm();
            while( ( remainingLength > newSegmentLength ) || std::abs( remainingLength - newSegmentLength ) < wlimits::DBL_EPS )
            {
                remainingLength -= newSegmentLength;
                WPosition newPoint = at( i ) + remainingLength * ( at( i - 1 ) - at( i ) ).normalized();
                newLine.push_back( newPoint );
            }
        }
        // using string_utils::operator<<;
        // std::cout << "|remL - newSegL|: " << std::abs( remainingLength - newSegmentLength ) << std::endl;
        // std::cout << std::endl << "this: " << *this << std::endl << "new:  " << newLine << std::endl;
        // std::cout << std::setprecision( 35 ) << "remainingLength: " << remainingLength << " newSegmentLength: " << newSegmentLength << std::endl;
        // std::cout << "this size: " << size() << " new size: " << newLine.size() << std::endl;
        *this = newLine;
    }
    assert( size() == numPoints && "Resampling of a line has failed! Is your fiber of length 0 or even the new sample rate??" );
}

} // end of namespace
