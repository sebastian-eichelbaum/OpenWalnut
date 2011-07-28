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

#include "WCreateColorArraysThread.h"


WCreateColorArraysThread::WCreateColorArraysThread(  int left, int right, boost::shared_ptr< std::vector< float > >vertices,
        boost::shared_ptr< std::vector< size_t > > lineStartIndexes,
        boost::shared_ptr< std::vector< size_t > > lineLengths,
        boost::shared_ptr< std::vector< float > > globalColors,
        boost::shared_ptr< std::vector< float > > localColors,
        boost::shared_ptr< std::vector< float > > tangents ):
    WThreadedRunner(),
    m_myThreadFinished( false ),
    m_left( left ),
    m_right( right ),
    m_vertices( vertices ),
    m_tangents( tangents ),
    m_globalColors( globalColors ),
    m_localColors( localColors ),
    m_lineStartIndexes( lineStartIndexes ),
    m_lineLengths( lineLengths )
{
}

WCreateColorArraysThread::~WCreateColorArraysThread()
{
}

void WCreateColorArraysThread::threadMain()
{
    if( !m_vertices || !m_tangents || !m_globalColors || !m_localColors || !m_lineStartIndexes || !m_lineLengths )
    {
        return;
    }

    if( !m_vertices->size() ||
         !m_tangents->size() ||
         !m_globalColors->size() ||
         !m_localColors->size() ||
         !m_lineStartIndexes->size() ||
         !m_lineLengths->size() )
    {
        return;
    }

    int pc = 0;
    for( int i = 0; i < m_left; ++i )
    {
        pc += (*m_lineLengths)[i]*3;
    }

    float r, g, b, rr, gg, bb;
    float x1, x2, y1, y2, z1, z2;
    float lastx, lasty, lastz;
    for( int i = m_left; i <= m_right; ++i )
    {
        x1 = (*m_vertices)[pc];
        y1 = (*m_vertices)[pc + 1];
        z1 = (*m_vertices)[pc + 2];
        x2 = (*m_vertices)[pc + (*m_lineLengths)[i] * 3 - 3 ];
        y2 = (*m_vertices)[pc + (*m_lineLengths)[i] * 3 - 2 ];
        z2 = (*m_vertices)[pc + (*m_lineLengths)[i] * 3 - 1 ];
        r = ( x1 ) - ( x2 );
        g = ( y1 ) - ( y2 );
        b = ( z1 ) - ( z2 );
        if( r < 0.0 )
            r *= -1.0;
        if( g < 0.0 )
            g *= -1.0;
        if( b < 0.0 )
            b *= -1.0;

        float norm = sqrt( r * r + g * g + b * b );
        r *= 1.0 / norm;
        g *= 1.0 / norm;
        b *= 1.0 / norm;

        lastx = (*m_vertices)[pc] + ( (*m_vertices)[pc] - (*m_vertices)[pc + 3] );
        lasty = (*m_vertices)[pc+ 1] + ( (*m_vertices)[pc + 1] - (*m_vertices)[pc + 4] );
        lastz = (*m_vertices)[pc + 2] + ( (*m_vertices)[pc + 2] - (*m_vertices)[pc + 5] );

        for( size_t j = 0; j < m_lineLengths->at( i ); ++j )
        {
            rr = lastx - (*m_vertices)[pc];
            gg = lasty - (*m_vertices)[pc + 1];
            bb = lastz - (*m_vertices)[pc + 2];
            lastx = (*m_vertices)[pc];
            lasty = (*m_vertices)[pc + 1];
            lastz = (*m_vertices)[pc + 2];

            float norm = sqrt( rr * rr + gg * gg + bb * bb );
            rr *= 1.0 / norm;
            gg *= 1.0 / norm;
            bb *= 1.0 / norm;
            (*m_tangents)[pc] = rr;
            (*m_tangents)[pc+1] = gg;
            (*m_tangents)[pc+2] = bb;

            if( rr < 0.0 )
                rr *= -1.0;
            if( gg < 0.0 )
                gg *= -1.0;
            if( bb < 0.0 )
                bb *= -1.0;

            (*m_localColors)[pc] = rr;
            (*m_localColors)[pc+1] = gg;
            (*m_localColors)[pc+2] = bb;

            (*m_globalColors)[pc] = r;
            (*m_globalColors)[pc+1] = g;
            (*m_globalColors)[pc+2] = b;
            pc += 3;
        }
    }

    m_myThreadFinished = true;
}

bool WCreateColorArraysThread::isFinished()
{
    return m_myThreadFinished;
}
