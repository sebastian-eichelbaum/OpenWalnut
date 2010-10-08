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

#include "WMarchingLegoAlgorithm.h"

WMarchingLegoAlgorithm::WMarchingLegoAlgorithm()
    : m_matrix( 4, 4 )
{
}

WMarchingLegoAlgorithm::~WMarchingLegoAlgorithm()
{
}

void WMarchingLegoAlgorithm::addSurface( size_t x, size_t y, size_t z, size_t surface )
{
    WMLPointXYZId pt1;
    WMLPointXYZId pt2;
    WMLPointXYZId pt3;
    WMLPointXYZId pt4;

    pt1.newID = 0;
    pt2.newID = 0;
    pt3.newID = 0;
    pt4.newID = 0;

    switch ( surface )
    {
        case 1:
        {
            pt1.x = x;
            pt1.y = y;
            pt1.z = z;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x;
            pt2.y = y + 1;
            pt2.z = z;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x;
            pt3.y = y + 1;
            pt3.z = z + 1;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x;
            pt4.y = y;
            pt4.z = z + 1;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        case 2:
        {
            pt1.x = x + 1;
            pt1.y = y;
            pt1.z = z;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x + 1;
            pt2.y = y;
            pt2.z = z + 1;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x + 1;
            pt3.y = y + 1;
            pt3.z = z + 1;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x + 1;
            pt4.y = y + 1;
            pt4.z = z;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        case 3:
        {
            pt1.x = x;
            pt1.y = y;
            pt1.z = z;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x;
            pt2.y = y;
            pt2.z = z + 1;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x + 1;
            pt3.y = y;
            pt3.z = z + 1;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x + 1;
            pt4.y = y;
            pt4.z = z;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        case 4:
        {
            pt1.x = x;
            pt1.y = y + 1;
            pt1.z = z;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x + 1;
            pt2.y = y + 1;
            pt2.z = z;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x + 1;
            pt3.y = y + 1;
            pt3.z = z + 1;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x;
            pt4.y = y + 1;
            pt4.z = z + 1;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        case 5:
        {
            pt1.x = x;
            pt1.y = y;
            pt1.z = z;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x + 1;
            pt2.y = y;
            pt2.z = z;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x + 1;
            pt3.y = y + 1;
            pt3.z = z;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x;
            pt4.y = y + 1;
            pt4.z = z;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        case 6:
        {
            pt1.x = x;
            pt1.y = y;
            pt1.z = z + 1;
            unsigned int id1 = getVertexID( pt1.x , pt1.y, pt1.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id1, pt1 ) );

            pt2.x = x;
            pt2.y = y + 1;
            pt2.z = z + 1;
            unsigned int id2 = getVertexID( pt2.x , pt2.y, pt2.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id2, pt2 ) );

            pt3.x = x + 1;
            pt3.y = y + 1;
            pt3.z = z + 1;
            unsigned int id3 = getVertexID( pt3.x , pt3.y, pt3.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id3, pt3 ) );

            pt4.x = x + 1;
            pt4.y = y;
            pt4.z = z + 1;
            unsigned int id4 = getVertexID( pt4.x , pt4.y, pt4.z );
            m_idToVertices.insert( ID2WMLPointXYZId::value_type( id4, pt4 ) );

            WMLTriangle triangle1;
            triangle1.pointID[0] = id1;
            triangle1.pointID[1] = id2;
            triangle1.pointID[2] = id3;
            WMLTriangle triangle2;
            triangle2.pointID[0] = id3;
            triangle2.pointID[1] = id4;
            triangle2.pointID[2] = id1;
            m_trivecTriangles.push_back( triangle1 );
            m_trivecTriangles.push_back( triangle2 );
            break;
        }
        default:
            break;
    }
}

size_t WMarchingLegoAlgorithm::getVertexID( size_t nX, size_t nY, size_t nZ )
{
    return nZ * ( m_nCellsY + 1 ) * ( m_nCellsX + 1) + nY * ( m_nCellsX + 1 ) + nX;
}
