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
#include <cmath>
#include <utility>
#include <vector>

#include "../WLimits.h"
#include "WFiber.h"

namespace
{
    std::pair< double, double > dXt_optimized( double thresholdSquare, const WFiber &q, const WFiber &r )
    {
        const size_t qsize = q.size();
        const size_t rsize = r.size();
        double qr = 0.0;
        double rq = 0.0;

        // will contain every point-to-point square-distances
        std::vector< std::vector< double > > m( qsize, std::vector< double >( rsize, 0.0 ) );

        // double **m = new double*[qsize];

        for( size_t i = 0; i < qsize; ++i )
        {
            // m[i] = new double[rsize];
            for( size_t j = 0; j < rsize; ++j )
            {
                m[i][j] = length2( q[i] - r[j] );
            }
        }
        // compute dt(q,r)
        double minSoFar;
        for( size_t i = 0; i < qsize; ++i )
        {
            minSoFar = *( std::min_element( m[i].begin(), m[i].end() ) );
            // minSoFar = *( std::min_element( &m[i][0], &m[i][rsize] ) );
            if( minSoFar > thresholdSquare )
            {
                qr += std::sqrt( minSoFar );
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
            if( minSoFar > thresholdSquare )
            {
                rq += std::sqrt( minSoFar );
            }
        }
        rq = rq / rsize;
        return std::make_pair( qr, rq );
    }
}

WFiber::WFiber( const std::vector< WPosition > &points )
    : WLine( points )
{
}

WFiber::WFiber()
    : WLine()
{
}

double WFiber::distDST( double thresholdSquare, const WFiber &q, const WFiber &r )
{
    std::pair< double, double > result = ::dXt_optimized( thresholdSquare, q, r );
    return std::min( result.first, result.second );
}

double WFiber::distDLT( double thresholdSquare, const WFiber &q, const WFiber &r )
{
    std::pair< double, double > result = ::dXt_optimized( thresholdSquare, q, r );
    return std::max( result.first, result.second );
}
