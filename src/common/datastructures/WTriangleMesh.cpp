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

#include <boost/shared_ptr.hpp>

#include "WTriangleMesh.h"
#include "WUnionFind.h"

// init _static_ member variable and provide a linker reference to it
boost::shared_ptr< WPrototyped > WTriangleMesh::m_prototype = boost::shared_ptr< WPrototyped >();

boost::shared_ptr< WPrototyped > WTriangleMesh::getPrototype()
{
    if ( !m_prototype )
    {
         m_prototype = boost::shared_ptr< WPrototyped >( new WTriangleMesh() );
    }
    return m_prototype;
}

WTriangleMesh::WTriangleMesh()
    : m_fastAddVertId( 0 ),
      m_fastAddTriangleId( 0 ),
      m_computedTriNormals( false ),
      m_triNormals( 0 ),
      m_computedVertNormals( false ),
      m_vertNormals( 0 )
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

void WTriangleMesh::setVertices( const std::vector< wmath::WPosition >& vertices )
{
    m_fastAddVertId = vertices.size();
    m_vertices = vertices;
}

const std::vector< wmath::WPosition >& WTriangleMesh::getVertices() const
{
    return m_vertices;
}

const std::vector< Triangle >& WTriangleMesh::getTriangles() const
{
    return m_triangles;
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

void WTriangleMesh::setTriangles( const std::vector< Triangle >& triangles )
{
    m_fastAddTriangleId = triangles.size();
    m_triangles = triangles;
}

size_t WTriangleMesh::getFastAddTriangleId() const
{
    return m_fastAddTriangleId;
}

std::vector< unsigned int > WTriangleMesh::getPositionTriangleNeighborsSlow( size_t i ) const
{
    std::vector< unsigned int > neighborIds( 0 );
    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        for( unsigned int j = 0; j < 3; ++j )
        {
            if( m_triangles[triId].pointID[j] == i )
            {
                neighborIds.push_back( triId );
            }
        }
    }
    return neighborIds;
}

wmath::WVector3D WTriangleMesh::getVertexNormalSlow( size_t i ) const
{
    std::vector< unsigned int > neighborIndices = getPositionTriangleNeighborsSlow( i );

    wmath::WVector3D normal;
    for( unsigned int triId = 0; triId < neighborIndices.size(); ++triId )
    {
        normal += getTriangleNormal( neighborIndices[triId] );
    }
    normal *= 1./neighborIndices.size();
    return normal;
}

wmath::WVector3D WTriangleMesh::getVertexNormal( size_t i ) const
{
    assert( m_computedVertNormals );
    return m_vertNormals[i];
}

wmath::WVector3D WTriangleMesh::getTriangleNormal( size_t i ) const
{
    Triangle tri = m_triangles[i];
    wmath::WVector3D v1 = m_vertices[tri.pointID[1]] - m_vertices[tri.pointID[0]];
    wmath::WVector3D v2 = m_vertices[tri.pointID[2]] - m_vertices[tri.pointID[0]];

    wmath::WVector3D tempNormal = v1.crossProduct( v2 );

    if( tempNormal.norm() < 1e-7 )
    {
        // If the morm of the vector is below a certain threshold
        // the triangle is either very small, or its edges are
        // congruent (which also means that the triangle is very
        // small or zero) the triangle will probably not be visible.
        // Thus we set its normal to zero.
        tempNormal = wmath::WVector3D( 0, 0, 0 );
    }
    else
    {
        tempNormal.normalize();
    }

    return tempNormal;
}

void WTriangleMesh::computeTriNormals()
{
    if( m_computedTriNormals )
    {
        return;
    }

    m_triNormals.reserve( m_triangles.size() );
    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        m_triNormals.push_back( getTriangleNormal( triId ) );
    }

    m_computedTriNormals = true;
}

void WTriangleMesh::computeVertNormals()
{
    if( m_computedVertNormals )
    {
        return;
    }

    computeTriNormals();
    assert( m_computedTriNormals );

    //build neighborhood
    std::vector< std::vector< unsigned int > > neighborIds( m_vertices.size() );

    for( unsigned int triId = 0; triId < m_triangles.size(); ++triId )
    {
        for( unsigned int j = 0; j < 3; ++j )
        {
            neighborIds[m_triangles[triId].pointID[j]].push_back( triId );
        }
    }

    m_vertNormals.reserve( m_vertices.size() );

    for( unsigned int vertId = 0; vertId < m_vertices.size(); ++vertId )
    {
        wmath::WVector3D tempNormal( 0, 0, 0 );
        for( unsigned int neighId = 0; neighId < neighborIds[vertId].size(); ++neighId )
        {
            tempNormal += m_triNormals[neighborIds[vertId][neighId]];
        }
        tempNormal *= 1./neighborIds.size();
        if( tempNormal.norm() < 1e-7 )
        {
            // If the morm of the vector is below a certain threshold
            // the triangle is either very small, or its edges are
            // congruent (which also means that the triangle is very
            // small or zero) the triangle will probably not be visible.
            // Thus we set its normal to zero.
            tempNormal = wmath::WVector3D( 0, 0, 0 );
        }
        else
        {
            tempNormal.normalize();
        }

        m_vertNormals.push_back( tempNormal );
    }

    m_computedVertNormals = true;
}

