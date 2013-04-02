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

#include <list>
#include <string>
#include <vector>

#include <osg/Geode>

#include "../common/math/linearAlgebra/WPosition.h"
#include "../common/math/linearAlgebra/WVectorFixed.h"
#include "../common/WAssert.h"
#include "../common/WColor.h"
#include "../common/WTransferable.h"


/**
 * Triangle mesh data structure allowing for convenient access of the elements.
 */
class WTriangleMesh  : public WTransferable
{
friend class WTriangleMeshTest;
public:
    /**
     * Shared pointer
     */
    typedef boost::shared_ptr< WTriangleMesh > SPtr;

    /**
     * Const shared pointer
     */
    typedef boost::shared_ptr< const WTriangleMesh > ConstSPtr;

    /**
     * constructor that already reserves space for a given number of triangles and vertexes
     *
     * \param vertNum
     * \param triangleNum
     */
    WTriangleMesh( size_t vertNum, size_t triangleNum );

    /**
     * Constructs a new mesh out of the given vertices and triangles.
     *
     * \param vertices Vec3Array storing all vertices
     * \param triangles Vector of consecutive vertex indices where each 3 IDs are a triangle starting at 0,1,2 for first triangle 3,4,5 for the second
     */
    WTriangleMesh( osg::ref_ptr< osg::Vec3Array > vertices, const std::vector< size_t >& triangles );

    /**
     * destructor
     */
    virtual ~WTriangleMesh();

    /**
     * Returns a prototype instantiated with the true type of the deriving class.
     *
     * \return the prototype.
     */
    static boost::shared_ptr< WPrototyped > getPrototype();

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
     * adds a vertex position to the mesh
     *
     * \param vert
     */
    void addVertex( osg::Vec3 vert );

    /**
     * adds a vertex position to the mesh
     *
     * \param x
     * \param y
     * \param z
     */
    void addVertex( float x, float y, float z );

    /**
     * adds a vertex position to the mesh
     *
     * \param vert
     */
    void addVertex( WPosition vert );

    /**
     * Adds a texture coordinate for the vertex.
     *
     * \param texCoord the texture coordinate
     */
    void addTextureCoordinate( osg::Vec3 texCoord );

    /**
     * Adds a texture coordinate for the vertex.
     *
     * \param x texture coordinate X
     * \param y texture coordinate Y
     * \param z texture coordinate Z
     */
    void addTextureCoordinate( float x, float y, float z );

    /**
     * adds a tringle to the mesh
     *
     * \param vert0 index of the first vertex
     * \param vert1 index of the second vertex
     * \param vert2 index of the third vertex
     */
    void addTriangle( size_t vert0, size_t vert1, size_t vert2 );

    /**
     * adds a tringle and its 3 vertexes  to the mesh
     *
     * \param vert0 position of the first vertex
     * \param vert1 position of the second vertex
     * \param vert2 position of the third vertex
     */
    void addTriangle( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 );

    /**
     * sets a vertex to a new position
     *
     * \param index
     * \param vert
     */
    void setVertex( size_t index, osg::Vec3 vert );

    /**
     * sets the normal for a given vertex
     *
     * \param index
     * \param normal
     */
    void setVertexNormal( size_t index, osg::Vec3 normal );

    /**
     * sets the normal for a given vertex
     *
     * \param index
     * \param normal
     */
    void setVertexNormal( size_t index, WPosition normal );

    /**
     * sets the color for a given vertex
     *
     * \param index
     * \param color
     */
    void setVertexColor( size_t index, osg::Vec4 color );

    /**
     * sets the color for a given triangle
     *
     * \param index
     * \param color
     */
    void setTriangleColor( size_t index, osg::Vec4 color );

    /**
     * Return triangle colors
     *
     * \return OSG Vec4 Array of triangle colors
     */
    osg::ref_ptr< osg::Vec4Array > getTriangleColors() const;

    /**
     * getter
     *
     * \return pointer to the vertex array
     */
    osg::ref_ptr< osg::Vec3Array > getVertexArray();

    /**
     * Returns a const reference pointer to the vertex array.
     *
     * \return vertex array
     */
    osg::ref_ptr< const osg::Vec3Array > getVertexArray() const;

    /**
     * Returns a reference pointer to the texture coordinate array.
     *
     * \return texture coordinate array
     */
    osg::ref_ptr< osg::Vec3Array > getTextureCoordinateArray();

