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

#include <iostream>
#include <vector>

#include "../../common/math/WPosition.h"
#include "WSimpleResampler.h"

WSimpleResampler::WSimpleResampler( size_t numSamples )
    : m_numSamples( numSamples )
{
}

void WSimpleResampler::resample( boost::shared_ptr< const std::vector< double > > verts,
                                 const size_t startIdx,
                                 const size_t length,
                                 boost::shared_ptr< std::vector< double > > newVerts,
                                 const size_t newStartIdx ) const
{
    if( length != m_numSamples && length > 0 && m_numSamples > 0 )
    {
        // 1. compute length of tract
        // 2. compute new segment length
        // 3. walk along the tract and make every new segment length a new sample point.
        // 4. YES I know that this does not result in perfect equidistant
        //    sample points, but thats exactly the reason why this algo has the
        //    SIMPLE in its name :P
        const double pathLength = lineIntegration( verts, startIdx, length );
        const double newSegmentLength = pathLength / ( m_numSamples - 1 );

        double remainingLength = 0.0;

        (*newVerts)[newStartIdx * 3    ] = (*verts)[startIdx * 3   ];
        (*newVerts)[newStartIdx * 3 + 1] = (*verts)[startIdx * 3 + 1];
        (*newVerts)[newStartIdx * 3 + 2] = (*verts)[startIdx * 3 + 2];
        for( size_t i = startIdx * 3, j = ( newStartIdx + 1 ) * 3; i < ( startIdx + length - 1 ) * 3; i += 3 )
        {
            wmath::WPosition current( ( *verts )[i], ( *verts )[i + 1], ( *verts )[i + 2] );
            wmath::WPosition next( ( *verts )[i + 3], ( *verts )[i + 4], ( *verts )[i + 5] );

            remainingLength += ( current - next ).norm();
            while( remainingLength > newSegmentLength )
            {
                remainingLength -= newSegmentLength;
                wmath::WPosition newPoint = next + remainingLength * ( current - next ).normalized();
                ( *newVerts )[j    ] = newPoint[0];
                ( *newVerts )[j + 1] = newPoint[1];
                ( *newVerts )[j + 2] = newPoint[2];
                j += 3;
            }
        }
    }
}

double WSimpleResampler::lineIntegration( boost::shared_ptr< const std::vector< double > > verts,
                                          const size_t startIdx,
                                          const size_t length ) const
{
    double result = 0.0;
    size_t start = startIdx * 3;
    wmath::WPosition last( ( *verts )[start], ( *verts )[start + 1], ( *verts )[start + 2] );
    for( size_t i = startIdx * 3; i < ( startIdx + length - 1 ) * 3; i += 3 )
    {
        wmath::WPosition current( ( *verts )[i], ( *verts )[i + 1], ( *verts )[i + 2] );
        wmath::WPosition next( ( *verts )[i + 3], ( *verts )[i + 4], ( *verts )[i + 5] );

        result += ( current - next ).norm();
    }
    return result;
}
