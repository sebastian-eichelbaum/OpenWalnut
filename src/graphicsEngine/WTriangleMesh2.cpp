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
#include <string>

#include "WTriangleMesh2.h"

// init _static_ member variable and provide a linker reference to it
boost::shared_ptr< WPrototyped > WTriangleMesh2::m_prototype = boost::shared_ptr< WPrototyped >();

boost::shared_ptr< WPrototyped > WTriangleMesh2::getPrototype()
{
    if ( !m_prototype )
    {
         m_prototype = boost::shared_ptr< WPrototyped >( new WTriangleMesh2( 0, 0 ) );
    }
    return m_prototype;
}

/**
 * constructor that already reserves space for a given number of triangles and vertexes
 */
WTriangleMesh2::WTriangleMesh2( size_t vertNum, size_t triangleNum ) :
    m_countVerts( 0 ),
    m_countTriangles( 0 ),
    m_meshDirty( true ),
    m_neighborsCalculated( false )
{
    m_verts = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( vertNum ) );
    m_vertColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( vertNum ) );

    m_triangles.resize( triangleNum * 3 );
    m_triangleNormals = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array( triangleNum ) );
    m_triangleColors = osg::ref_ptr< osg::Vec4Array >( new osg::Vec4Array( triangleNum ) );
}

WTriangleMesh2::~WTriangleMesh2()
{
}

void WTriangleMesh2::addVertex( float x, float y, float z )
{
    addVertex( osg::Vec3( x, y, z ) );
}

void WTriangleMesh2::addVertex( wmath::WPosition vert )
{
    addVertex( osg::Vec3( vert[0], vert[1], vert[2] ) );
}

void WTriangleMesh2::addTriangle( size_t vert0, size_t vert1, size_t vert2 )
{
    if ( m_triangles.size() == m_countTriangles * 3 )
    {
        m_triangles.resize( m_countTriangles * 3 + 3 );
    }
    m_triangles[ m_countTriangles * 3 ] = vert0;
    m_triangles[ m_countTriangles * 3 + 1 ] = vert1;
    m_triangles[ m_countTriangles * 3 + 2 ] = vert2;
    ++m_countTriangles;
}

void WTriangleMesh2::addTriangle( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 )
{
    addVertex( vert0 );
    addVertex( vert1 );
    addVertex( vert2 );
    addTriangle( m_countVerts - 3, m_countVerts - 2, m_countVerts - 1 );
}

void WTriangleMesh2::setVertexNormal( size_t index, osg::Vec3 normal )
{
    WAssert( index < m_countVerts, "set vertex normal: index out of range" );
    ( *m_vertNormals )[index] = normal;
}

void WTriangleMesh2::setVertexNormal( size_t index, wmath::WPosition normal )
{
    WAssert( index < m_countVerts, "set vertex normal: index out of range" );
    setVertexNormal( index, osg::Vec3( normal[0], normal[1], normal[2] ) );
}

void WTriangleMesh2::setVertexColor( size_t index, osg::Vec4 color )
{
    WAssert( index < m_countVerts, "set vertex color: index out of range" );
    ( *m_vertColors )[index] = color;
}

void WTriangleMesh2::setVertexColor( size_t index, WColor color )
{
    WAssert( index < m_countVerts, "set vertex color: index out of range" );
    setVertexColor( index, osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() ) );
}


void WTriangleMesh2::setTriangleColor( size_t index, osg::Vec4 color )
{
    WAssert( index < m_countTriangles, "set triangle color: index out of range" );
    ( *m_triangleColors )[index] = color;
}

void WTriangleMesh2::setTriangleColor( size_t index, WColor color )
{
    WAssert( index < m_countTriangles, "set triangle color: index out of range" );
    setTriangleColor( index, osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() ) );
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh2::getVertexArray()
{
    return m_verts;
}

osg::ref_ptr< osg::Vec3Array >WTriangleMesh2::getVertexNormalArray( bool forceRecalc )
{
    if ( forceRecalc || m_meshDirty )
    {
        recalcVertNormals();
    }
    return m_vertNormals;
}

osg::ref_ptr< osg::Vec4Array >WTriangleMesh2::getVertexColorArray()
{
    return m_vertColors;
}

std::vector< size_t >WTriangleMesh2::getTriangles()
{
    return m_triangles;
}

osg::Vec3 WTriangleMesh2::getVertex( size_t index ) const
{
    WAssert( index < m_countVerts, "get vertex: index out of range" );
    return ( *m_verts )[index];
}

wmath::WPosition WTriangleMesh2::getVertexAsPosition( size_t index ) const
{
    WAssert( index < m_countVerts, "get vertex as position: index out of range" );
    return wmath::WPosition( ( *m_verts )[index][0], ( *m_verts )[index][1], ( *m_verts )[index][2] );
}

wmath::WPosition WTriangleMesh2::getVertexAsPosition( size_t triangleIndex, size_t vertNum )
{
    WAssert( triangleIndex < m_countTriangles, "get vertex as position: index out of range" );
    osg::Vec3 v = getTriVert( triangleIndex, vertNum );
    return wmath::WPosition( v[0], v[1], v[2] );
}