    /**
     * Returns a const reference pointer to the texture coordinate array.
     *
     * \return texture coordinate array
     */
    osg::ref_ptr< const osg::Vec3Array > getTextureCoordinateArray() const;

    /**
     * getter
     *
     * \param forceRecalc
     * \return pointer to the vertex normal array
     */
    osg::ref_ptr< osg::Vec3Array > getVertexNormalArray( bool forceRecalc = false );

    /**
     * getter
     *
     * \return pointer to the vertex color array
     */
    osg::ref_ptr< osg::Vec4Array > getVertexColorArray();

    /**
     * Returns a const reference to the vertex ids of the triangles.
     *
     * \return The triangle vertex id list
     */
    const std::vector< size_t >& getTriangles() const;

    /**
     * getter
     *
     * \param forceRecalc
     * \return pointer to the triangle normal array
     */
    osg::ref_ptr< osg::Vec3Array > getTriangleNormalArray( bool forceRecalc = false );


    /**
     * getter
     *
     * \param index
     * \return vertex
     */
    osg::Vec3 getVertex( size_t index ) const;

    /**
     * getter
     *
     * \param index
     * \return color
     */
    osg::Vec4 getVertColor( size_t index ) const;

    /**
     * getter
     *
     * \param triId
     * \param vertNum
     * \return vertex
     */
    osg::Vec3 getTriVert( size_t triId, size_t vertNum );

    /**
     * getter
     *
     * \param index
     * \return normal
     */
    WVector3d getNormal( size_t index );

    /**
     * getter
     *
     * \return number of vertices in the mesh
     */
    size_t vertSize() const;

    /**
     * getter
     *
     * \return number of triangles in the mesh
     */
    size_t triangleSize() const;

    /**
     * performs a loop subdivision on the triangle mesh
     */
    void doLoopSubD();

    /**
     * returns the id of the first vertex of a triangle
     *
     * \param triId id of the triangle
     * \return id of the vertex
     */
    size_t getTriVertId0( size_t triId ) const;

    /**
     * returns the id of the second vertex of a triangle
     *
     * \param triId id of the triangle
     * \return id of the vertex
     */
    size_t getTriVertId1( size_t triId ) const;

    /**
     * return the id of the third vertex of a triangle
     *
     * \param triId id of the triangle
     * \return id of the vertex
     */
    size_t getTriVertId2( size_t triId ) const;

    /**
     *  adds a mesh to the existing, no check for duplicate vertexes is performed, an additional
     *  vector may be specified to move the mesh to add
     *
     *  \param mesh
     *  \param xOff
     *  \param yOff
     *  \param zOff
     */
    void addMesh( boost::shared_ptr<WTriangleMesh> mesh, float xOff = 0., float yOff = 0., float zOff = 0. );

    /**
     *  moves the entire mesh to a new postion
     *
     *  \param xOff
     *  \param yOff
     *  \param zOff
     */
    void translateMesh( float xOff, float yOff, float zOff );

    /**
     * multiplies the vertex vectors of the mesh with a given number
     *
     * \param zoom
     */
    void zoomMesh( float zoom );

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

    /**
     * Rescale the vertex colors so that the maximum of all r, g and b values is 1
     */
    void rescaleVertexColors();

protected:
    static boost::shared_ptr< WPrototyped > m_prototype; //!< The prototype as singleton.
private:
    /**
     * we don't allow the standard constructor
     */
    WTriangleMesh();

    /**
     * removes a vertex from the vertex array, if any triangles still index that vertex they will be
     * removed if forceRemoveTriangle is true
     *
     * \param index the index of the vertex to remove
     */
    void removeVertex( size_t index );

    /**
     * removes a triangle from the mesh
     *
     * \param index the triangle to remove
     */
    void removeTriangle( size_t index );

    /**
     * recalculates the vertex normals
     */
    void recalcVertNormals();

    /**
     * calculates a normal from the 3 points in space defining a triangle
     *
     * \param triangle
     *
     * \return the normal of the triangle
     */
    osg::Vec3 calcTriangleNormal( size_t triangle );

    /**
     * calculates a normal from the 3 points in space
     *
     * \param vert0 vertex 1
     * \param vert1 vertex 2
     * \param vert2 vertex 3
     *
     * \return the normal of the plane defined by these three points
     */
    osg::Vec3 calcNormal( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 );

