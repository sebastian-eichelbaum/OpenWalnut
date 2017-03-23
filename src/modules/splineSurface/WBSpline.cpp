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

#include "WBSpline.h"

WBSpline::WBSpline( int order, std::vector< WVector3d > deBoorPoints )
{
    m_deBoorPoints = deBoorPoints;
    int n = m_deBoorPoints.size();
    int k = m_order = order;

    //define a normalized knotVector
    for( int i = 0; i < (n + k); i++)
    {
        double tempKnot;
        if( i < k)
            tempKnot = k - 1;
        else if( (i >= k) && (i < n))
            tempKnot = i;
        else //if( i >= n)
            tempKnot = n;

        m_knots.push_back( tempKnot );
    }
}

WBSpline::WBSpline( int order, std::vector< WVector3d > deBoorPoints, std::vector<double> knots )
{
    m_order = order;
    m_deBoorPoints = deBoorPoints;
    m_knots = knots;
}

WBSpline::~WBSpline()
{
}

WVector3d WBSpline::f( double _t )
{
    WVector3d result;
    unsigned int r = 0, i;

    if( _t < m_knots[0] )
    {
        _t = m_knots[0];
    }

    if( _t > m_knots[m_knots.size() - 1] )
    {
        _t = m_knots[m_knots.size() - 1];
    }

    m_t = _t; // set current paramter _t as class variable

    for( i = 0; i < m_knots.size(); i++ ) // -1 ?
    {
        if( m_knots[i] > _t )
        {
            break;
        }
    }

    r = i - 1;

    if( _t == m_knots[m_knots.size() - 1])
    {
        for(i = ( m_knots.size() - 1); i > 0; i-- )
        {
            if( m_knots[i] < _t )
            {
                break;
            }
        }
        r = i;
    }

    result = controlPoint_i_j( r, m_order - 1 );

    return result;
}

std::vector< WVector3d > WBSpline::getDeBoorPoints()
{
    return m_deBoorPoints;
}

std::vector<double> WBSpline::getKnots()
{
    return m_knots;
}

int WBSpline::getOrder()
{
    return m_order;
}

void WBSpline::setDeBoorPoints( std::vector< WVector3d > deBoorPoints )
{
    m_deBoorPoints = deBoorPoints;
}

void WBSpline::setKnots( std::vector<double> knots )
{
    m_knots = knots;
}

void WBSpline::setOrder( int order )
{
    m_order = order;
}

void WBSpline::samplePoints( std::vector< WVector3d > *points, double resolution )
{
    double deltaT = resolution;
    double currentT = m_knots[0];

    int steps = static_cast< int >( ( m_knots[m_knots.size() - 1] - m_knots[0] ) / deltaT + 1 );

    for( int step = 0; step < steps; step++ )
    {
        currentT = m_knots[0] + step * deltaT;
        WVector3d samplePoint = f( currentT );
        points->push_back( samplePoint );
    }
}

double WBSpline::getAlpha_i_j( int _i, int _j )
{
    double result = ( m_t - m_knots[_i] ) / ( m_knots[_i + m_order - _j] - m_knots[_i] );
    return result;
}

WVector3d WBSpline::controlPoint_i_j( int _i, int _j )
{
    WVector3d result;

    if( _j == 0)
    {
        result[0] = m_deBoorPoints[_i][0];
        result[1] = m_deBoorPoints[_i][1];
        result[2] = m_deBoorPoints[_i][2];
        return result;
    }
    double bufferedAlpha = getAlpha_i_j( _i, _j );

    result = ( 1 - bufferedAlpha ) * controlPoint_i_j( _i - 1, _j - 1 ) +
        bufferedAlpha * controlPoint_i_j( _i, _j - 1 );


    return result;
}