void WTriangleMesh2::removeVertex( size_t index )
{
    WAssert( index < m_countVerts, "remove vertex: index out of range" );
    if ( m_vertexIsInTriangle[ index ].size() > 0 )
    {
        return;
    }
    ( *m_verts ).erase( ( *m_verts ).begin() + index );

    for ( size_t i = 0; i < m_countTriangles * 3; ++i )
    {
        if ( m_triangles[ i ] > index )
        {
            --m_triangles[ i ];
        }
    }
    m_meshDirty = true;
}

void WTriangleMesh2::removeTriangle( size_t index )
{
    WAssert( index < m_countTriangles, "remove triangle: index out of range" );
    m_triangles.erase( m_triangles.begin() + index * 3, m_triangles.begin() + index * 3 + 3 );
    m_meshDirty = true;
}

void WTriangleMesh2::cleanUpMesh()
{
    //TODO(schurade): implement this
}

void WTriangleMesh2::recalcVertNormals()
{
    updateVertsInTriangles();

    ( *m_vertNormals ).resize( m_countVerts );
    ( *m_triangleNormals ).resize( m_countTriangles );

    for ( size_t i = 0; i < m_countTriangles; ++i )
    {
        ( *m_triangleNormals )[i] = calcTriangleNormal( i );
    }

    for ( size_t vertId = 0; vertId < m_countVerts; ++vertId )
    {
        osg::Vec3 tempNormal( 0.0, 0.0, 0.0 );
        for ( size_t neighbour = 0; neighbour < m_vertexIsInTriangle[vertId].size(); ++neighbour )
        {
            tempNormal += ( *m_triangleNormals )[ m_vertexIsInTriangle[vertId][neighbour] ];
        }
        tempNormal *= 1./m_vertexIsInTriangle[vertId].size();

        tempNormal.normalize();
        ( *m_vertNormals )[vertId] = tempNormal;
    }

    m_meshDirty = false;
}

void WTriangleMesh2::updateVertsInTriangles()
{
    m_vertexIsInTriangle.clear();
    std::vector< size_t >v;
    m_vertexIsInTriangle.resize( ( *m_verts ).size(), v );

    for ( size_t i = 0; i < m_countTriangles; ++i )
    {
        m_vertexIsInTriangle[ getTriVertId0( i ) ].push_back( i );
        m_vertexIsInTriangle[ getTriVertId1( i ) ].push_back( i );
        m_vertexIsInTriangle[ getTriVertId2( i ) ].push_back( i );
    }
}

osg::Vec3 WTriangleMesh2::calcTriangleNormal( size_t triangle )
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

osg::Vec3 WTriangleMesh2::calcNormal( osg::Vec3 vert0, osg::Vec3 vert1, osg::Vec3 vert2 )
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

size_t WTriangleMesh2::vertSize() const
{
    return m_countVerts;
}

size_t WTriangleMesh2::triangleSize() const
{
    return m_countTriangles;
}

