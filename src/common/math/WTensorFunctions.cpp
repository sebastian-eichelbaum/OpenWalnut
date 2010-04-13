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

#include <vector>
#include <cmath>

#include "../WAssert.h"
#include "../WLimits.h"

#include "WTensorFunctions.h"

namespace wmath
{
template< typename Data_T >
void jacobiEigenvector3D( WTensorSym< 2, 3, Data_T > const& mat,
                          std::vector< Data_T >* eigenValues,
                          std::vector< WTensor< 1, 3, Data_T > >* eigenVectors )
{
    WTensorSym< 2, 3, Data_T > in = WTensorSym< 2, 3, Data_T >( mat );
    WTensor< 2, 3, Data_T > ev;

    int iter = 50;
    Data_T evp[ 3 ];
    Data_T evq[ 3 ];

    while( iter >= 0 )
    {
        int p = 1;
        int q = 0;

        for( int i = 0; i < 2; ++i )
        {
            if( fabs( in( 2, i ) ) > fabs( in( p, q ) ) )
            {
                p = 2;
                q = i;
            }
        }

        if( fabs( in( p, q ) ) == 0.0 )
        {
            for( int i = 0; i < 3; ++i )
            {
                eigenValues->at( i ) = in( i, i );
                for( int j = 0; j < 3; ++j )
                {
                    eigenVectors->at( i )( j ) = ev( i, j );
                }
            }
            return;
        }

        Data_T r = in( q, q ) - in( p, p );
        Data_T o = r / ( 2.0 * in( p, q ) );

        Data_T t;
        Data_T signofo = ( o < 0.0 ? -1.0 : 1.0 );
        if( o * o > wlimits::MAX_DOUBLE )
        {
            t = signofo / ( 2.0 * fabs( o ) );
        }
        else
        {
            t = signofo / ( fabs( o ) + sqrt( o * o + 1.0 ) );
        }

        Data_T c;

        if( t * t < wlimits::DBL_EPS )
        {
            c = 1.0;
        }
        else
        {
            c = 1.0 / sqrt( t * t + 1.0 );
        }

        Data_T s = c * t;

        int k = 0;
        while( k == q || k == p )
        {
            ++k;
        }
        WAssert( k < 3, "" );

        Data_T u = ( 1.0 - c ) / s;

        Data_T x = in( k, p );
        Data_T y = in( k, q );
        in( p, k ) = in( k, p ) = x - s * ( y + u * x );
        in( q, k ) = in( k, q ) = y + s * ( x - u * y );
        x = in( p, p );
        y = in( q, q );
        in( p, p ) = x - t * in( p, q );
        in( q, q ) = y + t * in( p, q );
        in( q, p ) = in( p, q ) = 0.0;

        for( int l = 0; l < 3; ++l )
        {
            evp[ l ] = ev( l, p );
            evq[ l ] = ev( l, q );
        }
        for( int l = 0; l < 3; ++l )
        {
            ev( l, p ) = c * evp[ l ] - s * evq[ l ];
            ev( l, q ) = s * evp[ l ] + c * evq[ l ];
        }

        --iter;
    }
    WAssert( iter >= 0, "Jacobi eigenvector iteration did not converge." );
}
}  // namespace wmath
