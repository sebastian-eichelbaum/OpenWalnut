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

#include <fstream>
#include <vector>

#include "math.h"
#include "../lic/fantom/FMatrix.h"

#include "WSurface.h"

WSurface::WSurface()
{
    m_radius = 30.0;
    m_my = 8.0;
    m_numDeBoorRows = 12;
    m_numDeBoorCols = 12;
    m_order = 4;
    m_sampleRateT = m_sampleRateU = 0.5;
    m_numPoints = 0;
    m_tMesh = boost::shared_ptr< WTriangleMesh2 >( new WTriangleMesh2( 0, 0 ) );
}

WSurface::~WSurface()
{
}

FTensor WSurface::getCovarianceMatrix( std::vector< std::vector< double > > points )
{
    FTensor result( 3, 2, true );
    m_xAverage = m_yAverage = m_zAverage = 0;

    std::vector< std::vector< double > >::iterator pointsIt;
    for ( pointsIt = points.begin(); pointsIt != points.end(); pointsIt++ )
    {
        std::vector< double > dmy = *pointsIt;
        m_xAverage += dmy[0];
        m_yAverage += dmy[1];
        m_zAverage += dmy[2];
    }

    m_xAverage /= points.size();
    m_yAverage /= points.size();
    m_zAverage /= points.size();

    for ( pointsIt = points.begin(); pointsIt != points.end(); pointsIt++ )
    {
        std::vector< double > dmy = *pointsIt;

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

void WSurface::getSplineSurfaceDeBoorPoints( std::vector< std::vector< double > > &givenPoints, std::vector< std::vector< double > > &deBoorPoints, int numRows, int numCols ) // NOLINT
{
    double xMin = givenPoints[0][0];
    double xMax = givenPoints[0][0];
    double zMin = givenPoints[0][2];
    double zMax = givenPoints[0][2];

    std::vector< std::vector< double > >::iterator givenPointsIt;

    for ( givenPointsIt = givenPoints.begin(); givenPointsIt != givenPoints.end(); givenPointsIt++ )
    {
        std::vector< double > dmy = *givenPointsIt;
        if ( dmy[0] < xMin )
            xMin = dmy[0];
        if ( dmy[0] > xMax )
            xMax = dmy[0];
        if ( dmy[2] < zMin )
            zMin = dmy[2];
        if ( dmy[2] > zMax )
            zMax = dmy[2];
    }

    double dX = ( xMax - xMin ) / ( numCols - 1 );
    double dZ = ( zMax - zMin ) / ( numRows - 1 );

    deBoorPoints.reserve( numRows * numCols );

    for ( int row = 0; row < numRows; ++row )
        for ( int col = 0; col < numCols; ++col )
        {
            std::vector< double > dmy;
            double x = xMin + dX * col;
            double z = zMin + dZ * row;
            dmy.push_back( x );

            double y = 0;
            double numerator = 0, denominator = 0;

            //<local shepard with franke-little-weights>
            for ( givenPointsIt = givenPoints.begin(); givenPointsIt != givenPoints.end(); givenPointsIt++ )
            {
                std::vector< double > dmy1 = *givenPointsIt;
                FArray dmyArray( dmy1 );
                dmyArray[1] = 0;
                FArray thisPoint( x, 0, z );

                double xi; //greek alphabet

                if ( thisPoint.distance( dmyArray ) < m_radius )
                    xi = 1 - thisPoint.distance( dmyArray ) / m_radius;
                else
                    xi = 0;

                numerator += ( pow( xi, m_my ) * dmy1[1] );
                denominator += ( pow( xi, m_my ) );
            }
            if ( denominator == 0 )
                y = 0;
            else
                y = numerator / denominator;
            //</local shepard with franke-little-weights>
            dmy.push_back( y );
            dmy.push_back( z );

            deBoorPoints.push_back( dmy );
        }
    return;
}

boost::shared_ptr< WTriangleMesh2 > WSurface::execute()
{
    std::vector< std::vector< double > > givenPoints;

    for ( int y = 0; y < 11; ++y )
    {
        for ( int z = 0; z < 11; ++z )
        {
            std::vector< double > p( 3 );
            float pi = 3.14159265;
            float pi2 = pi * 2;
            float yy = pi2 * y / 10.;
            float zz = pi2 * z / 10.;

            p[0] = 60. + sin( yy ) * 10 + cos( zz ) * 10;
            p[1] = y * 20.;
            p[2] = z * 16.;
            givenPoints.push_back( p );
        }
    }

    std::vector< std::vector< double > > deBoorPoints;
    m_splinePoints.clear();

    FTensor myTensor = getCovarianceMatrix( givenPoints );

    FArray eigenValues( 3 );
    FArray eigenVectors[3];
    eigenVectors[0] = FArray( 3 );
    eigenVectors[1] = FArray( 3 );
    eigenVectors[2] = FArray( 3 );

    myTensor.getEigenSystem( eigenValues, eigenVectors );

    eigenVectors[0].normalize();
    eigenVectors[1].normalize();
    eigenVectors[2].normalize();

    FTensor::sortEigenvectors( eigenValues, eigenVectors );

    FMatrix transMatrix = FMatrix( 3, 3 );
    transMatrix( 0, 0 ) = eigenVectors[1][0];
    transMatrix( 0, 1 ) = eigenVectors[1][1];
    transMatrix( 0, 2 ) = eigenVectors[1][2];

    transMatrix( 1, 0 ) = eigenVectors[2][0];
    transMatrix( 1, 1 ) = eigenVectors[2][1];
    transMatrix( 1, 2 ) = eigenVectors[2][2];

    transMatrix( 2, 0 ) = eigenVectors[0][0];
    transMatrix( 2, 1 ) = eigenVectors[0][1];
    transMatrix( 2, 2 ) = eigenVectors[0][2];

    std::vector< std::vector< double > >::iterator pointsIt;

    //translate and orientate given points to origin
    for ( pointsIt = givenPoints.begin(); pointsIt != givenPoints.end(); pointsIt++ )
    {
        ( *pointsIt )[0] -= m_xAverage;
        ( *pointsIt )[1] -= m_yAverage;
        ( *pointsIt )[2] -= m_zAverage;

        FArray dmy( *pointsIt );

        F::FVector result = transMatrix * dmy;
        ( *pointsIt )[0] = result[0];
        ( *pointsIt )[1] = result[1];
        ( *pointsIt )[2] = result[2];
    }

    //get de Boor points using shepard's method
    getSplineSurfaceDeBoorPoints( givenPoints, deBoorPoints, m_numDeBoorRows, m_numDeBoorCols );

    //translate and orientate de Boor points back
    transMatrix.invert();
    for ( pointsIt = deBoorPoints.begin(); pointsIt != deBoorPoints.end(); pointsIt++ )
    {
        FArray dmy( *pointsIt );

        F::FVector result = transMatrix * dmy;
        ( *pointsIt )[0] = result[0];
        ( *pointsIt )[1] = result[1];
        ( *pointsIt )[2] = result[2];

        ( *pointsIt )[0] += m_xAverage;
        ( *pointsIt )[1] += m_yAverage;
        ( *pointsIt )[2] += m_zAverage;
    }

    FBSplineSurface splineSurface( m_order, m_order, deBoorPoints, m_numDeBoorCols, m_numDeBoorRows );

    splineSurface.samplePoints( m_splinePoints, m_sampleRateT, m_sampleRateU );

    std::vector< double > positions;

    m_renderpointsPerCol = splineSurface.getNumSamplePointsU();
    m_renderpointsPerRow = splineSurface.getNumSamplePointsT();

    boost::shared_ptr< WTriangleMesh2 > newMesh = boost::shared_ptr< WTriangleMesh2 >( new WTriangleMesh2( m_splinePoints.size(), 2
            * m_renderpointsPerCol * m_renderpointsPerRow ) );

    for ( std::vector< std::vector< double > >::iterator posIt = m_splinePoints.begin(); posIt != m_splinePoints.end(); posIt++ )
    {
        newMesh->addVertex( ( *posIt )[0], ( *posIt )[1], ( *posIt )[2] );
    }

    for ( int z = 0; z < m_renderpointsPerCol - 1; z++ )
    {
        for ( int x = 0; x < m_renderpointsPerRow - 1; x++ )
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

    return m_tMesh;
}

std::vector< std::vector< double > > WSurface::getSplinePoints()
{
    return m_splinePoints;
}

void WSurface::setSetSampleRate( float r )
{
    m_sampleRateT = m_sampleRateU = r;
    execute();
}

