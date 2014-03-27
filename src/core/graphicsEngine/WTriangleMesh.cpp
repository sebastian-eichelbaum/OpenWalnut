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
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <limits>

#include <osg/io_utils>

#include <Eigen/Eigenvalues>

#include "../common/WAssert.h"
#include "../common/WLogger.h"
#include "../common/math/WMath.h"
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
      m_autoNormal( true ),
      m_neighborsCalculated( false ),
      m_curvatureCalculated( false )
{
    m_verts = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_textureCoordinates = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertFlatNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( vertNum ) );

    m_triangles.resize( triangleNum * 3 );
    m_triangleNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( triangleNum ) );
    m_triangleColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( triangleNum ) );
}

WTriangleMesh::WTriangleMesh( osg::ref_ptr< osg::Vec3Array > vertices, const std::vector< size_t >& triangles )
    : m_countVerts( vertices->size() ),
      m_countTriangles( triangles.size() / 3 ),
      m_meshDirty( true ),
      m_autoNormal( true ),
      m_neighborsCalculated( false ),
      m_verts( vertices ),
      m_textureCoordinates( new osg::Vec3Array( vertices->size() ) ),
      m_vertNormals( new osg::Vec3Array( vertices->size() ) ),
      m_vertFlatNormals( new osg::Vec3Array( vertices->size() ) ),
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

void WTriangleMesh::setAutoRecalcNormals( bool autoRecalc )
{
    m_autoNormal = autoRecalc;
}

size_t WTriangleMesh::addVertex( float x, float y, float z )
{
    return addVertex( osg::Vec3( x, y, z ) );
}

size_t WTriangleMesh::addVertex( WPosition vert )
{
    return addVertex( osg::Vec3( vert[0], vert[1], vert[2] ) );
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
    setVertexNormal( index, osg::Vec3( normal[0], normal[1], normal[2] ) );
}

