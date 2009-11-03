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

#include "WFiber.h"
#include "../common/WLimits.h"

namespace wmath
{
    WFiber::WFiber( const std::vector< WPosition > &points )
        : WLine( points )
    {
    }

    std::pair< double, double > WFiber::dXt_optimized( const WFiber &other,
                                                       const double thresholdSquare ) const
    {
        const WFiber &q = *this;
        const WFiber &r = other;
        const size_t qsize = q.size();
        const size_t rsize = r.size();
        double qr = 0.0;
        double rq = 0.0;

        std::vector< std::vector< double > > m( qsize, std::vector< double >( rsize, 0.0 ) );
        for( size_t i = 0; i < qsize; ++i )
        {
            for( size_t j = 0; j < rsize; ++j )
            {
                m[i][j] = q[i].distanceSquare( r[j] );
            }
        }
        double minSoFar;
        for( size_t i = 0; i < qsize; ++i )
        {
            minSoFar = *( std::min_element( m[i].begin(), m[i].end() ) );
            if( minSoFar > thresholdSquare )
            {
                qr += sqrt( minSoFar );
            }
        }
        qr = qr / qsize;
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
                rq += sqrt( minSoFar );
            }
        }
        rq = rq / rsize;
        return std::make_pair( qr, rq );
    }

    double WFiber::dSt( const WFiber& other, const double thresholdSquare ) const
    {
        std::pair< double, double > result = dXt_optimized( other, thresholdSquare );
        return std::min( result.first, result.second );
    }

    double WFiber::dLt( const WFiber& other, const double thresholdSquare ) const
    {
        std::pair< double, double > result = dXt_optimized( other, thresholdSquare );
        return std::max( result.first, result.second );
    }
}
