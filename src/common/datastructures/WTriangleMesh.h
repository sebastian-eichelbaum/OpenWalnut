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

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../math/WPosition.h"
#include "../math/WVector3D.h"
#include "../WTransferable.h"



/**
 * A triangle consisting of 3 id of vertices that represent its corners
 */
struct Triangle
{
    size_t pointID[3]; //!< the ID of the vertices representing the triangle corners

    /**
     * Compares two triangles. Triangles having the same position in different order are considered different since it maybe to expensive.
     *
     * \param rhs The other triangle.
     *
     * \return True if and only if both triangles having the same positions in same order.
     */
    bool operator==( const Triangle& rhs ) const;
};

inline bool Triangle::operator==( const Triangle& rhs ) const
{
    return pointID[0] == rhs.pointID[0] && pointID[1] == rhs.pointID[1] && pointID[2] == rhs.pointID[2];
}

class WTriangleMesh;

/**
 * TriangleMesh utils
 */
namespace tm_utils
{
    /**
     * Decompose the given mesh into connected components.
     *
     * \param mesh The triangle mesh to decompose
     *
     * \return List of components where each of them is a WTriangleMesh again.
     */
    boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > componentDecomposition( const WTriangleMesh& mesh );

    /**
     * Prints for each mesh \#vertices and \#triangles, as well as each triangle with its positions. No point IDs are printed.
     *
     * \param os Output stream to print on.
     * \param rhs The mesh instance.
     *
     * \return The output stream again for further usage.
     */
    std::ostream& operator<<( std::ostream& os, const WTriangleMesh& rhs );
}

/**
 * Triangle mesh data structure allowing for convenient access of the elements.
 */
class WTriangleMesh : public WTransferable
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
    virtual ~WTriangleMesh();

    /**
     * Gets the name of this prototype.
     *
     * \return the name.
     */
    virtual const std::string getName() const;

    /**
     * Gets the description for this prototype.
     *
     * \return the description
     */
    virtual const std::string getDescription() const;

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

    /**
     * \return Size of the vertex container.
     */
    size_t getNumVertices() const;

    /**
     * \return Size of the triangle container.
     */
    size_t getNumTriangles() const;

    /**
     * Clear all mesh information, i.e. do things like making
     * the vertices and triangles containers empty and reset counter variables.
     */
    void clearMesh();

    /**
     * Changes the size of the vertex container.
     * \param size the new size of the vertex container
     */
    void resizeVertices( size_t size );

    /**
     * Changes the size of the triangle container.
     * \param size the new size of the trinagle container
     */
    void resizeTriangles( size_t size );

    /**
     * Add a new vertex at the position given by internal state variable
     * fastAddVertId. Each call increases fastAddVertId by one. Exercise care
     * when using this function because it depends on the state of the WTriangleMesh.
     * \param newVert The vertex that will be added.
     */
    void fastAddVert( const wmath::WPosition& newVert );

    /**
     * Set vertices to given vector of vertex positions. Be aware that this includes
     * a complete copy operation.
     * \param vertices A vector of position that will be the vertices of the grid.
     */
    void setVertices( const std::vector< wmath::WPosition >& vertices );

    /**
     * Get vector of the vertex positions.
     *
     * \return const reference to the vertices
     */
    const std::vector< wmath::WPosition >& getVertices() const;

    /**
     * Get the vector of triangles (vertex IDs).
     *
     * \return const reference to the triangles
     */
    const std::vector< Triangle >& getTriangles() const;

    /**
     * \return the state of the variable telling fastAddVert where to insert the vertex.
     */
    size_t getFastAddVertId() const;

    /**
     * Add a new triangle at the position given by internal state variable
     * fastAddTriangleId. Each call increases fastAddTriangleId by one. Exercise care
     * when using this function because it depends on the state of the WTriangleMesh.
     * \param vertA Id of vertex in vertex array that will be the first corner of the new triangle
     * \param vertB Id of vertex in vertex array that will be the second corner of the new triangle
     * \param vertC Id of vertex in vertex array that will be the third corner of the new triangle
     */
    void fastAddTriangle( unsigned int vertA, unsigned int vertB, unsigned int vertC );

    /**
     * Set triangles to given vector of vertex index triples. Be aware that this includes
     * a complete copy operation.
     * \param triangles Vector of triangles (i.e. encapsulated vertex IDs) that will be the mesh
     */
    void setTriangles( const std::vector< Triangle >& triangles );

    /**
     * \return the state of the variable telling fastAddTriangle where to insert the triangle.
     */
    size_t getFastAddTriangleId() const;

    /**
     * \param id ID of vertex
     * \return position of id-th vertex.
     */
    wmath::WPosition getVertex( size_t id ) const;

    /**
     * \param triId id of the triangle in triangle list
     * \param vertId id of the vertex in the triangle's vertex list
     * \return global vertex id of the vertId-th vertex of the triId-th triangle.
     */
    size_t getTriangleVertexId( size_t triId, size_t vertId ) const;

    /**
     * \param i ID of the triangle the normal will be computed for.
     * \return normal of i-th trinagle. Will be computed in the moment of the call.
     * i.e. with no memory overhead but possibly slow.
     * Normal will be of length 1.
     */
    wmath::WVector3D getTriangleNormal( size_t i ) const;

    /**
     * Get the indices of the triangles surrounding the i-th position.
     * This function is slow but const and memory efficient.
     * Normal will be of length 1.
     * \param i ID of position.
     */
    std::vector< unsigned int > getPositionTriangleNeighborsSlow( size_t i ) const;

    /**
     * Get an approximated normal for point with index i.
     * The normal is a mean of the the normals of the surrounding triangles.
     * This function is slow but const and memory efficient.
     * \param i ID of vertex.
     */
    wmath::WVector3D getVertexNormalSlow( size_t i ) const;

    /**
     * Get an approximated normal for point with index i.
     * The normal is a mean of the the normals of the surrounding triangles.
     * This function is fast but requires the normals to be precomputed
     * and stored by computeVertNormals().
     * Normal will be of length 1.
     * \param i ID of vertex.
     */
    wmath::WVector3D getVertexNormal( size_t i ) const;

    /**
     * Compute the normals for all triangles and store them.
     * Normals will be of length 1.
     */
    void computeTriNormals();

    /**
     * Approximate the normals for all vertices by averaging the normals
     * of all neighboring triangles and store them.
     * This involves a call of computeTriNormals() first.
     * Normals will be of length 1.
     */
    void computeVertNormals();

    /**
     * Checks if two meshes are exactly the same. Same number of triangles, and
     * points, and indices as well as same ordering. Keep in mind different
     * ordering might result in the same structure but is considered different
     * here.
     *
     * \param rhs The other mesh to compare with
     *
     * \return True if and only if both: vertices and triangles are exactly the same.
     */
    bool operator==( const WTriangleMesh& rhs ) const;

