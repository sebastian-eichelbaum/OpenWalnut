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
#include "../../math/WVector3D.h"

struct Triangle
{
    size_t pointID[3];
};

/**
 * Triangle mesh data structure allowing for convenient access of the elements.
 */
class WTriangleMesh
{
/**
 * Only UnitTests may be friends.
 */
friend class WTriangleMeshTest;

public:
    /**
     * Initializes some members.
     */
    WTriangleMesh();

    /**
     * Clears and destroys mesh.
     */
    ~WTriangleMesh();

    /**
     * \return Size of the vertex container.
     */
    size_t getNumVertices() const
    {
        return m_vertices.size();
    }

    /**
     * \return Size of the triangle container.
     */
    size_t getNumTriangles() const
    {
        return m_triangles.size();
    }

    /**
     * Clear all mesh information, i.e. do things like making
     * the vertices and triangles containers empty and reset counter variables.
     */
    void clearMesh();

    /**
     * Changes the size of the vertex container.
     */
    void resizeVertices( size_t size );

    /**
     * Changes the size of the triangle container.
     */
    void resizeTriangles( size_t size );

    /**
     * Add a new vertex at the position given by internal state variable
     * fastAddVertId. Each call increases fastAddVertId by one. Exercise care
     * when using this function because it depends on the state of the WTriangleMesh.
     */
    void fastAddVert( const wmath::WPosition& newVert );

    /**
     * Set vertices to given vector of vertex positions. Be aware that this includes
     * a complete copy operation.
     */
    void setVertices( const std::vector< wmath::WPosition >& vertices );

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
     * Set triangles to given vector of vertex index triples. Be aware that this includes
     * a complete copy operation.
     */
    void setTriangles( const std::vector< Triangle >& triangles );

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

    /**
     * \return normal of i-th trinagle. Will be computed in the moment of the call.
     * i.e. with no memory overhead but possibly slow.
     */
    wmath::WVector3D getTriangleNormal( size_t i ) const;


protected:
private:
    std::vector< wmath::WPosition > m_vertices;  //!< All vertices of the mesh.
    std::vector< Triangle > m_triangles;  //!< All triangles of the mesh, given as groups of vertex indices.

    size_t m_fastAddVertId;  //!< tells fastAddVert where to insert the vertex.
    size_t m_fastAddTriangleId;  //!< tells fastAddTriangle where to insert the triangle
};

#endif  // WTRIANGLEMESH_H
