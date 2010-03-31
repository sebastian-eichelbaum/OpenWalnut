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
    m_triangles[ m_countTriangles * 3 ] = vert0;
    m_triangles[ m_countTriangles * 3 +1 ] = vert1;
    m_triangles[ m_countTriangles * 3 +2 ] = vert2;
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
    ( *m_vertNormals )[index] = normal;
}

void WTriangleMesh2::setVertexNormal( size_t index, wmath::WPosition normal )
{
    setVertexNormal( index, osg::Vec3( normal[0], normal[1], normal[2] ) );
}

void WTriangleMesh2::setVertexColor( size_t index, osg::Vec4 color )
{
    ( *m_vertColors )[index] = color;
}

void WTriangleMesh2::setVertexColor( size_t index, WColor color )
{
    setVertexColor( index, osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() ) );
}


void WTriangleMesh2::setTriangleColor( size_t index, osg::Vec4 color )
{
    ( *m_triangleColors )[index] = color;
}

void WTriangleMesh2::setTriangleColor( size_t index, WColor color )
{
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

osg::Vec3 WTriangleMesh2::getVertex( size_t index )
{
    return ( *m_verts )[index];
}

wmath::WPosition WTriangleMesh2::getVertexAsPosition( size_t index )
{
    return wmath::WPosition( ( *m_verts )[index][0], ( *m_verts )[index][1], ( *m_verts )[index][2] );
}

wmath::WPosition WTriangleMesh2::getVertexAsPosition( size_t triangleIndex, size_t vertNum )
{
    osg::Vec3 v = getVertex( triangleIndex, vertNum );
    return wmath::WPosition( v[0], v[1], v[2] );
}

void WTriangleMesh2::removeVertex( size_t index )
{
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
    m_vertexIsInTriangle.resize( m_countVerts, v );

    for ( size_t i = 0; i < m_countTriangles; ++i )
    {
        m_vertexIsInTriangle[ m_triangles[i*3] ].push_back( i );
        m_vertexIsInTriangle[ m_triangles[i*3+1] ].push_back( i );
        m_vertexIsInTriangle[ m_triangles[i*3+2] ].push_back( i );
    }
}

osg::Vec3 WTriangleMesh2::calcTriangleNormal( size_t triangle )
{
    osg::Vec3 v1( getVertex( triangle, 1 ) - getVertex( triangle, 0 ) );
    osg::Vec3 v2( getVertex( triangle, 2 ) - getVertex( triangle, 0 ) );

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

size_t WTriangleMesh2::vertSize()
{
    return m_countVerts;
}

size_t WTriangleMesh2::triangleSize()
{
    return m_countTriangles;
}

void WTriangleMesh2::calcNeighbors()
{
    std::vector<size_t> v( 3, -1 );
    m_triangleNeighbors.resize( m_countTriangles, v );

    for ( size_t triId = 0; triId < m_countTriangles; ++triId )
    {
        size_t coVert0 = m_triangles[triId*3];
        size_t coVert1 = m_triangles[triId*3+1];
        size_t coVert2 = m_triangles[triId*3+1];

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
