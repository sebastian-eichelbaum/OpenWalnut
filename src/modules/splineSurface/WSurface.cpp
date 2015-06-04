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
#include <fstream>
#include <map>
#include <vector>

#include "core/common/math/WLinearAlgebraFunctions.h"
#include "core/common/math/WMatrix.h"
#include "core/common/math/WTensorFunctions.h"
#include "WSurface.h"

WSurface::WSurface()
    : m_tMesh( new WTriangleMesh( 0, 0 ) ),
      m_radius( 30.0 ),
      m_mu( 8.0 ),
      m_numDeBoorRows( 12 ),
      m_numDeBoorCols( 12 ),
      m_order( 4 ),
      m_sampleRateT( 0.5 ),
      m_sampleRateU( 0.5 ),
      m_xAverage( 0.0 ),
      m_yAverage( 0.0 ),
      m_zAverage( 0.0 )
{
}

WSurface::~WSurface()
{
}

WTensorSym< 2, 3, double > WSurface::getCovarianceMatrix( std::vector< WVector3d > points )
{
    WTensorSym< 2, 3, double > result;
    m_xAverage = m_yAverage = m_zAverage = 0;

    std::vector< WVector3d >::iterator pointsIt;
    for( pointsIt = points.begin(); pointsIt != points.end(); ++pointsIt )
    {
        WVector3d dmy = *pointsIt;
        m_xAverage += dmy[0];
        m_yAverage += dmy[1];
        m_zAverage += dmy[2];
    }

    m_xAverage /= points.size();
    m_yAverage /= points.size();
    m_zAverage /= points.size();

    for( pointsIt = points.begin(); pointsIt != points.end(); ++pointsIt )
    {
        WVector3d dmy = *pointsIt;

        result( 0, 0 ) += ( dmy[0] - m_xAverage ) * ( dmy[0] - m_xAverage ); //XX
        result( 0, 1 ) += ( dmy[0] - m_xAverage ) * ( dmy[1] - m_yAverage ); //XY
        result( 0, 2 ) += ( dmy[0] - m_xAverage ) * ( dmy[2] - m_zAverage ); //XZ

        result( 1, 1 ) += ( dmy[1] - m_yAverage ) * ( dmy[1] - m_yAverage ); //YY
        result( 1, 2 ) += ( dmy[1] - m_yAverage ) * ( dmy[2] - m_zAverage ); //YZ

        result( 2, 2 ) += ( dmy[2] - m_zAverage ) * ( dmy[2] - m_zAverage ); //ZZ
    }

    result( 1, 0 ) = result( 0, 1 );
    result( 2, 0 ) = result( 0, 2 );
    result( 2, 1 ) = result( 1, 2 );

    return result;
}

void WSurface::getSplineSurfaceDeBoorPoints( std::vector< WVector3d > &givenPoints, std::vector< WVector3d > &deBoorPoints, int numRows, int numCols ) // NOLINT
{
    double xMin = givenPoints[0][0];
    double xMax = givenPoints[0][0];
    double zMin = givenPoints[0][2];
    double zMax = givenPoints[0][2];

    std::vector< WVector3d >::iterator givenPointsIt;

    for( givenPointsIt = givenPoints.begin(); givenPointsIt != givenPoints.end(); ++givenPointsIt )
    {
        WVector3d dmy = *givenPointsIt;
        if( dmy[0] < xMin )
        {
            xMin = dmy[0];
        }
        if( dmy[0] > xMax )
        {
            xMax = dmy[0];
        }
        if( dmy[2] < zMin )
        {
            zMin = dmy[2];
        }
        if( dmy[2] > zMax )
        {
            zMax = dmy[2];
        }
    }

    double dX = ( xMax - xMin ) / ( numCols - 1 );
    double dZ = ( zMax - zMin ) / ( numRows - 1 );

    deBoorPoints.reserve( numRows * numCols );

    for( int row = 0; row < numRows; ++row )
        for( int col = 0; col < numCols; ++col )
        {
            double x = xMin + dX * col;
            double z = zMin + dZ * row;

            double y = 0;
            double numerator = 0, denominator = 0;

            //<local shepard with franke-little-weights>
            for( givenPointsIt = givenPoints.begin(); givenPointsIt != givenPoints.end(); ++givenPointsIt )
            {
                WVector3d dmy1 = *givenPointsIt;
                WVector3d dmyArray( dmy1[0], dmy1[1], dmy1[2] );
                dmyArray[1] = 0;
                WVector3d thisPoint( x, 0, z );

                double xi; //greek alphabet

                if( length( thisPoint - dmyArray ) < m_radius )
                {
                    xi = 1 - length( thisPoint - dmyArray ) / m_radius;
                }
                else
                {
                    xi = 0;
                }

                numerator += ( pow( xi, m_mu ) * dmy1[1] );
                denominator += ( pow( xi, m_mu ) );
            }
            if( denominator == 0 )
            {
                y = 0;
            }
            else
            {
                y = numerator / denominator;
            }
            //</local shepard with franke-little-weights>
            deBoorPoints.push_back( WVector3d( x, y, z ) );
        }
    return;
}