std::ostream& tm_utils::operator<<( std::ostream& os, const WTriangleMesh& rhs )
{
    std::stringstream ss;
    ss << "WTriangleMesh( #vertices=" << rhs.getNumVertices() << " #triangles=" << rhs.getNumTriangles() << " )" << std::endl;
    using string_utils::operator<<;
    size_t count = 0;
    ss << std::endl;
    const std::vector< Triangle > triangles = rhs.getTriangles();
    const std::vector< wmath::WPosition > vertices = rhs.getVertices();
    for( std::vector< Triangle >::const_iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle, ++count )
    {
        std::stringstream prefix;
        prefix << "triangle: " << count << "[ ";
        std::string indent( prefix.str().size(), ' ' );
        ss << prefix.str() << vertices[ triangle->pointID[0] ] << std::endl;
        ss << indent << vertices[ triangle->pointID[1] ] << std::endl;
        ss << indent << vertices[ triangle->pointID[2] ] << std::endl;
        ss << std::string( indent.size() - 2, ' ' ) << "]" << std::endl;
    }
    return os << ss.str();
}

boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > tm_utils::componentDecomposition( const WTriangleMesh& mesh )
{
    WUnionFind uf( mesh.getNumVertices() ); // idea: every vertex in own component, then successivley join in accordance with the triangles

    const std::vector< Triangle >& triangles = mesh.getTriangles();
    for( std::vector< Triangle >::const_iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle )
    {
        uf.merge( triangle->pointID[0], triangle->pointID[1] );
        uf.merge( triangle->pointID[0], triangle->pointID[2] ); // uf.merge( triangle->pointID[2], triangle->pointID[1] ); they are already in same
    }

    // ATTENTION: The reason for using the complex BucketType instead of pasting vertices directly into a new WTriangleMesh
    // is performance! For example: If there are many vertices reused inside the former WTriangleMesh mesh, then we want
    // to reuse them in the new components too. Hence we must determine if a certain vertex is already inside the new component.
    // Since the vertices are organized in a vector, we can use std::find( v.begin, v.end(), vertexToLookUp ) which results
    // in O(N^2) or we could use faster lookUp via key and value leading to the map and the somehow complicated BucketType.
    typedef std::map< wmath::WPosition, size_t > VertexType; // look up fast if a vertex is already inside the new mesh!
    typedef std::vector< Triangle > TriangleType;
    typedef std::pair< VertexType, TriangleType > BucketType; // Later on the Bucket will be transformed into the new WTriangleMesh component
    std::map< size_t, BucketType > buckets; // Key identify with the cannonical element from UnionFind the new connected component

    const std::vector< wmath::WPosition >& vertices = mesh.getVertices();
    for( std::vector< Triangle >::const_iterator triangle = triangles.begin(); triangle != triangles.end(); ++triangle )
    {
        size_t component = uf.find( triangle->pointID[0] );
        if( buckets.find( component ) == buckets.end() )
        {
            buckets[ component ] = BucketType( VertexType(), TriangleType() ); // create new bucket
        }

        // Note: We discard the order of the points and indices, but semantically the structure remains the same
        VertexType& mapRef = buckets[ component ].first; // short hand alias
        Triangle x = { { 0, 0, 0 } }; // NOLINT
        for( int i = 0; i < 3; ++i )
        {
            size_t id = 0;
            const wmath::WPosition& vertex = vertices[ triangle->pointID[i] ];
            if( mapRef.find( vertex ) == mapRef.end() )
            {
                id = mapRef.size(); // since size might change in next line
                mapRef[ vertex ] = id;
            }
            else
            {
                id = mapRef[ vertex ];
            }
            x.pointID[i] = id;
        }

        buckets[ component ].second.push_back( x );
    }

    boost::shared_ptr< std::list< boost::shared_ptr< WTriangleMesh > > > result( new std::list< boost::shared_ptr< WTriangleMesh > >() );
    for( std::map< size_t, BucketType >::const_iterator cit = buckets.begin(); cit != buckets.end(); ++cit )
    {
        std::vector< wmath::WPosition > newVertices;
        newVertices.resize( cit->second.first.size() );
        for( VertexType::const_iterator vit = cit->second.first.begin(); vit != cit->second.first.end(); ++vit )
        {
            newVertices.at( vit->second ) = vit->first; // if you are sure that vit->second is always valid replace at() call with operator[]
        }
        boost::shared_ptr< WTriangleMesh > newMesh( new WTriangleMesh() );
        newMesh->resizeVertices( newVertices.size() );
        newMesh->setVertices( newVertices );
        newMesh->resizeTriangles( cit->second.second.size() );
        newMesh->setTriangles( cit->second.second );
        result->push_back( newMesh );
    }

    return result;
}
