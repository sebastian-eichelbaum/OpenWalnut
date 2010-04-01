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

#ifndef WTRIANGLEMESH2_H
#define WTRIANGLEMESH2_H

#include <vector>
#include <string>

#include <osg/Geode>

#include "../WColor.h"
#include "../WTransferable.h"
#include "../math/WVector3D.h"

/**
 * Triangle mesh data structure allowing for convenient access of the elements.
 */
class WTriangleMesh2  : public WTransferable
{
public:
    /**
     * constructor that already reserves space for a given number of triangles and vertexes
     *
     * \param vertNum
     * \param triangleNum
     */
    WTriangleMesh2( size_t vertNum, size_t triangleNum );

    /**
     * destructor
     */
    ~WTriangleMesh2();

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
    void addVertex( wmath::WPosition vert );

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
    void setVertexNormal( size_t index, wmath::WPosition normal );

    /**
     * sets the color for a given vertex
     *
     * \param index
     * \param color
     */
    void setVertexColor( size_t index, osg::Vec4 color );

    /**
     * sets the color for a given vertex
     *
     * \param index
     * \param color
     */
    void setVertexColor( size_t index, WColor color );

    /**
     * sets the color for a given triangle
     *
     * \param index
     * \param color
     */
    void setTriangleColor( size_t index, osg::Vec4 color );

    /**
     * sets the color for a given triangle
     *
     * \param index
     * \param color
     */
    void setTriangleColor( size_t index, WColor color );

    /**
     * getter
     *
     * \return pointer to the vertex array
     */
    osg::ref_ptr< osg::Vec3Array >getVertexArray();

    /**
     * getter
     *
     * \param forceRecalc
     * \return pointer to the vertex normal array
     */
    osg::ref_ptr< osg::Vec3Array >getVertexNormalArray( bool forceRecalc = false );

    /**
     * getter
     *
     * \return pointer to the vertex color array
     */
    osg::ref_ptr< osg::Vec4Array >getVertexColorArray();

    /**
     * getter
     *
     * \return the triangle list
     */
    std::vector< size_t >getTriangles();

    /**
     * getter
     *
     * \param index
     * \return vertex
     */
    osg::Vec3 getVertex( size_t index );

    /**
     * getter
     *
     * \param triangleIndex
     * \param vertNum
     * \return vertex
     */
    osg::Vec3 getVertex( size_t triangleIndex, size_t vertNum );

    /**
     * getter
     *
     * \param index
     * \return vertex
     */
    wmath::WPosition getVertexAsPosition( size_t index );

    /**
     * getter
     *
     * \param triangleIndex
     * \param vertNum
     * \return vertex
     */
    wmath::WPosition getVertexAsPosition( size_t triangleIndex, size_t vertNum );

    /**
     * getter
     *
     * \return number of vertexes in the mesh
     */
    size_t vertSize();

    /**
     * getter
     *
     * \return number of triangles in the mesh
     */
    size_t triangleSize();

protected:
    static boost::shared_ptr< WPrototyped > m_prototype; //!< The prototype as singleton.
private:
    /**
     * we don't allow the standard constructor
     */
    WTriangleMesh2();

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
     * removes any vertex that isn't indexed by a triangle
     */
    void cleanUpMesh();

    /**
     * recalculates the vertex normals
     */
    void recalcVertNormals();

    /**
     * calculates a normal from the 3 points in space defining a triangle
     *
     * \param triangle
     */
    osg::Vec3 calcTriangleNormal( size_t triangle );

    /**
     * calculates a normal from the 3 points in space
     *
     * \param vert0
     * \param vert1
     * \param vert2
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
     */
    size_t getNeighbor( const size_t coVert1, const size_t coVert2, const size_t triangleNum );



    size_t m_countVerts; //!< number of vertexes in the mesh

    size_t m_countTriangles; //!< number of triangles in the mesh

    bool m_meshDirty; //!< flag indicating a change took place which requires a recalculation of components

    bool m_neighborsCalculated; //!< flag indicating whether the neighbor information has been calculated yet

    osg::ref_ptr< osg::Vec3Array >m_verts; //!< array containing the vertexes

    osg::ref_ptr< osg::Vec3Array >m_vertNormals; //!< array containing the vertex normals

    osg::ref_ptr< osg::Vec4Array >m_vertColors; //!< array containing vertex colors

    std::vector< size_t >m_triangles; //!< array containing the triangles

    osg::ref_ptr< osg::Vec3Array >m_triangleNormals; //!< array containing the triangle normals

    osg::ref_ptr< osg::Vec4Array >m_triangleColors; //!< array containing the triangle colors

    // helper structures
    std::vector < std::vector< size_t > >m_vertexIsInTriangle; //!< for each vertex, list of triangles it is part of

    std::vector< std::vector< size_t > > m_triangleNeighbors; //!< edge neighbors for each triangle
};


inline void WTriangleMesh2::addVertex( osg::Vec3 vert )
{
    ( *m_verts )[m_countVerts++] = vert;
}

inline osg::Vec3 WTriangleMesh2::getVertex( size_t triangleIndex, size_t vertNum )
{
    return ( *m_verts )[ m_triangles[ triangleIndex * 3  + vertNum] ];
}

inline const std::string WTriangleMesh2::getName() const
{
    return "WTriangleMesh";
}

inline const std::string WTriangleMesh2::getDescription() const
{
    return "Triangle mesh data structure allowing for convenient access of the elements.";
}

#endif  // WTRIANGLEMESH2_H