protected:
    static boost::shared_ptr< WPrototyped > m_prototype; //!< The prototype as singleton.

private:
    std::vector< wmath::WPosition > m_vertices;  //!< All vertices of the mesh.
    std::vector< Triangle > m_triangles;  //!< All triangles of the mesh, given as groups of vertex indices.

    size_t m_fastAddVertId;  //!< tells fastAddVert where to insert the vertex.
    size_t m_fastAddTriangleId;  //!< tells fastAddTriangle where to insert the triangle

    bool m_computedTriNormals;  //!< Have the triangle normals been computed?.
    std::vector< wmath::WVector3D > m_triNormals;  //!< Normals for all Triangles.
    bool m_computedVertNormals;  //!< Have the vertex normals been approximated?.
    std::vector< wmath::WVector3D > m_vertNormals;  //!< Approximated normals for all vertices.
};

inline size_t WTriangleMesh::getNumVertices() const
{
    return m_vertices.size();
}

inline size_t WTriangleMesh::getNumTriangles() const
{
    return m_triangles.size();
}

inline  wmath::WPosition WTriangleMesh::getVertex( size_t id ) const
{
    return m_vertices[id];
}

inline size_t WTriangleMesh::getTriangleVertexId( size_t triId, size_t vertId ) const
{
    return m_triangles[triId].pointID[vertId];
}

inline const std::string WTriangleMesh::getName() const
{
    return "WTriangleMesh";
}

inline const std::string WTriangleMesh::getDescription() const
{
    return "Triangle mesh data structure allowing for convenient access of the elements.";
}

inline bool WTriangleMesh::operator==( const WTriangleMesh& rhs ) const
{
    return std::equal( m_vertices.begin(), m_vertices.end(), rhs.m_vertices.begin() ) &&
           std::equal( m_triangles.begin(), m_triangles.end(), rhs.m_triangles.begin() );
}

#endif  // WTRIANGLEMESH_H
