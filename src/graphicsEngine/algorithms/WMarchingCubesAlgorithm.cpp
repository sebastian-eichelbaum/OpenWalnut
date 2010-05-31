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

#include "WMarchingCubesAlgorithm.h"

WMarchingCubesAlgorithm::WMarchingCubesAlgorithm()
    : m_matrix( 4, 4 )
{
}

WPointXYZId WMarchingCubesAlgorithm::interpolate( double fX1, double fY1, double fZ1, double fX2, double fY2, double fZ2,
                                             double tVal1, double tVal2 )
{
    WPointXYZId interpolation;
    double mu;

    mu = static_cast<double>( ( m_tIsoLevel - tVal1 ) ) / ( tVal2 - tVal1 );
    interpolation.x = fX1 + mu * ( fX2 - fX1 );
    interpolation.y = fY1 + mu * ( fY2 - fY1 );
    interpolation.z = fZ1 + mu * ( fZ2 - fZ1 );
    interpolation.newID = 0;

    return interpolation;
}

int WMarchingCubesAlgorithm::getEdgeID( unsigned int nX, unsigned int nY, unsigned int nZ, unsigned int nEdgeNo )
{
    switch ( nEdgeNo )
    {
        case 0:
            return 3 * getVertexID( nX, nY, nZ ) + 1;
        case 1:
            return 3 * getVertexID( nX, nY + 1, nZ );
        case 2:
            return 3 * getVertexID( nX + 1, nY, nZ ) + 1;
        case 3:
            return 3 * getVertexID( nX, nY, nZ );
        case 4:
            return 3 * getVertexID( nX, nY, nZ + 1 ) + 1;
        case 5:
            return 3 * getVertexID( nX, nY + 1, nZ + 1 );
        case 6:
            return 3 * getVertexID( nX + 1, nY, nZ + 1 ) + 1;
        case 7:
            return 3 * getVertexID( nX, nY, nZ + 1 );
        case 8:
            return 3 * getVertexID( nX, nY, nZ ) + 2;
        case 9:
            return 3 * getVertexID( nX, nY + 1, nZ ) + 2;
        case 10:
            return 3 * getVertexID( nX + 1, nY + 1, nZ ) + 2;
        case 11:
            return 3 * getVertexID( nX + 1, nY, nZ ) + 2;
        default:
            // Invalid edge no.
            return -1;
    }
}

unsigned int WMarchingCubesAlgorithm::getVertexID( unsigned int nX, unsigned int nY, unsigned int nZ )
{
    return nZ * ( m_nCellsY + 1 ) * ( m_nCellsX + 1) + nY * ( m_nCellsX + 1 ) + nX;
}

void WMarchingCubesAlgorithm::transformPositions( ID2WPointXYZId* positions )
{
    for( ID2WPointXYZId::iterator it = positions->begin(); it != positions->end(); ++it )
    {
        wmath::WPosition pos = wmath::WPosition( it->second.x, it->second.y, it->second.z );

        std::vector< double > resultPos4D( 4 );
        resultPos4D[0] = m_matrix( 0, 0 ) * pos[0] + m_matrix( 0, 1 ) * pos[1] + m_matrix( 0, 2 ) * pos[2] + m_matrix( 0, 3 ) * 1;
        resultPos4D[1] = m_matrix( 1, 0 ) * pos[0] + m_matrix( 1, 1 ) * pos[1] + m_matrix( 1, 2 ) * pos[2] + m_matrix( 1, 3 ) * 1;
        resultPos4D[2] = m_matrix( 2, 0 ) * pos[0] + m_matrix( 2, 1 ) * pos[1] + m_matrix( 2, 2 ) * pos[2] + m_matrix( 2, 3 ) * 1;
        resultPos4D[3] = m_matrix( 3, 0 ) * pos[0] + m_matrix( 3, 1 ) * pos[1] + m_matrix( 3, 2 ) * pos[2] + m_matrix( 3, 3 ) * 1;

        it->second.x = resultPos4D[0] / resultPos4D[3];
        it->second.y = resultPos4D[1] / resultPos4D[3];
        it->second.z = resultPos4D[2] / resultPos4D[3];
    }
}
