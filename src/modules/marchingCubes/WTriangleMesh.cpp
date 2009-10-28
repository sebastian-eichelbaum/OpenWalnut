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

#include "WTriangleMesh.h"


WTriangleMesh::WTriangleMesh()
    : m_fastAddVertId( 0 ),
      m_fastAddTriangleId( 0 )
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

size_t WTriangleMesh::getFastAddTriangleId() const
{
    return m_fastAddTriangleId;
}
