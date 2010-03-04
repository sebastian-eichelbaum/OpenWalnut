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
#include <utility>
#include <vector>

#include "WZhangMetric.h"
#include "../../WLimits.h"

WZhangMetric::WZhangMetric( double thresholdSquare )
    : m_thresholdSquare( thresholdSquare )
{
}

void WZhangMetric::setThreshold( double thresholdSquare )
{
    m_thresholdSquare = thresholdSquare;
}

std::pair< double, double > WZhangMetric::dXt_optimized( const wmath::WFiber &q,
        const wmath::WFiber &r ) const
{
    const size_t qsize = q.size();
    const size_t rsize = r.size();
    double qr = 0.0;
    double rq = 0.0;

    // will contain every point-to-point square-distances
    std::vector< std::vector< double > > m( qsize, std::vector< double >( rsize, 0.0 ) );
    for( size_t i = 0; i < qsize; ++i )
    {
        for( size_t j = 0; j < rsize; ++j )
        {
            m[i][j] = q[i].distanceSquare( r[j] );
        }
    }
    // compute dt(q,r)
    double minSoFar;
    for( size_t i = 0; i < qsize; ++i )
    {
        minSoFar = *( std::min_element( m[i].begin(), m[i].end() ) );
        if( minSoFar > m_thresholdSquare )
        {
            qr += sqrt( minSoFar );
        }
    }
    qr = qr / qsize;
    // compute dt(r,q)
    for( size_t j = 0; j < rsize; ++j )
    {
        minSoFar = wlimits::MAX_DOUBLE;
        for( size_t i = 0; i < qsize; ++i )
        {
            if( m[i][j] < minSoFar )
            {
                minSoFar = m[i][j];
            }
        }
        if( minSoFar > m_thresholdSquare )
        {
            rq += sqrt( minSoFar );
        }
    }
    rq = rq / rsize;
    return std::make_pair( qr, rq );
}