void WSurface::execute()
{
    std::vector< WVector3d > deBoorPoints;
    m_splinePoints.clear();

    WTensorSym< 2, 3, double > myTensor = getCovarianceMatrix( m_supportPoints );

    RealEigenSystem eigenSys;

    jacobiEigenvector3D( myTensor, &eigenSys );

    eigenSys[0].second = normalize( eigenSys[0].second );
    eigenSys[1].second = normalize( eigenSys[1].second );
    eigenSys[2].second = normalize( eigenSys[2].second );

    // This sorts the entries automatically :-)
    std::map< double, WVector3d > sortedEigenSystem;
    for( size_t i = 0; i < 3 ; ++i )
    {
        sortedEigenSystem[eigenSys[i].first] = eigenSys[i].second;
    }

    WMatrix< double > transMatrix = WMatrix< double >( 3, 3 );

    std::map< double, WVector3d >::iterator sortedSystemIter = sortedEigenSystem.begin();
    transMatrix( 1, 0 ) =( *sortedSystemIter ).second[0];
    transMatrix( 1, 1 ) =( *sortedSystemIter ).second[1];
    transMatrix( 1, 2 ) =( *sortedSystemIter ).second[2];

    ++sortedSystemIter;
    transMatrix( 0, 0 ) =( *sortedSystemIter ).second[0];
    transMatrix( 0, 1 ) =( *sortedSystemIter ).second[1];
    transMatrix( 0, 2 ) =( *sortedSystemIter ).second[2];

    ++sortedSystemIter;
    transMatrix( 2, 0 ) =( *sortedSystemIter ).second[0];
    transMatrix( 2, 1 ) =( *sortedSystemIter ).second[1];
    transMatrix( 2, 2 ) =( *sortedSystemIter ).second[2];

    std::vector< WVector3d >::iterator pointsIt;

    //translate and orientate given points to origin
    for( pointsIt = m_supportPoints.begin(); pointsIt != m_supportPoints.end(); ++pointsIt )
    {
        ( *pointsIt )[0] -= m_xAverage;
        ( *pointsIt )[1] -= m_yAverage;
        ( *pointsIt )[2] -= m_zAverage;

        WVector3d dmy( ( *pointsIt )[0], ( *pointsIt )[1], ( *pointsIt )[2]  );

        WVector3d result = transMatrix * dmy;
        ( *pointsIt )[0] = result[0];
        ( *pointsIt )[1] = result[1];
        ( *pointsIt )[2] = result[2];
    }

    //get de Boor points using shepard's method
    getSplineSurfaceDeBoorPoints( m_supportPoints, deBoorPoints, m_numDeBoorRows, m_numDeBoorCols );

    //translate and orientate de Boor points back
    transMatrix = invertMatrix3x3( transMatrix );
    for( pointsIt = deBoorPoints.begin(); pointsIt != deBoorPoints.end(); ++pointsIt )
    {
        WVector3d dmy( ( *pointsIt )[0], ( *pointsIt )[1], ( *pointsIt )[2]  );

        WVector3d result = transMatrix * dmy;
        ( *pointsIt )[0] = result[0];
        ( *pointsIt )[1] = result[1];
        ( *pointsIt )[2] = result[2];

        ( *pointsIt )[0] += m_xAverage;
        ( *pointsIt )[1] += m_yAverage;
        ( *pointsIt )[2] += m_zAverage;
    }

    WBSplineSurface splineSurface( m_order, m_order, deBoorPoints, m_numDeBoorCols, m_numDeBoorRows );

    splineSurface.samplePoints( &m_splinePoints, m_sampleRateT, m_sampleRateU );

    m_renderpointsPerCol = splineSurface.getNumSamplePointsU();
    m_renderpointsPerRow = splineSurface.getNumSamplePointsT();

    boost::shared_ptr< WTriangleMesh > newMesh( new WTriangleMesh( m_splinePoints.size(), 2 * m_renderpointsPerCol * m_renderpointsPerRow ) );

    for( std::vector< WVector3d >::iterator posIt = m_splinePoints.begin(); posIt != m_splinePoints.end(); ++posIt )
    {
        newMesh->addVertex( ( *posIt )[0], ( *posIt )[1], ( *posIt )[2] );
    }

    for( int z = 0; z < m_renderpointsPerCol - 1; ++z )
    {
        for( int x = 0; x < m_renderpointsPerRow - 1; ++x )
        {
            int p0 = z * m_renderpointsPerCol + x;
            int p1 = z * m_renderpointsPerCol + x + 1;
            int p2 = ( z + 1 ) * m_renderpointsPerCol + x;
            int p3 = ( z + 1 ) * m_renderpointsPerCol + x + 1;

            newMesh->addTriangle( p0, p1, p2 );
            newMesh->addTriangle( p2, p1, p3 );
        }
    }

    m_tMesh = newMesh;
}

std::vector< WVector3d > WSurface::getSplinePoints()
{
    return m_splinePoints;
}

void WSurface::setSetSampleRate( float r )
{
    m_sampleRateT = m_sampleRateU = r;
    execute();
}

void WSurface::setSupportPoints( std::vector< WVector3d> supportPoints, bool forceUpdate )
{
    m_supportPoints = supportPoints;
    if( forceUpdate )
    {
        execute();
    }
}

boost::shared_ptr< WTriangleMesh > WSurface::getTriangleMesh()
{
    return m_tMesh;
}