    /**
     * updates the list for which vertexes appear in which triangle
     */
    void updateVertsInTriangles();

    /**
     * calculates neighbor information for triangles
     */
    void calcNeighbors();

    /**
     * returns the triangle index of a triangle neighboring a given edge of a vertex
     *
     * \param coVert1
     * \param coVert2
     * \param triangleNum
     *
     * \return the number of the neighboring triangle.
     */
    size_t getNeighbor( const size_t coVert1, const size_t coVert2, const size_t triangleNum );

    /**
     * higher level access function to the triangle vector, sets the first vertex of a triangle to
     * a given vertex id
     *
     * \param triId the id of the triangle to modify
     * \param vertId new id of the first vertex
     */
    void setTriVert0( size_t triId, size_t vertId );

    /**
     * higher level access function to the triangle vector, sets the second vertex of a triangle to
     * a given vertex id
     *
     * \param triId the id of the triangle to modify
     * \param vertId new id of the second vertex
     */
    void setTriVert1( size_t triId, size_t vertId );

    /**
     * higher level access function to the triangle vector, sets the third vertex of a triangle to
     * a given vertex id
     *
     * \param triId the id of the triangle to modify
     * \param vertId new id of the third vertex
     */
    void setTriVert2( size_t triId, size_t vertId );


    // the next functions are helper functions for the loop subdivision algorithm and exist only for that
    // purpose, for more information read http://research.microsoft.com/en-us/um/people/cloop/thesis.pdf


    /**
     * changes the vertex ids of a triangle
     *
     * \param triId
     * \param vertId1
     * \param vertId2
     * \param vertId3
     */
    void loopSetTriangle( size_t triId, size_t vertId1, size_t vertId2, size_t vertId3 );

    /**
     * erases a triangle from the vertexe's list of triangles it is part of
     *
     * \param triId
     * \param vertId
     */
    void loopEraseTriangleFromVertex( size_t triId, size_t vertId );

    /**
     * calculates the new position of a vertex depending on it's location in the grid and number of neighbors
     *
     * \param vertId the vertex id
     * \return new position in 3D space
     */
    osg::Vec3 loopCalcNewPosition( size_t vertId );

    /**
     * inserts the center triangle in a given triangle,
     *
     * \param triId the triangle id
     */
    void loopInsertCenterTriangle( size_t triId );

    /**
     * inserts the 3 corner triangles in a given triangle
     *
     * \param triId the triangle id
     */
    void loopInsertCornerTriangles( size_t triId );

    /**
     * calculates the vertex id for a given edge, inserts a new vertex of none exists yet
     *
     * \param triId the triangle id
     * \param edgeV1
     * \param edgeV2
     * \param V3
     * \return index of the vertex
     */
    size_t loopCalcEdgeVert( size_t triId, size_t edgeV1, size_t edgeV2, size_t V3 );

    /**
     * loop helper function
     * \param n
     * \return alpha
     */
    double loopGetAlpha( int n );

    /**
     * returns the id of the next vertex int he triangle
     *
     * \param triNum id of the triangle
     * \param vertNum id of the vertex
     * \return id of the next vertex
     */
    size_t loopGetNextVertex( size_t triNum, size_t vertNum );

    /**
     * returns the id of the third vertex of a triangle for two given vertexes
     *
     * \param coVert1
     * \param coVert2
     * \param triangleNum
     * \return id of the third vertex
     */
    size_t loopGetThirdVert( size_t coVert1, size_t coVert2, size_t triangleNum );


    size_t m_countVerts; //!< number of vertexes in the mesh

    size_t m_countTriangles; //!< number of triangles in the mesh

    bool m_meshDirty; //!< flag indicating a change took place which requires a recalculation of components

    bool m_neighborsCalculated; //!< flag indicating whether the neighbor information has been calculated yet

    osg::ref_ptr< osg::Vec3Array > m_verts; //!< array containing the vertices

    osg::ref_ptr< osg::Vec3Array > m_textureCoordinates; //!< array containing the texture coordinates

    osg::ref_ptr< osg::Vec3Array > m_vertNormals; //!< array containing the vertex normals

    osg::ref_ptr< osg::Vec4Array > m_vertColors; //!< array containing vertex colors

