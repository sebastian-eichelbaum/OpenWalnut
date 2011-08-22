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

#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include <osg/io_utils>

#include "../common/datastructures/WUnionFind.h"
#include "WTriangleMesh.h"

// init _static_ member variable and provide a linker reference to it
boost::shared_ptr< WPrototyped > WTriangleMesh::m_prototype = boost::shared_ptr< WPrototyped >();

boost::shared_ptr< WPrototyped > WTriangleMesh::getPrototype()
{
    if( !m_prototype )
    {
         m_prototype = boost::shared_ptr< WPrototyped >( new WTriangleMesh( 0, 0 ) );
    }
    return m_prototype;
}

/**
 * constructor that already reserves space for a given number of triangles and vertexes
 */
WTriangleMesh::WTriangleMesh( size_t vertNum, size_t triangleNum )
    : m_countVerts( 0 ),
      m_countTriangles( 0 ),
      m_meshDirty( true ),
      m_neighborsCalculated( false )
{
    m_verts = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_textureCoordinates = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( vertNum ) );

    m_triangles.resize( triangleNum * 3 );
    m_triangleNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( triangleNum ) );
    m_triangleColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( triangleNum ) );
}

WTriangleMesh::WTriangleMesh( osg::ref_ptr< osg::Vec3Array > vertices, const std::vector< size_t >& triangles )
    : m_countVerts( vertices->size() ),
      m_countTriangles( triangles.size() / 3 ),
      m_meshDirty( true ),
      m_neighborsCalculated( false ),
      m_verts( vertices ),
      m_textureCoordinates( new osg::Vec3Array( vertices->size() ) ),
      m_vertNormals( new osg::Vec3Array( vertices->size() ) ),
      m_vertColors( new osg::Vec4Array( vertices->size() ) ),
      m_triangles( triangles ),
      m_triangleNormals( new osg::Vec3Array( triangles.size() / 3 ) ),
      m_triangleColors( new osg::Vec4Array( triangles.size() / 3 ) )
{
    WAssert( triangles.size() % 3 == 0, "Invalid triangle vector, having an invalid size (not divideable by 3)" );
}

WTriangleMesh::~WTriangleMesh()
{
}

void WTriangleMesh::addVertex( float x, float y, float z )
{
    addVertex( osg::Vec3( x, y, z ) );
}

void WTriangleMesh::addVertex( WPosition vert )
{
    addVertex( osg::Vec3( vert[0], vert[1], vert[2] ) );
}

void WTriangleMesh::addTriangle( size_t vert0, size_t vert1, size_t vert2 )
{
    if( m_triangles.size() == m_countTriangles * 3 )
    {
        m_triangles.resize( m_countTriangles * 3 + 3 );
    }
    m_triangles[ m_countTriangles * 3 ] = vert0;
    m_triangles[ m_countTriangles * 3 + 1 ] = vert1;
    m_triangles[ m_countTriangles * 3 + 2 ] = vert2;
    ++m_countTriangles;
}

void WTriangleMesh::addTriangle( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 )
{
    addVertex( vert0 );
    addVertex( vert1 );
    addVertex( vert2 );
    addTriangle( m_countVerts - 3, m_countVerts - 2, m_countVerts - 1 );
}

void WTriangleMesh::setVertexNormal( size_t index, osg::Vec3 normal )
{
    WAssert( index < m_countVerts, "set vertex normal: index out of range" );
    ( *m_vertNormals )[index] = normal;
}

void WTriangleMesh::setVertexNormal( size_t index, WPosition normal )
{
    WAssert( index < m_countVerts, "set vertex normal: index out of range" );
    setVertexNormal( index, osg::Vec3( normal[0], normal[1], normal[2] ) );
}

void WTriangleMesh::setVertexColor( size_t index, osg::Vec4 color )
{
    WAssert( index < m_countVerts, "set vertex color: index out of range" );
    ( *m_vertColors )[index] = color;
}

