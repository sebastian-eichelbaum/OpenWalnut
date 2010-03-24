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

#include <cmath>
#include <vector>

#include "../../common/WAssert.h"
#include "../../common/WLimits.h"

#include "WEigenThread.h"

WEigenThread::WEigenThread( boost::shared_ptr< WGridRegular3D > grid,
                            boost::shared_ptr< WValueSet< float > > values,
                            boost::shared_ptr< WProgress > progr,
                            uint32_t index,
                            boost::shared_ptr< std::vector< wmath::WVector3D > > eigenVectors,
                            boost::shared_ptr< std::vector< double > > fa )
    : m_grid( grid ),
      m_values( values ),
      m_progress( progr ),
      m_index( index ),
      m_eigenVectors( eigenVectors ),
      m_FA( fa )
{
}

WEigenThread::~WEigenThread()
{
}

void WEigenThread::threadMain()
{
    size_t sx, tx, sy, ty, sz, tz;

    sx = ( m_index & 1u ? m_grid->getNbCoordsX() / 2 : 0 );
    tx = ( m_index & 1u ? m_grid->getNbCoordsX() : m_grid->getNbCoordsX() / 2 );
    sy = ( m_index & 2u ? m_grid->getNbCoordsY() / 2 : 0 );
    ty = ( m_index & 2u ? m_grid->getNbCoordsY() : m_grid->getNbCoordsY() / 2 );
    sz = ( m_index & 4u ? m_grid->getNbCoordsZ() / 2 : 0 );
    tz = ( m_index & 4u ? m_grid->getNbCoordsZ() : m_grid->getNbCoordsZ() / 2 );

    for( size_t k = sz; k < tz; ++k )
    {
        for( size_t j = sy; j < ty; ++j )
        {
            for( size_t i = sx; i < tx; ++i )
            {
                ++*m_progress;

                size_t t = i + j * m_grid->getNbCoordsX() + k * m_grid->getNbCoordsX() * m_grid->getNbCoordsY();
                eigen( t );
            }
        }
    }
}

void WEigenThread::jacobi( wmath::WMatrix< double >* inmat, wmath::WMatrix< double >* eigen_vectors )
{
    wmath::WMatrix< double >& ev = *eigen_vectors;
    ev.makeIdentity();
    wmath::WMatrix< double >& in = *inmat;

    int iter = 50;
    double evp[ 3 ];
    double evq[ 3 ];

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
        return;
    }

    double r = in( q, q ) - in( p, p );
    double o = r / ( 2.0 * in( p, q ) );

    double t;
    double signofo = ( o < 0.0 ? -1.0 : 1.0 );
    if( o * o > wlimits::MAX_DOUBLE )
    {
        t = signofo / ( 2.0 * fabs( o ) );
    }
    else
    {
        t = signofo / ( fabs( o ) + sqrt( o * o + 1.0 ) );
    }

    double c;

    if( t * t < wlimits::DBL_EPS )
    {
        c = 1.0;
    }
    else
    {
        c = 1.0 / sqrt( t * t + 1.0 );
    }

    double s = c * t;

    int k = 0;
    while( k == q || k == p )
    {
        ++k;
    }
    WAssert( k < 3, "" );

    double u = ( 1.0 - c ) / s;

    double x = in( k, p );
    double y = in( k, q );
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

void WEigenThread::eigen( size_t i )
{
    wmath::WMatrix< double > m( 3, 3 );
    m( 0, 0 ) = m_values->rawData()[ 6 * i + 0 ];
    m( 0, 1 ) = m_values->rawData()[ 6 * i + 1 ];
    m( 0, 2 ) = m_values->rawData()[ 6 * i + 2 ];
    m( 1, 1 ) = m_values->rawData()[ 6 * i + 3 ];
    m( 1, 2 ) = m_values->rawData()[ 6 * i + 4 ];
    m( 2, 2 ) = m_values->rawData()[ 6 * i + 5 ];

    m( 1, 0 ) = m( 0, 1 );
    m( 2, 0 ) = m( 0, 2 );
    m( 2, 1 ) = m( 1, 2 );

    wmath::WMatrix< double > vectors( 3, 3 );

    jacobi( &m, &vectors );

    int u = 0;
    double h = 0.0;
    for( int n = 0; n < 3; ++n )
    {
        if( fabs( m( n, n ) ) > h )
        {
            h = fabs( m( n, n ) );
            u = n;
        }
    }

    m_eigenVectors->at( i ) = wmath::WVector3D( vectors( 0, u ), vectors( 1, u ), vectors( 2, u ) );

    calcFA( m( 0, 0 ), m( 1, 1 ), m( 2, 2 ), i );
}

void WEigenThread::calcFA( double const t0, double const t1, double const t2, size_t const i )
{
    float d = t0 * t0 + t1 * t1 + t2 * t2;
    if( fabs( d ) == 0.0 )
    {
        m_FA->at( i ) = 0.0;
        return;
    }

    float trace = ( t0 + t1 + t2 ) / 3;
    m_FA->at( i ) = sqrt( 1.5 * ( ( t0 - trace ) * ( t0 - trace ) + ( t1 - trace ) * ( t1 - trace ) + ( t2 - trace ) * ( t2 - trace ) ) / d );
}