    std::vector< size_t > m_triangles; //!< array containing the triangles

    osg::ref_ptr< osg::Vec3Array > m_triangleNormals; //!< array containing the triangle normals

    osg::ref_ptr< osg::Vec4Array > m_triangleColors; //!< array containing the triangle colors

    // helper structures
    std::vector < std::vector< size_t > > m_vertexIsInTriangle; //!< for each vertex, list of triangles it is part of

    std::vector< std::vector< size_t > > m_triangleNeighbors; //!< edge neighbors for each triangle

    size_t m_numTriVerts; //!< stores the number of vertexes before the loop subdivion is run, needed by the loop algorithm

    size_t m_numTriFaces; //!< stores the number of triangles before the loop subdivion is run, needed by the loop algorithm
};

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

inline bool WTriangleMesh::operator==( const WTriangleMesh& rhs ) const
{
    return std::equal( m_verts->begin(), m_verts->end(), rhs.m_verts->begin() ) &&
           std::equal( m_triangles.begin(), m_triangles.end(), rhs.m_triangles.begin() );
}

inline void WTriangleMesh::addTextureCoordinate( osg::Vec3 texCoord )
{
    ( *m_textureCoordinates )[m_countVerts-1] = texCoord;
}

inline void WTriangleMesh::addTextureCoordinate( float x, float y, float z )
{
    addTextureCoordinate( osg::Vec3( x, y, z ) );
}

inline void WTriangleMesh::addVertex( osg::Vec3 vert )
{
    if( ( *m_verts ).size() == m_countVerts )
    {
        ( *m_verts ).resize( m_countVerts + 1 );
    }
    if( ( *m_textureCoordinates ).size() == m_countVerts )
    {
        ( *m_textureCoordinates ).resize( m_countVerts + 1 );
    }
    if( ( *m_vertColors ).size() == m_countVerts )
    {
        ( *m_vertColors ).resize( m_countVerts + 1 );
    }

    ( *m_verts )[m_countVerts] = vert;
    ++m_countVerts;
}

inline const std::string WTriangleMesh::getName() const
{
    return "WTriangleMesh";
}

inline const std::string WTriangleMesh::getDescription() const
{
    return "Triangle mesh data structure allowing for convenient access of the elements.";
}

inline void WTriangleMesh::setTriVert0( size_t triId, size_t vertId )
{
    WAssert( triId < m_countTriangles, "set tri vert 0: triangle id out of range" );
    WAssert( vertId < m_countVerts, "vertex id out of range" );
    m_triangles[ triId * 3 ] = vertId;
}

inline void WTriangleMesh::setTriVert1( size_t triId, size_t vertId )
{
    WAssert( triId < m_countTriangles, "set tri vert 1: triangle id out of range" );
    WAssert( vertId < m_countVerts, "vertex id out of range" );
    m_triangles[ triId * 3 + 1] = vertId;
}

inline void WTriangleMesh::setTriVert2( size_t triId, size_t vertId )
{
    WAssert( triId < m_countTriangles, "set tri vert 2: triangle id out of range" );
    WAssert( vertId < m_countVerts, "vertex id out of range" );
    m_triangles[ triId * 3 + 2] = vertId;
}

inline osg::Vec3 WTriangleMesh::getTriVert( size_t triId, size_t vertNum )
{
    WAssert( triId < m_countTriangles, "triangle id out of range" );
    return ( *m_verts )[ m_triangles[ triId * 3  + vertNum] ];
}

inline size_t WTriangleMesh::getTriVertId0( size_t triId ) const
{
    WAssert( triId < m_countTriangles, "get tri vert id 0: triangle id out of range" );
    return m_triangles[triId * 3];
}

inline size_t WTriangleMesh::getTriVertId1( size_t triId ) const
{
    WAssert( triId < m_countTriangles, "get tri vert id 1: triangle id out of range" );
    return m_triangles[triId * 3 + 1];
}

inline size_t WTriangleMesh::getTriVertId2( size_t triId ) const
{
    WAssert( triId < m_countTriangles, "get tri vert id 2: triangle id out of range" );
    return m_triangles[triId * 3 + 2];
}

inline void WTriangleMesh::setVertex( size_t index, osg::Vec3 vert )
{
    ( *m_verts )[index] = vert;
}

#endif  // WTRIANGLEMESH_H
