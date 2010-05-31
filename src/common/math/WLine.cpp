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
#include <iostream>
#include <vector>

#include "../WLimits.h"
#include "../WStringUtils.h"
#include "../exceptions/WOutOfBounds.h"
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

const wmath::WPosition& WLine::midPoint() const
{
    if( empty() )
    {
        throw WOutOfBounds( "There is no midpoint for an empty line." );
    }
    return at( ( size() - 1 ) / 2 );
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
        const double pathL = pathLength();
        double newSegmentLength = pathL / ( numPoints - 1 );
        const double delta = newSegmentLength * 1.0e-10; // 1.0e-10 which represents the precision is choosen by intuition

        WLine newLine;
        newLine.reserve( numPoints );

        double remainingLength = 0.0;
        newLine.push_back( front() );
        for( size_t i = 0; i < ( size() - 1 ); ++i )
        {
            remainingLength += ( at( i ) - at( i + 1 ) ).norm();
            while( ( remainingLength > newSegmentLength ) || std::abs( remainingLength - newSegmentLength ) < delta )
            {
                remainingLength -= newSegmentLength;
                // TODO(math): fix numerical issuses: newSegmentLength may be wrong => great offset by many intraSegment sample points
                //                                    remainingLength may be wrong => ...
                //                                    Take a look at the unit test testNumericalStabilityOfResampling
                WPosition newPoint = at( i + 1 ) + remainingLength * ( at( i ) - at( i + 1 ) ).normalized();
                newLine.push_back( newPoint );
                // std::cout << "line size so far" << newLine.size() << " lenght so far: " << newLine.pathLength() << std::endl;
                // std::cout << numPoints - newLine.size() << std::endl;
             }
        }
        // using string_utils::operator<<;
        // std::cout << "this: " << *this << std::endl << "new:  " << newLine << std::endl;
        // std::cout << "|remL - newSegL|: " << std::abs( remainingLength - newSegmentLength ) << std::endl;
        // std::cout << std::setprecision( 35 ) << "remainingLength: " << remainingLength << " newSegmentLength: " << newSegmentLength << std::endl;
        // std::cout << "this size: " << size() << " new size: " << newLine.size() << std::endl;
        this->WMixinVector< wmath::WPosition >::operator=( newLine );
    }
    WAssert( size() == numPoints, "Resampling of a line has failed! Is your line of length 0 or even the new sample rate??" );
}

int WLine::equalsDelta( const wmath::WLine& other, double delta ) const
{
    size_t pts = ( std::min )( other.size(), size() ); // This ( std::min ) thing compiles also under Win32/Win64
    size_t diffPos = 0;
    bool sameLines = true;
    for( diffPos = 0; diffPos < pts; ++diffPos )
    {
        for( int x = 0; x < 3; ++x )
        {
            WAssert( at( diffPos ).size() == 3 && other.at( diffPos ).size() == 3, "Wrong dimension of positions." );
            sameLines = sameLines && ( std::abs( at( diffPos )[x] - other.at( diffPos )[x] ) <= delta );
            if( !sameLines )
            {
                break;
            }
        }
        if( !sameLines )
        {
            break;
        }
    }
    if( sameLines )
    {
        if( size() == other.size() )
        {
            return -1;
        }
    }
    return diffPos;
}

} // end of namespace
