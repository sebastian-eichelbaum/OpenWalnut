//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

WBSpline::WBSpline( int order, std::vector< std::vector< double > > deBoorPoints )
{
    this->deBoorPoints = deBoorPoints;
    int n = this->deBoorPoints.size();
    int k = this->order = order;

    //define a normalized knotVector
    for( int i = 0; i < (n + k); i++)
    {
        double tempKnot;
        if( i < k)
            tempKnot = k - 1;
        if( (i >= k) && (i < n))
            tempKnot = i;
        if( i >= n)
            tempKnot = n;

        knots.push_back( tempKnot );
    }
}

WBSpline::WBSpline( int order, std::vector< std::vector< double > > deBoorPoints, std::vector<double> knots )
{
    this->order = order;
    this->deBoorPoints = deBoorPoints;
    this->knots = knots;
}

WBSpline::~WBSpline()
{
}

wmath::WVector3D WBSpline::f( double _t )
{
    wmath::WVector3D result;
    unsigned int r = 0, i;

    if( _t < knots[0])
    {
        _t = knots[0];
    }

    if( _t > knots[ knots.size() - 1])
    {
        _t = knots[ knots.size() - 1];
    }

    t = _t; // set current paramter _t as class variable

    for(i = 0; i < knots.size(); i++) // -1 ?
    {
        if(knots[i] > _t)
        {
            break;
        }
    }

    r = i - 1;

    if( _t == knots[ knots.size() - 1])
    {
        for(i = (knots.size() - 1); i > 0; i--)
        {
            if( knots[i] < _t )
            {
                break;
            }
        }
        r = i;
    }

    result = controlPoint_i_j( r, order - 1 );

    return result;
}

std::vector< std::vector< double > > WBSpline::getDeBoorPoints()
{
    return deBoorPoints;
}

std::vector<double> WBSpline::getKnots()
{
    return this->knots;
}

int WBSpline::getOrder()
{
    return this->order;
}

void WBSpline::setDeBoorPoints( std::vector< std::vector< double > > deBoorPoints )
{
    this->deBoorPoints = deBoorPoints;
}

void WBSpline::setKnots( std::vector<double> knots )
{
    this->knots = knots;
}

void WBSpline::setOrder( int order )
{
    this->order = order;
}

void WBSpline::samplePoints( std::vector< std::vector< double > > *points, double resolution )
{
    double deltaT = resolution;
    double currentT = knots[0];

    int steps = static_cast< int >( ( knots[knots.size() - 1] - knots[0] ) / deltaT + 1 );

    for( int step = 0; step < steps; step++ )
    {
        std::vector< double > dummy;
        currentT = knots[0] + step * deltaT;
        wmath::WVector3D samplePoint = f( currentT );
        dummy.push_back( samplePoint[0] );
        dummy.push_back( samplePoint[1] );
        dummy.push_back( samplePoint[2] );
        points->push_back( dummy );
    }
}

double WBSpline::getAlpha_i_j( int _i, int _j )
{
    double result = ( t - knots[_i] ) / ( knots[_i + order - _j] - knots[_i] );
    return result;
}

wmath::WVector3D WBSpline::controlPoint_i_j( int _i, int _j )
{
    wmath::WVector3D result;

    if( _j == 0)
    {
        result[0] = deBoorPoints[_i][0];
        result[1] = deBoorPoints[_i][1];
        result[2] = deBoorPoints[_i][2];
        return result;
    }
    double bufferedAlpha = getAlpha_i_j( _i, _j );

    result = ( 1 - bufferedAlpha ) * controlPoint_i_j( _i - 1, _j - 1 ) +
        bufferedAlpha * controlPoint_i_j( _i, _j - 1 );


    return result;
}
