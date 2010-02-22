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

#include "WTriangleMesh.h"


WTriangleMesh::WTriangleMesh()
    : m_fastAddVertId( 0 ),
      m_fastAddTriangleId( 0 ),
      m_computedTriNormals( false ),
      m_triNormals( 0 ),
      m_computedVertNormals( false ),
      m_vertNormals( 0 )
{
}

WTriangleMesh::~WTriangleMesh()
{
    clearMesh();
}

void WTriangleMesh::clearMesh()
{
    m_vertices.clear();
    m_triangles.clear();
    m_fastAddVertId = 0;
    m_fastAddTriangleId = 0;
}

void WTriangleMesh::resizeVertices( size_t size )
{
    m_vertices.resize( size );
}

void WTriangleMesh::resizeTriangles( size_t size )
{
    m_triangles.resize( size );
}

void WTriangleMesh::fastAddVert( const wmath::WPosition& newVert )
{
    m_vertices[m_fastAddVertId] = newVert;
    ++m_fastAddVertId;
}

void WTriangleMesh::setVertices( const std::vector< wmath::WPosition >& vertices )
{
    m_fastAddVertId = vertices.size();
    m_vertices = vertices;
}

size_t WTriangleMesh::getFastAddVertId() const
{
    return m_fastAddVertId;
}

void WTriangleMesh::fastAddTriangle( unsigned int vertA, unsigned int vertB, unsigned int vertC )
{
    Triangle t = { { vertA, vertB, vertC } };  // NOLINT
    m_triangles[m_fastAddTriangleId] = t;
    ++m_fastAddTriangleId;
}

void WTriangleMesh::setTriangles( const std::vector< Triangle >& triangles )
{
    m_fastAddTriangleId = triangles.size();
    m_triangles = triangles;
}

size_t WTriangleMesh::getFastAddTriangleId() const
{
    return m_fastAddTriangleId;
}

std::vector< unsigned int > WTriangleMesh::getPositionTriangleNeighborsSlow( size_t i ) const
{
    std::vector< unsigned int > neighborIds( 0 );
    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        for( unsigned int j = 0; j < 3; ++j )
        {
            if( m_triangles[triId].pointID[j] == i )
            {
                neighborIds.push_back( triId );
            }
        }
    }
    return neighborIds;
}

wmath::WVector3D WTriangleMesh::getVertexNormalSlow( size_t i ) const
{
    std::vector< unsigned int > neighborIndices = getPositionTriangleNeighborsSlow( i );

    wmath::WVector3D normal;
    for( unsigned int triId = 0; triId < neighborIndices.size(); ++triId )
    {
        normal += getTriangleNormal( neighborIndices[triId] );
    }
    normal *= 1./neighborIndices.size();
    return normal;
}

wmath::WVector3D WTriangleMesh::getVertexNormal( size_t i ) const
{
    assert( m_computedVertNormals );
    return m_vertNormals[i];
}

wmath::WVector3D WTriangleMesh::getTriangleNormal( size_t i ) const
{
    Triangle tri = m_triangles[i];
    wmath::WVector3D v1 = m_vertices[tri.pointID[1]] - m_vertices[tri.pointID[0]];
    wmath::WVector3D v2 = m_vertices[tri.pointID[2]] - m_vertices[tri.pointID[0]];

    wmath::WVector3D tempNormal = v1.crossProduct( v2 );

    if( tempNormal.norm() < 1e-7 )
    {
        // If the morm of the vector is below a certain threshold
        // the triangle is either very small, or its edges are
        // congruent (which also means that the triangle is very
        // small or zero) the triangle will probably not be visible.
        // Thus we set its normal to zero.
        tempNormal = wmath::WVector3D( 0, 0, 0 );
    }
    else
    {
        tempNormal.normalize();
    }

    return tempNormal;
}

void WTriangleMesh::computeTriNormals()
{
    if( m_computedTriNormals )
    {
        return;
    }

    m_triNormals.reserve( m_triangles.size() );
    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        m_triNormals.push_back( getTriangleNormal( triId ) );
    }

    m_computedTriNormals = true;
}

void WTriangleMesh::computeVertNormals()
{
    if( m_computedVertNormals )
    {
        return;
    }

    computeTriNormals();
    assert( m_computedTriNormals );

    //build neighborhood
    std::vector< std::vector< unsigned int > > neighborIds( m_vertices.size() );

    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        for( unsigned int j = 0; j < 3; ++j )
        {
            neighborIds[m_triangles[triId].pointID[j]].push_back( triId );
        }
    }

    m_vertNormals.reserve( m_vertices.size() );

    for( unsigned int vertId = 0; vertId < m_vertices.size(); ++vertId )
    {
        wmath::WVector3D tempNormal( 0, 0, 0 );
        for( unsigned int neighId = 0; neighId < neighborIds[vertId].size(); ++neighId )
        {
            tempNormal += m_triNormals[neighborIds[vertId][neighId]];
        }
        tempNormal *= 1./neighborIds.size();
        if( tempNormal.norm() < 1e-7 )
        {
            // If the morm of the vector is below a certain threshold
            // the triangle is either very small, or its edges are
            // congruent (which also means that the triangle is very
            // small or zero) the triangle will probably not be visible.
            // Thus we set its normal to zero.
            tempNormal = wmath::WVector3D( 0, 0, 0 );
        }
        else
        {
            tempNormal.normalize();
        }

        m_vertNormals.push_back( tempNormal );
    }

    m_computedVertNormals = true;
}