void WTriangleMesh::setTriangleColor( size_t index, osg::Vec4 color )
{
    WAssert( index < m_countTriangles, "set triangle color: index out of range" );
    ( *m_triangleColors )[index] = color;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh::getVertexArray()
{
    return m_verts;
}

osg::ref_ptr< const osg::Vec3Array >WTriangleMesh::getVertexArray() const
{
    return m_verts;
}

osg::ref_ptr< osg::Vec3Array > WTriangleMesh::getTextureCoordinateArray()
{
    return m_textureCoordinates;
}

osg::ref_ptr< const osg::Vec3Array > WTriangleMesh::getTextureCoordinateArray() const
{
    return m_textureCoordinates;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh::getVertexNormalArray( bool forceRecalc )
{
    if( forceRecalc || m_meshDirty )
    {
        recalcVertNormals();
    }
    return m_vertNormals;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh::getTriangleNormalArray( bool forceRecalc )
{
    if( forceRecalc || m_meshDirty )
    {
        recalcVertNormals();
    }
    return m_triangleNormals;
}


osg::ref_ptr< osg::Vec4Array >WTriangleMesh::getVertexColorArray()
{
    return m_vertColors;
}

const std::vector< size_t >& WTriangleMesh::getTriangles() const
{
    return m_triangles;
}

osg::Vec3 WTriangleMesh::getVertex( size_t index ) const
{
    WAssert( index < m_countVerts, "get vertex: index out of range" );
    return ( *m_verts )[index];
}

osg::Vec4 WTriangleMesh::getVertColor( size_t index ) const
{
    WAssert( index < m_countVerts, "get vertex color: index out of range" );
    return ( *m_vertColors )[index];
}

WVector3d WTriangleMesh::getNormal( size_t index ) const
{
    WAssert( index < m_countVerts, "get normal as position: index out of range" );
    return WPosition( ( *m_vertNormals )[index][0], ( *m_vertNormals )[index][1], ( *m_vertNormals )[index][2] );
}

void WTriangleMesh::removeVertex( size_t index )
{
    WAssert( index < m_countVerts, "remove vertex: index out of range" );
    if( m_vertexIsInTriangle[ index ].size() > 0 )
    {
        return;
    }
    ( *m_verts ).erase( ( *m_verts ).begin() + index );

    for( size_t i = 0; i < m_countTriangles * 3; ++i )
    {
        if( m_triangles[ i ] > index )
        {
            --m_triangles[ i ];
        }
    }
    m_meshDirty = true;
}

void WTriangleMesh::removeTriangle( size_t index )
{
    WAssert( index < m_countTriangles, "remove triangle: index out of range" );
    m_triangles.erase( m_triangles.begin() + index * 3, m_triangles.begin() + index * 3 + 3 );
    m_meshDirty = true;
}

void WTriangleMesh::cleanUpMesh()
{
    //TODO(schurade): implement this
}

void WTriangleMesh::recalcVertNormals()
{
    updateVertsInTriangles();

    ( *m_vertNormals ).resize( m_countVerts );
    ( *m_triangleNormals ).resize( m_countTriangles );

    for( size_t i = 0; i < m_countTriangles; ++i )
    {
        ( *m_triangleNormals )[i] = calcTriangleNormal( i );
    }

    for( size_t vertId = 0; vertId < m_countVerts; ++vertId )
    {
        osg::Vec3 tempNormal( 0.0, 0.0, 0.0 );
        for( size_t neighbour = 0; neighbour < m_vertexIsInTriangle[vertId].size(); ++neighbour )
        {
            tempNormal += ( *m_triangleNormals )[ m_vertexIsInTriangle[vertId][neighbour] ];
        }
        tempNormal *= 1./m_vertexIsInTriangle[vertId].size();

        tempNormal.normalize();
        ( *m_vertNormals )[vertId] = tempNormal;
    }

    m_meshDirty = false;
}

void WTriangleMesh::updateVertsInTriangles()
{
    m_vertexIsInTriangle.clear();
    std::vector< size_t >v;
    m_vertexIsInTriangle.resize( ( *m_verts ).size(), v );

    for( size_t i = 0; i < m_countTriangles; ++i )
    {
        m_vertexIsInTriangle[ getTriVertId0( i ) ].push_back( i );
        m_vertexIsInTriangle[ getTriVertId1( i ) ].push_back( i );
        m_vertexIsInTriangle[ getTriVertId2( i ) ].push_back( i );
    }
}

osg::Vec3 WTriangleMesh::calcTriangleNormal( size_t triangle )
{
    osg::Vec3 v1( getTriVert( triangle, 1 ) - getTriVert( triangle, 0 ) );
    osg::Vec3 v2( getTriVert( triangle, 2 ) - getTriVert( triangle, 0 ) );

    osg::Vec3 tempNormal( 0, 0, 0 );

    tempNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    tempNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];
    tempNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];

    tempNormal.normalize();

    return tempNormal;
}

osg::Vec3 WTriangleMesh::calcNormal( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 )
{
    osg::Vec3 v1( vert1 - vert0 );
    osg::Vec3 v2( vert2 - vert0 );

    osg::Vec3 tempNormal( 0, 0, 0 );

    tempNormal[0] = v1[1] * v2[2] - v1[2] * v2[1];
    tempNormal[1] = v1[2] * v2[0] - v1[0] * v2[2];
    tempNormal[2] = v1[0] * v2[1] - v1[1] * v2[0];

    tempNormal.normalize();

    return tempNormal;
}

size_t WTriangleMesh::vertSize() const
{
    return m_countVerts;
}

size_t WTriangleMesh::triangleSize() const
{
    return m_countTriangles;
}

void WTriangleMesh::calcNeighbors()
{
    std::vector<size_t> v( 3, -1 );
    m_triangleNeighbors.resize( ( *m_triangleNormals ).size(), v );

    for( size_t triId = 0; triId < m_countTriangles; ++triId )
    {
        size_t coVert0 = getTriVertId0( triId );
        size_t coVert1 = getTriVertId1( triId );
        size_t coVert2 = getTriVertId2( triId );

        m_triangleNeighbors[triId][0] = getNeighbor( coVert0, coVert1, triId );
        m_triangleNeighbors[triId][1] = getNeighbor( coVert1, coVert2, triId );
        m_triangleNeighbors[triId][2] = getNeighbor( coVert2, coVert0, triId );
    }
    m_neighborsCalculated = true;
}

size_t WTriangleMesh::getNeighbor( const size_t coVert1, const size_t coVert2, const size_t triangleNum )
{
    std::vector< size_t > candidates = m_vertexIsInTriangle[coVert1];
    std::vector< size_t > compares = m_vertexIsInTriangle[coVert2];

    for( size_t i = 0; i < candidates.size(); ++i )
    {
        for( size_t k = 0; k < compares.size(); ++k )
        {
            if( ( candidates[i] != triangleNum ) && ( candidates[i] == compares[k] ) )
            {
                return candidates[i];
            }
        }
    }
    return triangleNum;
}

void WTriangleMesh::doLoopSubD()
{
    m_numTriVerts = m_countVerts;
    m_numTriFaces = m_countTriangles;

    ( *m_verts ).resize( m_numTriVerts * 4 );
    m_triangles.resize( m_numTriFaces * 4 * 3 );

    updateVertsInTriangles();

    osg::Vec3* newVertexPositions = new osg::Vec3[m_numTriVerts];

    //std::cout << "Loop subdivision pass 1" << std::endl;
    for( size_t i = 0; i < m_numTriVerts; ++i )
    {
        newVertexPositions[i] = loopCalcNewPosition( i );
    }

    //std::cout << "Loop subdivision pass 2" << std::endl;
    for( size_t i = 0; i < m_numTriFaces; ++i )
    {
        loopInsertCenterTriangle( i );
    }
    ( *m_verts ).resize( m_countVerts );
    std::vector< size_t >v;
    m_vertexIsInTriangle.resize( ( *m_verts ).size(), v );

    //std::cout << "Loop subdivision pass 3" << std::endl;
    for( size_t i = 0; i < m_numTriFaces; ++i )
    {
        loopInsertCornerTriangles( i );
    }

    //std::cout << "Loop subdivision pass 4" << std::endl;
    for( size_t i = 0; i < m_numTriVerts; ++i )
    {
        ( *m_verts )[i] = newVertexPositions[i];
    }

    delete[] newVertexPositions;

    m_vertNormals->resize( m_verts->size() );
    m_vertColors->resize( m_verts->size() );
    m_triangleColors->resize( m_triangles.size() / 3 );

    m_meshDirty = true;
}


osg::Vec3 WTriangleMesh::loopCalcNewPosition( size_t vertId )
{
    std::vector< size_t > starP = m_vertexIsInTriangle[vertId];
    int starSize = starP.size();

    osg::Vec3 oldPos = getVertex( vertId );
    double alpha = loopGetAlpha( starSize );

    double scale = 1.0 - ( static_cast<double>( starSize ) * alpha );
    oldPos *= scale;

    osg::Vec3 newPos;
    int edgeV = 0;
    for( int i = 0; i < starSize; i++ )
    {
        edgeV = loopGetNextVertex( starP[i], vertId );
        osg::Vec3 translate = getVertex( edgeV );
        newPos += translate;
    }
    newPos *= alpha;

    return oldPos + newPos;
}

void WTriangleMesh::loopInsertCenterTriangle( size_t triId )
{
    size_t edgeVerts[3];

    edgeVerts[0] = loopCalcEdgeVert( triId, getTriVertId0( triId ), getTriVertId1( triId ), getTriVertId2( triId ) );
    edgeVerts[1] = loopCalcEdgeVert( triId, getTriVertId1( triId ), getTriVertId2( triId ), getTriVertId0( triId ) );
    edgeVerts[2] = loopCalcEdgeVert( triId, getTriVertId2( triId ), getTriVertId0( triId ), getTriVertId1( triId ) );

    addTriangle( edgeVerts[0], edgeVerts[1], edgeVerts[2] );
}


size_t WTriangleMesh::loopCalcEdgeVert( size_t triId, size_t edgeV1, size_t edgeV2, size_t V3 )
{
    size_t neighborVert = -1;
    size_t neighborFaceNum = -1;
    osg::Vec3 edgeVert;

    neighborFaceNum = getNeighbor( edgeV1, edgeV2, triId );

    if( neighborFaceNum == triId )
    {
        osg::Vec3 edgeVert = ( ( *m_verts )[edgeV1] + ( *m_verts )[edgeV2] ) / 2.0;
        size_t vertId = m_countVerts;
        addVertex( edgeVert );
        return vertId;
    }

    else if( neighborFaceNum > triId )
    {
        neighborVert = loopGetThirdVert( edgeV1, edgeV2, neighborFaceNum );

        osg::Vec3 edgePart = ( *m_verts )[edgeV1] + ( *m_verts )[edgeV2];
        osg::Vec3 neighborPart = ( *m_verts )[neighborVert] + ( *m_verts )[V3];

        edgeVert = ( ( edgePart * ( 3.0 / 8.0 ) ) + ( neighborPart * ( 1.0 / 8.0 ) ) );
        size_t vertId = m_countVerts;
        addVertex( edgeVert );
        return vertId;
    }
    else
    {
        size_t neighborCenterP = neighborFaceNum + m_numTriFaces;
        size_t neighborP = neighborFaceNum;

        if( getTriVertId0( neighborP ) == edgeV2 )
        {
            return getTriVertId0( neighborCenterP );
        }
        else if( getTriVertId1( neighborP ) == edgeV2 )
        {
            return getTriVertId1( neighborCenterP );
        }
        else
        {
            return getTriVertId2( neighborCenterP );
        }
    }
    return -1;
}

void WTriangleMesh::loopInsertCornerTriangles( size_t triId )
{
    // comment:             center are twisted from the orignal vertices.
    // original:    0, 1, 2
    // center:              a, b, c
    // reAsgnOrig:  0, a, c
    // addTris:             1, b, a
    // addTris:             2, c, b
    //
    size_t originalTri0 = getTriVertId0( triId );
    size_t originalTri1 = getTriVertId1( triId );
    size_t originalTri2 = getTriVertId2( triId );

    size_t centerTri0 = getTriVertId0( triId + m_numTriFaces );
    size_t centerTri1 = getTriVertId1( triId + m_numTriFaces );
    size_t centerTri2 = getTriVertId2( triId + m_numTriFaces );

    addTriangle( originalTri1, centerTri1, centerTri0 );
    addTriangle( originalTri2, centerTri2, centerTri1 );
    loopSetTriangle( triId, originalTri0, centerTri0, centerTri2 );
}

void WTriangleMesh::loopSetTriangle( size_t triId, size_t vertId1, size_t vertId2, size_t vertId3 )
{
    loopEraseTriangleFromVertex( triId, getTriVertId1( triId ) );
    loopEraseTriangleFromVertex( triId, getTriVertId2( triId ) );

    setTriVert0( triId, vertId1 );
    setTriVert1( triId, vertId2 );
    setTriVert2( triId, vertId3 );

    m_vertexIsInTriangle[vertId2].push_back( triId );
    m_vertexIsInTriangle[vertId3].push_back( triId );
}

void WTriangleMesh::loopEraseTriangleFromVertex( size_t triId, size_t vertId )
{
    std::vector< size_t > temp;
    for( size_t i = 0; i < m_vertexIsInTriangle[vertId].size(); ++i )
    {
        if( triId != m_vertexIsInTriangle[vertId][i] )
            temp.push_back( m_vertexIsInTriangle[vertId][i] );
    }
    m_vertexIsInTriangle[vertId] = temp;
}

double WTriangleMesh::loopGetAlpha( int n )
{
    double answer;
    if( n > 3 )
    {
        double center = ( 0.375 + ( 0.25 * cos( ( 2.0 * 3.14159265358979 ) / static_cast<double>( n ) ) ) );
        answer = ( 0.625 - ( center * center ) ) / static_cast<double>( n );
    }
    else
    {
        answer = 3.0 / 16.0;
    }
    return answer;
}

size_t WTriangleMesh::loopGetNextVertex( size_t triNum, size_t vertNum )
{
    if( getTriVertId0( triNum ) == vertNum )
    {
        return getTriVertId1( triNum );
    }
    else if( getTriVertId1( triNum ) == vertNum )
    {
        return getTriVertId2( triNum );
    }
    return getTriVertId0( triNum );
}

size_t WTriangleMesh::loopGetThirdVert( size_t coVert1, size_t coVert2, size_t triangleNum )
{
    if( !( getTriVertId0( triangleNum ) == coVert1 ) && !( getTriVertId0( triangleNum ) == coVert2 ) )
    {
        return getTriVertId0( triangleNum );
    }
    else if( !( getTriVertId1( triangleNum ) == coVert1 ) && !( getTriVertId1( triangleNum ) == coVert2 ) )
    {
        return getTriVertId1( triangleNum );
    }
    return getTriVertId2( triangleNum );
}

void WTriangleMesh::addMesh( boost::shared_ptr<WTriangleMesh> mesh, float xOff, float yOff, float zOff )
{
    size_t oldVertSize = m_countVerts;

    ( *m_vertColors ).resize( oldVertSize + mesh->vertSize() );
    for( size_t i = 0; i < mesh->vertSize(); ++i )
    {
        osg::Vec3 v( mesh->getVertex( i ) );
        v[0] += xOff;
        v[1] += yOff;
        v[2] += zOff;
        addVertex( v );
        setVertexColor( oldVertSize + i, mesh->getVertColor( i ) );
    }
    for( size_t i = 0; i < mesh->triangleSize(); ++i )
    {
        addTriangle( mesh->getTriVertId0( i ) + oldVertSize, mesh->getTriVertId1( i ) + oldVertSize, mesh->getTriVertId2( i ) + oldVertSize );
    }
    m_meshDirty = true;
}

void WTriangleMesh::translateMesh( float xOff, float yOff, float zOff )
{
    osg::Vec3 t( xOff, yOff, zOff );
    for( size_t i = 0; i < ( *m_verts ).size(); ++i )
    {
        ( *m_verts )[i] += t;
    }
}

void WTriangleMesh::zoomMesh( float zoom )
{
    for( size_t i = 0; i < ( *m_verts ).size(); ++i )
    {
        ( *m_verts )[i] *= zoom;
    }
}

std::ostream& tm_utils::operator<<( std::ostream& os, const WTriangleMesh& rhs )
{
    std::stringstream ss;
    ss << "WTriangleMesh( #vertices=" << rhs.vertSize() << " #triangles=" << rhs.triangleSize() << " )" << std::endl;
    using string_utils::operator<<;
    size_t count = 0;
    ss << std::endl;
    const std::vector< size_t >& triangles = rhs.getTriangles();
    osg::ref_ptr< const osg::Vec3Array > vertices = rhs.getVertexArray();
    for( size_t vID = 0 ; vID <= triangles.size() - 3; ++count )
    {
        std::stringstream prefix;
        prefix << "triangle: " << count << "[ ";
        std::string indent( prefix.str().size(), ' ' );
        using osg::operator<<; // using operator<< as defined in osg/io_utils
        ss << prefix.str() << vertices->at( triangles[ vID++ ] ) << std::endl;
        ss << indent << vertices->at( triangles[ vID++ ] ) << std::endl;
        ss << indent << vertices->at( triangles[ vID++ ] ) << std::endl;
        ss << std::string( indent.size() - 2, ' ' ) << "]" << std::endl;
    }
    return os << ss.str();
}

boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > tm_utils::componentDecomposition( const WTriangleMesh& mesh )
{
    boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > result( new std::list< boost::shared_ptr< WTriangleMesh > >() );
    if( mesh.vertSize() <= 0 ) // no component possible
    {
        return result;
    }
    if( mesh.triangleSize() < 3 )
    {
        if( mesh.vertSize() > 0 )
        {
            // there are vertices but no triangles
            WAssert( false, "Not implemented the decomposition of a TriangleMesh without any triangles" );
        }
        else // no component possible
        {
            return result;
        }
    }

    WUnionFind uf( mesh.vertSize() ); // idea: every vertex in own component, then successivley join in accordance with the triangles

    const std::vector< size_t >& triangles = mesh.getTriangles();
    for( size_t vID = 0; vID <= triangles.size() - 3; vID += 3)
    {
        uf.merge( triangles[ vID ], triangles[ vID + 1 ] );
        uf.merge( triangles[ vID ], triangles[ vID + 2 ] ); // uf.merge( triangles[ vID + 2 ], triangles[ vID + 1 ] ); they are already in same
    }

    // ATTENTION: The reason for using the complex BucketType instead of pasting vertices directly into a new WTriangleMesh
    // is performance! For example: If there are many vertices reused inside the former WTriangleMesh mesh, then we want
    // to reuse them in the new components too. Hence we must determine if a certain vertex is already inside the new component.
    // Since the vertices are organized in a vector, we can use std::find( v.begin, v.end(), vertexToLookUp ) which results
    // in O(N^2) or we could use faster lookUp via key and value leading to the map and the somehow complicated BucketType.
    typedef std::map< osg::Vec3, size_t > VertexType; // look up fast if a vertex is already inside the new mesh!
    typedef std::vector< size_t > TriangleType;
    typedef std::pair< VertexType, TriangleType > BucketType; // Later on the Bucket will be transformed into the new WTriangleMesh component
    std::map< size_t, BucketType > buckets; // Key identify with the cannonical element from UnionFind the new connected component

    osg::ref_ptr< const osg::Vec3Array > vertices = mesh.getVertexArray();
    for( size_t vID = 0; vID <= triangles.size() - 3; vID += 3 )
    {
        size_t component = uf.find( triangles[ vID ] );
        if( buckets.find( component ) == buckets.end() )
        {
            buckets[ component ] = BucketType( VertexType(), TriangleType() ); // create new bucket
        }

        // Note: We discard the order of the points and indices, but semantically the structure remains the same
        VertexType& mapRef = buckets[ component ].first; // short hand alias
        for( int i = 0; i < 3; ++i )
        {
            size_t id = 0;
            const osg::Vec3& vertex = ( *vertices )[ triangles[ vID + i ] ];
            if( mapRef.find( vertex ) == mapRef.end() )
            {
                id = mapRef.size(); // since size might change in next line
                mapRef[ vertex ] = id;
            }
            else
            {
                id = mapRef[ vertex ];
            }
            buckets[ component ].second.push_back( id );
        }
    }

    for( std::map< size_t, BucketType >::const_iterator cit = buckets.begin(); cit != buckets.end(); ++cit )
    {
        osg::ref_ptr< osg::Vec3Array > newVertices( new osg::Vec3Array );
        newVertices->resize( cit->second.first.size() );
        for( VertexType::const_iterator vit = cit->second.first.begin(); vit != cit->second.first.end(); ++vit )
        {
            newVertices->at( vit->second ) = vit->first; // if you are sure that vit->second is always valid replace at() call with operator[]
        }
        boost::shared_ptr< WTriangleMesh > newMesh( new WTriangleMesh( newVertices, cit->second.second ) );
        result->push_back( newMesh );
    }

    return result;
}

osg::ref_ptr< osg::Vec4Array > WTriangleMesh::getTriangleColors() const
{
    return m_triangleColors;
}
