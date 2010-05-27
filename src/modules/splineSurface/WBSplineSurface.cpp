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

#include "WBSplineSurface.h"
#include "WBSpline.h"


WBSplineSurface::WBSplineSurface( int order1,
                                  int order2,
                                  std::vector< std::vector< double > > deBoorPoints,
                                  int numDeBoorPoints1,
                                  int numDeBoorPoints2 )
{
    this->m_order1 = order1;
    this->m_order2 = order2;
    this->m_deBoorPoints = deBoorPoints;
    this->m_numDeBoorPoints1 = numDeBoorPoints1;
    this->m_numDeBoorPoints2 = numDeBoorPoints2;

    //define a normalized knotVector1
    int n = this->m_numDeBoorPoints1;
    int k = this->m_order1;
    for( int i = 0; i < ( n + k ); i++ )
    {
        int tempKnot = 0;
        if( i < k)
            tempKnot = k - 1;
        if( ( i >= k ) && ( i < n ) )
            tempKnot = i;
        if( i >= n )
            tempKnot = n;

        m_knots1.push_back( tempKnot );
    }

    //define a normalized knotVector2
    n = this->m_numDeBoorPoints2;
    k = this->m_order2;
    for( int i = 0; i < ( n + k ); i++)
    {
        int tempKnot = 0;
        if( i < k )
            tempKnot = k - 1;
        if( ( i >= k ) && ( i < n ) )
            tempKnot = i;
        if( i >= n )
            tempKnot = n;

        m_knots2.push_back( tempKnot );
    }
}

WBSplineSurface::WBSplineSurface( int order1,
                                  int order2,
                                  std::vector< std::vector< double > > deBoorPoints,
                                  int numDeBoorPoints1,
                                  int numDeBoorPoints2,
                                  std::vector<double> knots1,
                                  std::vector<double> knots2 )
{
    this->m_order1 = order1;
    this->m_order2 = order2;
    this->m_deBoorPoints = deBoorPoints;
    this->m_numDeBoorPoints1 = numDeBoorPoints1;
    this->m_numDeBoorPoints2 = numDeBoorPoints2;
    this->m_knots1 = knots1;
    this->m_knots2 = knots2;
}

WBSplineSurface::~WBSplineSurface()
{
}

wmath::WVector3D WBSplineSurface::f( double _t, double _u )
{
      // numDeBoorPoints1 -> t-parameter (knots1)
      // n 0,0 _____________x_____________
      // u    |  |  |  |  |  |  |  |  |  |
      // m    |_____________x____________|
      // D    |  |  |  |  |  |  |  |  |  |
      // e    |_____________x____________| first all splines with t-param
      // B    |  |  |  |  |  |  |  |  |  | then final spline with u-param
      // o    |_____________x____________|
      // o    |  |  |  |  |  |  |  |  |  |
      // r    |____________[x]___________| f(t, u)
      // P    |  |  |  |  |  |  |  |  |  |
      // 2    |_____________x____________|
      // ->   |  |  |  |  |  |  |  |  |  |
      // u    |_____________x____________|
      // -    |  |  |  |  |  |  |  |  |  |
      // param|_____________x____________|
      // (knots2)|  |  |  |  |  |  |  |  |  |
      // |_____________x____________|

    std::vector< std::vector< double > > uSplineDeBoorPoints;

    for( int row = 0; row < m_numDeBoorPoints2; row++ )
    {
        std::vector< std::vector< double > > tSplineDeBoorPoints;

        for( int col = 0; col < m_numDeBoorPoints1; col++ )
        {
            tSplineDeBoorPoints.push_back( this->m_deBoorPoints[row * m_numDeBoorPoints1 + col] );
        }

        WBSpline tSpline( m_order1, tSplineDeBoorPoints );
        wmath::WVector3D dmyArray = tSpline.f( _t );

        std::vector< double > dmyVector;
        dmyVector.push_back( dmyArray[0] );
        dmyVector.push_back( dmyArray[1] );
        dmyVector.push_back( dmyArray[2] );

        uSplineDeBoorPoints.push_back( dmyVector );
    }

    WBSpline uSpline( m_order2, uSplineDeBoorPoints );
    return uSpline.f( _u );
}

std::vector< std::vector< double > > WBSplineSurface::getDeBoorPoints()
{
    return m_deBoorPoints;
}

int WBSplineSurface::getNumDeBoorPoints1()
{
    return m_numDeBoorPoints1;
}

int WBSplineSurface::getNumDeBoorPoints2()
{
    return m_numDeBoorPoints2;
}

std::vector<double> WBSplineSurface::getKnots1()
{
    return m_knots1;
}

std::vector<double> WBSplineSurface::getKnots2()
{
    return m_knots2;
}

int WBSplineSurface::getOrder1()
{
    return m_order1;
}

int WBSplineSurface::getOrder2()
{
    return m_order2;
}

int WBSplineSurface::getNumSamplePointsT()
{
    return m_numSamplePointsT;
}

int WBSplineSurface::getNumSamplePointsU()
{
    return m_numSamplePointsU;
}

void WBSplineSurface::setDeBoorPoints( std::vector< std::vector< double > > deBoorPoints, int numDeBoorPoints1, int numDeBoorPoints2 )
{
    this->m_deBoorPoints = deBoorPoints;
    this->m_numDeBoorPoints1 = numDeBoorPoints1;
    this->m_numDeBoorPoints2 = numDeBoorPoints2;
}

void WBSplineSurface::setKnots1( std::vector<double> knots )
{
    this->m_knots1 = knots;
}

void WBSplineSurface::setKnots2( std::vector<double> knots )
{
    this->m_knots2 = knots;
}

void WBSplineSurface::setOrder1( int order )
{
    this->m_order1 = order;
}

void WBSplineSurface::setOrder2( int order )
{
    this->m_order2 = order;
}

void WBSplineSurface::samplePoints( std::vector< std::vector< double > > *points, double tResolution, double uResolution )
{
    double currentT = m_knots1[0];
    double currentU = m_knots2[0];
    double deltaT = tResolution;
    double deltaU = uResolution;


    int stepsT = static_cast< int >( ( m_knots1[m_knots1.size() - 1] - m_knots1[0] ) / deltaT + 1 );
    m_numSamplePointsT = stepsT;
    int stepT = 0;
    int stepsU = static_cast< int >( ( m_knots2[m_knots2.size() - 1] - m_knots2[0] ) / deltaU + 1 );
    m_numSamplePointsU = stepsU;
    int stepU = 0;

    for( stepU = 0; stepU < stepsU; stepU++ )
    {
        currentU = m_knots2[0] + stepU * deltaU;
        for( stepT = 0; stepT < stepsT; stepT++ )
        {
            std::vector< double > dmy;

            currentT = m_knots1[0] + stepT * deltaT;
            wmath::WVector3D samplePoint = this->f( currentT, currentU );
            dmy.push_back( samplePoint[0] );
            dmy.push_back( samplePoint[1] );
            dmy.push_back( samplePoint[2] );
            points->push_back( dmy );
        }
    }

    return;
}