void WTriangleMesh::setVertexNormal( size_t index, float x, float y, float z )
{
    setVertexNormal( index, osg::Vec3( x, y, z ) );
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
    if( forceRecalc || ( m_meshDirty && m_autoNormal ) )
    {
        recalcVertNormals();
    }
    return m_vertNormals;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh::getVertexFlatNormalArray( bool forceRecalc )
{
    if( forceRecalc || ( m_meshDirty && m_autoNormal ) )
    {
        recalcVertNormals();
    }
    return m_vertFlatNormals;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh::getTriangleNormalArray( bool forceRecalc )
{
    if( forceRecalc || ( m_meshDirty && m_autoNormal ) )
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

WVector3d WTriangleMesh::getNormal( size_t index )
{
    WAssert( index < m_countVerts, "get normal as position: index out of range" );
    if( m_meshDirty )
    {
        recalcVertNormals();
    }
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

void WTriangleMesh::recalcVertNormals()
{
    updateVertsInTriangles();

    ( *m_vertNormals ).resize( m_countVerts );
    ( *m_vertFlatNormals ).resize( m_countVerts );
    ( *m_triangleNormals ).resize( m_countTriangles );

    for( size_t i = 0; i < m_countTriangles; ++i )
    {
        ( *m_triangleNormals )[i] = calcTriangleNormal( i );
    }

    for( size_t vertId = 0; vertId < m_countVerts; ++vertId )
    {
        osg::Vec3 tempNormal( 0.0, 0.0, 0.0 );
        osg::Vec3 tempFlatNormal( 0.0, 0.0, 0.0 );
        for( size_t neighbour = 0; neighbour < m_vertexIsInTriangle[vertId].size(); ++neighbour )
        {
            tempNormal += ( *m_triangleNormals )[ m_vertexIsInTriangle[vertId][neighbour] ];
            tempFlatNormal = ( *m_triangleNormals )[ m_vertexIsInTriangle[vertId][0] ];
        }
        tempNormal *= 1./m_vertexIsInTriangle[vertId].size();

        tempNormal.normalize();
        ( *m_vertNormals )[vertId] = tempNormal;
        ( *m_vertFlatNormals )[vertId] = tempFlatNormal; // note: normal already normalized
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

void WTriangleMesh::rescaleVertexColors()
{
    float maxR = 0;
    float maxG = 0;
    float maxB = 0;
    for( size_t vertId = 0; vertId < m_vertColors->size(); ++vertId )
    {
        if( ( *m_vertColors )[vertId][0] > maxR )
        {
            maxR = ( *m_vertColors )[vertId][0];
        }
        if( ( *m_vertColors )[vertId][1] > maxG )
        {
            maxG = ( *m_vertColors )[vertId][1];
        }
        if( ( *m_vertColors )[vertId][2] > maxB )
        {
            maxB = ( *m_vertColors )[vertId][2];
        }
    }
    for( size_t vertId = 0; vertId < m_vertColors->size(); ++vertId )
    {
        ( *m_vertColors )[vertId][0] /= maxR;
        ( *m_vertColors )[vertId][1] /= maxG;
        ( *m_vertColors )[vertId][2] /= maxB;
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

void WTriangleMesh::performFeaturePreservingSmoothing( float sigmaDistance, float sigmaInfluence )
{
    updateVertsInTriangles();

    calcNeighbors();

    // we perform a first smoothing pass and write the resulting vertex coords into a buffer
    // this will only update the normals
    performFeaturePreservingSmoothingMollificationPass( sigmaDistance / 2.0f, sigmaInfluence );

    // using the smoothed normals, we now perform a second smoothing pass, this time writing the new vertex coords
    performFeaturePreservingSmoothingVertexPass( sigmaDistance, sigmaInfluence );
}

void WTriangleMesh::performFeaturePreservingSmoothingMollificationPass( float sigmaDistance, float sigmaInfluence )
{
    // calc Eq. 3 for every triangle
    osg::ref_ptr< osg::Vec3Array > vtxArray = new osg::Vec3Array( m_verts->size() );

    for( std::size_t k = 0; k < m_verts->size(); ++k )
    {
        vtxArray->operator[] ( k ) = estimateSmoothedVertexPosition( k, sigmaDistance, sigmaInfluence, true );
    }

    // calc the new normal directions - update triangle normals
    for( std::size_t k = 0; k < m_triangles.size() / 3; ++k )
    {
        osg::Vec3 const& p0 = vtxArray->operator[]( m_triangles[ 3 * k + 0 ] );
        osg::Vec3 const& p1 = vtxArray->operator[]( m_triangles[ 3 * k + 1 ] );
        osg::Vec3 const& p2 = vtxArray->operator[]( m_triangles[ 3 * k + 2 ] );

        m_triangleNormals->operator[] ( k ) = ( p1 - p0 ) ^ ( p2 - p1 );
        m_triangleNormals->operator[] ( k ).normalize();
    }
}

void WTriangleMesh::performFeaturePreservingSmoothingVertexPass( float sigmaDistance, float sigmaInfluence )
{
    for( std::size_t k = 0; k < m_verts->size(); ++k )
    {
        m_verts->operator[] ( k ) = estimateSmoothedVertexPosition( k, sigmaDistance, sigmaInfluence, false );
    }

    recalcVertNormals();
}

osg::Vec3 WTriangleMesh::estimateSmoothedVertexPosition( std::size_t vtx, float sigmaDistance, float sigmaInfluence, bool mollify )
{
    std::stack< std::size_t > triStack;
    std::set< std::size_t > triSet;

    for( std::size_t k = 0; k < m_vertexIsInTriangle[ vtx ].size(); ++k )
    {
        triStack.push( m_vertexIsInTriangle[ vtx ][ k ] );
        triSet.insert( m_vertexIsInTriangle[ vtx ][ k ] );
    }

    while( !triStack.empty() )
    {
        std::size_t currentTriangle = triStack.top();
        triStack.pop();

        for( std::size_t k = 0; k < m_triangleNeighbors[ currentTriangle ].size(); ++k )
        {
            osg::Vec3 center = calcTriangleCenter( m_triangleNeighbors[ currentTriangle ][ k ] );

            if( ( m_verts->operator[] ( vtx ) - center ).length() > 4.0 * sigmaDistance )
            {
                continue;
            }

            if( triSet.find( m_triangleNeighbors[ currentTriangle ][ k ] ) == triSet.end() )
            {
                triStack.push( m_triangleNeighbors[ currentTriangle ][ k ] );
                triSet.insert( m_triangleNeighbors[ currentTriangle ][ k ] );
            }
        }
    }

    double sum = 0.0;
    osg::Vec3 res( 0.0, 0.0, 0.0 );

    for( std::set< std::size_t >::const_iterator it = triSet.begin(); it != triSet.end(); ++it )
    {
        osg::Vec3 center = calcTriangleCenter( *it );
        double area = calcTriangleArea( *it );

        // calc f
        double dist = ( m_verts->operator[] ( vtx ) - center ).length();
        double f = 1.0 / ( sigmaDistance * sqrt( 2.0 * piDouble ) ) * exp( -0.5 * dist * dist );

        double g;
        if( !mollify )
        {
            // calc prediction
            osg::Vec3f const& p = m_verts->operator[] ( vtx );
            osg::Vec3f const& n = m_triangleNormals->operator[] ( *it );
            osg::Vec3f pred = p + n * ( n * ( center - p ) );

            dist = ( p - pred ).length();
        }
        g = 1.0 / ( sigmaInfluence * sqrt( 2.0 * piDouble ) ) * exp( -0.5 * dist * dist );

        sum += area * f * g;
        res += center * area * f * g;
    }

    res /= sum;
    return res;
}

osg::Vec3 WTriangleMesh::calcTriangleCenter( std::size_t triIdx ) const
{
    osg::Vec3 res = m_verts->operator[] ( m_triangles[ 3 * triIdx + 0 ] );
    res += m_verts->operator[] ( m_triangles[ 3 * triIdx + 1 ] );
    res += m_verts->operator[] ( m_triangles[ 3 * triIdx + 2 ] );

    res /= 3.0f;
    return res;
}

float WTriangleMesh::calcTriangleArea( std::size_t triIdx ) const
{
    osg::Vec3 const& p0 = m_verts->operator[] ( m_triangles[ 3 * triIdx + 0 ] );
    osg::Vec3 const& p1 = m_verts->operator[] ( m_triangles[ 3 * triIdx + 1 ] );
    osg::Vec3 const& p2 = m_verts->operator[] ( m_triangles[ 3 * triIdx + 2 ] );

    return ( ( p1 - p0 ) ^ ( p2 - p0 ) ).length() * 0.5;
}

void WTriangleMesh::estimateCurvature()
{
    updateVertsInTriangles();
    calcNeighbors();

    std::vector< osg::Vec3 > normals( m_verts->size() );

    // init vectors
    m_mainNormalCurvature = boost::shared_ptr< std::vector< float > >( new std::vector< float >( m_verts->size() ) );
    m_secondaryNormalCurvature = boost::shared_ptr< std::vector< float > >( new std::vector< float >( m_verts->size() ) );
    m_mainCurvaturePrincipalDirection = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( m_verts->size() ) );
    m_secondaryCurvaturePrincipalDirection = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( m_verts->size() ) );

    // calculate vertex normals using distance-weighted summing of neighbor-triangle normals
    for( std::size_t vtxId = 0; vtxId < m_verts->size(); ++vtxId )
    {
        osg::Vec3 const& p = m_verts->operator[] ( vtxId );
        osg::Vec3 n( 0.0, 0.0, 0.0 );

        for( std::size_t k = 0; k < m_vertexIsInTriangle[ vtxId ].size(); ++k )
        {
            std::size_t triId = m_vertexIsInTriangle[ vtxId ][ k ];

            osg::Vec3 center = calcTriangleCenter( triId );
            double w = 1.0 / ( center - p ).length();

            n += m_triangleNormals->operator[] ( triId ) * w;
        }

        WAssert( n.length() > 0.0001, "Invalid normal!" );

        n.normalize();
        normals[ vtxId ] = n;
    }

    // calculate curvatures for every vertex
    for( std::size_t vtxId = 0; vtxId < m_verts->size(); ++vtxId )
    {
        osg::Vec3 const& p = m_verts->operator[] ( vtxId );

        osg::Vec3 const& normal = normals[ vtxId ];

        // get the set of neighbor vertices
        std::set< std::size_t > neighbors;

        for( std::size_t k = 0; k < m_vertexIsInTriangle[ vtxId ].size(); ++k )
        {
            std::size_t triId = m_vertexIsInTriangle[ vtxId ][ k ];

            for( std::size_t j = 0; j < 3; ++j )
            {
                std::size_t e = m_triangles[ 3 * triId + j ];

                if( neighbors.find( e ) == neighbors.end() && e != vtxId )
                {
                    neighbors.insert( e );
                }
            }
        }

        WAssert( neighbors.size() > 2, "Vertex has too few neighbors! Does this mesh have holes?" );

        double maxCurvature = -std::numeric_limits< double >::infinity();
        std::vector< double > curvatures;

        osg::Vec3 maxCurvatureTangent( 0.0, 0.0, 0.0 );
        std::vector< osg::Vec3 > tangents;

        // part 1: get curvatures at tangents and their maximum curvature
        for( std::set< std::size_t >::const_iterator it = neighbors.begin(); it != neighbors.end(); ++it )
        {
            osg::Vec3 const& neighbPos = m_verts->operator[] ( *it );
            osg::Vec3 const& neighbNormal = normals[ *it ];

            // project ( neighbPos - p ) onto the tangent plane
            osg::Vec3 tangent = ( neighbPos - p ) - normal * ( ( neighbPos - p ) * normal );
            tangent.normalize();

            // approximate normal curvature in tangent direction
            double ncurv = -1.0 * ( ( neighbPos - p ) * ( neighbNormal - normal ) ) / ( ( neighbPos - p ) * ( neighbPos - p ) );

            if( ncurv > maxCurvature )
            {
                maxCurvature = ncurv;
                maxCurvatureTangent = tangent;
            }

            tangents.push_back( tangent );
            curvatures.push_back( ncurv );
        }

        WAssert( maxCurvatureTangent.length() > 0.0001, "Invalid tangent length!" );

        // part 2: choose a coordinate system in the tangent plane
        osg::Vec3 const& e1 = maxCurvatureTangent;
        osg::Vec3 e2 = e1 ^ normal;

        e2.normalize();

        bool significantCurvature = false;
        for( std::vector< double >::const_iterator it = curvatures.begin(); it != curvatures.end(); ++it )
        {
            if( fabs( *it ) > 0.00001 )
            {
                significantCurvature = true;
                break;
            }
        }

        if( !significantCurvature )
        {
            // curvatures were all almost zero
            // the mesh is flat at this point, write values accordingly
            m_mainNormalCurvature->operator[] ( vtxId ) = 0.0;
            m_mainCurvaturePrincipalDirection->operator[] ( vtxId ) = e1;
            m_secondaryNormalCurvature->operator[] ( vtxId ) = 0.0;
            m_secondaryCurvaturePrincipalDirection->operator[] ( vtxId ) = e2;

            continue;
        }

        // calculate coefficients of the ellipse a * cos²(theta) + b * cos(theta) * sin(theta) * c * sin²(theta)
        // this is done by estimating a as the largest curvature amoung the estimated curvatures in all tangent
        // direction belonging to points that share a triangle with the current one
        double const& a = maxCurvature;

        Eigen::Matrix< double, -1, -1 > X( tangents.size(), 2 );
        Eigen::Matrix< double, -1, -1 > y( tangents.size(), 1 );

        for( std::size_t k = 0; k < tangents.size(); ++k )
        {
            double theta = calcAngleBetweenNormalizedVectors( tangents[ k ], e1 );

            X( k, 0 ) = cos( theta ) * sin( theta );
            X( k, 1 ) = sin( theta ) * sin( theta );
            y( k, 0 ) = curvatures[ k ] - a * cos( theta ) * cos( theta );
        }

        // use LU decomposition to calculate rank
        Eigen::FullPivLU< Eigen::Matrix< double, -1, -1 > > lu( X );

        // we need a rank of at least 2 to calculate the coeffs
        if( lu.rank() < 2 )
        {
            wlog::warn( "WTriangleMesh::estimateCurvature" ) << "Rank too low, cannot estimate curvature for this vertex!";

            m_mainNormalCurvature->operator[] ( vtxId ) = a;
            m_mainCurvaturePrincipalDirection->operator[] ( vtxId ) = e1;
            m_secondaryNormalCurvature->operator[] ( vtxId ) = 0.0;
            m_secondaryCurvaturePrincipalDirection->operator[] ( vtxId ) = e2;

            continue;
        }

        // do least squares
        Eigen::Matrix< double, -1, -1 > bb = ( X.transpose() * X ).inverse() * X.transpose() * y;

        // the missing coeffs b and c are now:
        double b = bb( 0, 0 );
        double c = bb( 1, 0 );

        // this calculates the maximum and minimum normal curvatures
        // (as eigenvalues)
        double Kg = a * c - 0.25 * b * b;
        double H = 0.5 * ( a + c );
        double s = H * H - Kg;

        if( s < 0.0 )
        {
            s = 0.0;
        }

        double k1 = H + sqrt( s );
        double k2 = H - sqrt( s );

        if( fabs( k1 - k2 ) < 0.000001 )
        {
            // if the curvatures are equal, there is no single principal direction
            m_mainNormalCurvature->operator[] ( vtxId ) = a;
            m_mainCurvaturePrincipalDirection->operator[] ( vtxId ) = e1;
        }
        else
        {
            // if the curvatures differ, we can now find the direction of maximum curvature
            double temp = b / ( k2 - k1 );

            if( temp > 1.0 )
            {
                temp = 1.0;
            }
            if( temp < -1.0 )
            {
                temp = -1.0;
            }

            double theta = 0.5 * asin( temp );

            osg::Vec3 ne1 = e1 * cos( theta ) + e2 * sin( theta );
            osg::Vec3 ne2 = e2 * cos( theta ) - e1 * sin( theta );

            ne1.normalize();
            ne2.normalize();

            e2 = ne2;

            theta = calcAngleBetweenNormalizedVectors( ne1, e1 );

            m_mainNormalCurvature->operator[] ( vtxId ) = a * cos( theta ) * cos( theta )
                                                        + b * cos( theta ) * sin( theta )
                                                        + c * sin( theta ) * sin( theta );
            m_mainCurvaturePrincipalDirection->operator[] ( vtxId ) = ne1;
        }

        double theta = calcAngleBetweenNormalizedVectors( e2, e1 );

        m_secondaryNormalCurvature->operator[] ( vtxId ) = a * cos( theta ) * cos( theta )
                                                         + b * cos( theta ) * sin( theta )
                                                         + c * sin( theta ) * sin( theta );
        m_secondaryCurvaturePrincipalDirection->operator[] ( vtxId ) = e2;
    }

    m_curvatureCalculated = true;
}

double WTriangleMesh::calcAngleBetweenNormalizedVectors( osg::Vec3 const& v1, osg::Vec3 const& v2 )
{
    // assumes vectors are normalized
    WAssert( v1.length() <  1.0001, "Vector is not normalized!" );
    WAssert( v1.length() > -1.0001, "Vector is not normalized!" );
    WAssert( v2.length() <  1.0001, "Vector is not normalized!" );
    WAssert( v2.length() > -1.0001, "Vector is not normalized!" );

    double temp = v1 * v2;

    // avoid NaNs due to numerical errors
    if( temp < -1.0 )
    {
        temp = -1.0;
    }
    if( temp > 1.0 )
    {
        temp = 1.0;
    }

    return acos( temp );
}

double WTriangleMesh::getMainCurvature( std::size_t vtxId ) const
{
    return m_mainNormalCurvature->operator[] ( vtxId );
}

double WTriangleMesh::getSecondaryCurvature( std::size_t vtxId ) const
{
    return m_secondaryNormalCurvature->operator[] ( vtxId );
}

boost::shared_ptr< std::vector< float > > const& WTriangleMesh::getMainCurvatures() const
{
    return m_mainNormalCurvature;
}

boost::shared_ptr< std::vector< float > > const& WTriangleMesh::getSecondaryCurvatures() const
{
    return m_secondaryNormalCurvature;
}

osg::Vec3 WTriangleMesh::getCurvatureMainPrincipalDirection( std::size_t vtxId ) const
{
    return m_mainCurvaturePrincipalDirection->operator[] ( vtxId );
}

osg::Vec3 WTriangleMesh::getCurvatureSecondaryPrincipalDirection( std::size_t vtxId ) const
{
    return m_secondaryCurvaturePrincipalDirection->operator[] ( vtxId );
}

void WTriangleMesh::setTextureCoord( std::size_t index, osg::Vec3 texCoord )
{
    m_textureCoordinates->operator[] ( index ) = texCoord;
}

osg::ref_ptr< osg::Vec3Array > WTriangleMesh::getMainPrincipalCurvatureDirectionArray()
{
    return m_mainCurvaturePrincipalDirection;
}

osg::ref_ptr< osg::Vec3Array > WTriangleMesh::getSecondaryPrincipalCurvatureDirectionArray()
{
    return m_secondaryCurvaturePrincipalDirection;
}

