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

#ifndef WTRIANGLEMESH_H
#define WTRIANGLEMESH_H

#include <vector>

#include "../../math/WPosition.h"

struct Triangle
{
    size_t pointID[3];
};

/**
 * Triangle mesh data structure allowing for convenient access of the elements.
 */
class WTriangleMesh
{
public:
    WTriangleMesh();
    ~WTriangleMesh();

    size_t getNumVertices() const
    {
        return m_vertices.size();
    }

    size_t getNumTriangles() const
    {
        return m_triangles.size();
    }

    /**
     * Clear all mesh information, i.e. do things like making
     * the vertices and triangles containers empty and reset counter variables.
     */
    void clearMesh();

    void resizeVertices( size_t size );
    void resizeTriangles( size_t size );

    /**
     * Add a new vertex at the position given by internal state variable
     * fastAddVertId. Each call increases fastAddVertId by one. Exercise care
     * when using this function because it depends on the state of the WTriangleMesh.
     */
    void fastAddVert( const wmath::WPosition& newVert );

    /**
     * \return the state of the variable telling fastAddVert where to insert the vertex.
     */
    size_t getFastAddVertId() const;

    /**
     * Add a new triangle at the position given by internal state variable
     * fastAddTriangleId. Each call increases fastAddTriangleId by one. Exercise care
     * when using this function because it depends on the state of the WTriangleMesh.
     */
    void fastAddTriangle( unsigned int vertA, unsigned int vertB, unsigned int vertC );

    /**
     * \return the state of the variable telling fastAddTriangle where to insert the triangle.
     */
    size_t getFastAddTriangleId() const;

    /**
     * \return position of id-th vertex.
     */
    wmath::WPosition getVertex( size_t id ) const
    {
        return m_vertices[id];
    }

    /**
     * \return global vertex id of the vertId-th vertex of the triId-th triangle.
     */
    size_t getTriangleVertexId( size_t triId, size_t vertId ) const
    {
        return m_triangles[triId].pointID[vertId];
    }


protected:
private:
    std::vector< wmath::WPosition > m_vertices;
    std::vector< Triangle > m_triangles;

    size_t m_fastAddVertId;
    size_t m_fastAddTriangleId;
};

#endif  // WTRIANGLEMESH_H