void WTriangleMesh2::calcNeighbors()
{
    std::vector<size_t> v( 3, -1 );
    m_triangleNeighbors.resize( ( *m_triangleNormals ).size(), v );

    for ( size_t triId = 0; triId < m_countTriangles; ++triId )
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

size_t WTriangleMesh2::getNeighbor( const size_t coVert1, const size_t coVert2, const size_t triangleNum )
{
    std::vector< size_t > candidates = m_vertexIsInTriangle[coVert1];
    std::vector< size_t > compares = m_vertexIsInTriangle[coVert2];

    for ( size_t i = 0; i < candidates.size(); ++i )
    {
        for ( size_t k = 0; k < compares.size(); ++k )
        {
            if ( ( candidates[i] != triangleNum ) && ( candidates[i] == compares[k] ) )
            {
                return candidates[i];
            }
        }
    }
    return triangleNum;
}

void WTriangleMesh2::doLoopSubD()
{
    m_numTriVerts = m_countVerts;
    m_numTriFaces = m_countTriangles;

    ( *m_verts ).resize( m_numTriVerts * 4 );
    m_triangles.resize( m_numTriFaces * 4 * 3 );

    updateVertsInTriangles();

    osg::Vec3* newVertexPositions = new osg::Vec3[m_numTriVerts];

    //std::cout << "loop subdivision pass 1" << std::endl;
    for ( size_t i = 0; i < m_numTriVerts; ++i )
    {
        newVertexPositions[i] = loopCalcNewPosition( i );
    }

    //std::cout << "loop subdivision pass 2" << std::endl;
    for ( size_t i = 0; i < m_numTriFaces; ++i )
    {
        loopInsertCenterTriangle( i );
    }
    ( *m_verts ).resize( m_countVerts );
    std::vector< size_t >v;
    m_vertexIsInTriangle.resize( ( *m_verts ).size(), v );

    //std::cout << "loop subdivision pass 3" << std::endl;
    for ( size_t i = 0; i < m_numTriFaces; ++i )
    {
        loopInsertCornerTriangles( i );
    }

    //std::cout << "loop subdivision pass 4" << std::endl;
    for ( size_t i = 0; i < m_numTriVerts; ++i )
    {
        ( *m_verts )[i] = newVertexPositions[i];
    }

    delete[] newVertexPositions;
    m_meshDirty = true;
}


osg::Vec3 WTriangleMesh2::loopCalcNewPosition( size_t vertId )
{
    std::vector< size_t > starP = m_vertexIsInTriangle[vertId];
    int starSize = starP.size();

    osg::Vec3 oldPos = getVertex( vertId );
    double alpha = loopGetAlpha( starSize );

    double scale = 1.0 - ( static_cast<double>( starSize ) * alpha );
    oldPos *= scale;

    osg::Vec3 newPos;
    int edgeV = 0;
    for ( int i = 0; i < starSize; i++ )
    {
        edgeV = loopGetNextVertex( starP[i], vertId );
        osg::Vec3 translate = getVertex( edgeV );
        newPos += translate;
    }
    newPos *= alpha;

    return oldPos + newPos;
}

void WTriangleMesh2::loopInsertCenterTriangle( size_t triId )
{
    size_t edgeVerts[3];

    edgeVerts[0] = loopCalcEdgeVert( triId, getTriVertId0( triId ), getTriVertId1( triId ), getTriVertId2( triId ) );
    edgeVerts[1] = loopCalcEdgeVert( triId, getTriVertId1( triId ), getTriVertId2( triId ), getTriVertId0( triId ) );
    edgeVerts[2] = loopCalcEdgeVert( triId, getTriVertId2( triId ), getTriVertId0( triId ), getTriVertId1( triId ) );

    addTriangle( edgeVerts[0], edgeVerts[1], edgeVerts[2] );
}


size_t WTriangleMesh2::loopCalcEdgeVert( size_t triId, size_t edgeV1, size_t edgeV2, size_t V3 )
{
    size_t neighborVert = -1;
    size_t neighborFaceNum = -1;
    osg::Vec3 edgeVert;

    neighborFaceNum = getNeighbor( edgeV1, edgeV2, triId );

    if ( neighborFaceNum == triId )
    {
        osg::Vec3 edgeVert = ( ( *m_verts )[edgeV1] + ( *m_verts )[edgeV2] ) / 2.0;
        size_t vertId = m_countVerts;
        addVertex( edgeVert );
        return vertId;
    }

    else if ( neighborFaceNum > triId )
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

        if ( getTriVertId0( neighborP ) == edgeV2 )
        {
            return getTriVertId0( neighborCenterP );
        }
        else if ( getTriVertId1( neighborP ) == edgeV2 )
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

void WTriangleMesh2::loopInsertCornerTriangles( size_t triId )
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

void WTriangleMesh2::loopSetTriangle( size_t triId, size_t vertId1, size_t vertId2, size_t vertId3 )
{
    loopEraseTriangleFromVertex( triId, getTriVertId1( triId ) );
    loopEraseTriangleFromVertex( triId, getTriVertId2( triId ) );

    setTriVert0( triId, vertId1 );
    setTriVert1( triId, vertId2 );
    setTriVert2( triId, vertId3 );

    m_vertexIsInTriangle[vertId2].push_back( triId );
    m_vertexIsInTriangle[vertId3].push_back( triId );
}

void WTriangleMesh2::loopEraseTriangleFromVertex( size_t triId, size_t vertId )
{
    std::vector< size_t > temp;
    for ( size_t i = 0; i < m_vertexIsInTriangle[vertId].size(); ++i )
    {
        if ( triId != m_vertexIsInTriangle[vertId][i] )
            temp.push_back( m_vertexIsInTriangle[vertId][i] );
    }
    m_vertexIsInTriangle[vertId] = temp;
}

double WTriangleMesh2::loopGetAlpha( int n )
{
    double answer;
    if ( n > 3 )
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

size_t WTriangleMesh2::loopGetNextVertex( size_t triNum, size_t vertNum )
{
    if ( getTriVertId0( triNum ) == vertNum )
    {
        return getTriVertId1( triNum );
    }
    else if ( getTriVertId1( triNum ) == vertNum )
    {
        return getTriVertId2( triNum );
    }
    return getTriVertId0( triNum );
}

size_t WTriangleMesh2::loopGetThirdVert( size_t coVert1, size_t coVert2, size_t triangleNum )
{
    if ( !( getTriVertId0( triangleNum ) == coVert1 ) && !( getTriVertId0( triangleNum ) == coVert2 ) )
    {
        return getTriVertId0( triangleNum );
    }
    else if ( !( getTriVertId1( triangleNum ) == coVert1 ) && !( getTriVertId1( triangleNum ) == coVert2 ) )
    {
        return getTriVertId1( triangleNum );
    }
    return getTriVertId2( triangleNum );
}
