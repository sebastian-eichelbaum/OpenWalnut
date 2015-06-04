//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
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

#include "WVertexFactory.h"


namespace butterfly
{
    WVertexFactory::WVertexFactory()
    {
        m_vertexCount = 0;
        m_triangleCount = 0;
    }

    WVertexFactory::WVertexFactory( boost::shared_ptr< WTriangleMesh > triangleMesh )
    {
        this->m_triangleMesh = triangleMesh;
        m_vertexCount = 0;
        m_triangleCount = 0;
    }

    WVertexFactory::~WVertexFactory()
    {
        //TODO(schwarzkopf): Memory is still not freed
        for  ( size_t index = 0; index < m_vertProp.size(); index++ )
            m_vertProp[index]->~WVertexProperty();

        m_vertProp.resize( 0 );
        m_vertProp.reserve( 0 );
    }
    void WVertexFactory::setTriangleMesh( boost::shared_ptr< WTriangleMesh > triangleMesh )
    {
        this->m_triangleMesh = triangleMesh;
    }

    void WVertexFactory::resize( size_t finalSize )
    {
        m_vertexCount = finalSize;
        size_t size = this->m_vertProp.size();
        if  ( finalSize >= size )
        {
            this->m_vertProp.reserve( finalSize * 3 / 2 );
            this->m_vertProp.resize( finalSize * 3 / 2 );

            for  ( size_t vertID = size; vertID < finalSize * 3 / 2; vertID++ )
                this->m_vertProp[vertID] = new WVertexProperty();
        }
    }

    void WVertexFactory::registerTriangle( size_t vertexID, size_t triangleID )
    {
        if  ( vertexID >= m_vertexCount )
            resize( vertexID+1 );

        this->m_vertProp[vertexID]->attachTriangle( triangleID );
        if  ( triangleID >= m_triangleCount )
            m_triangleCount = triangleID;
    }

    size_t WVertexFactory::getVertexCount()
    {
        return m_vertexCount;
    }

    vector<size_t> WVertexFactory::getAttachedTriangleIDs( size_t excludedTriangleID, size_t vert1, size_t vert2 )
    {
        vector<size_t> tris;
        size_t thirdID = excludedTriangleID >= m_triangleCount ?-1
                :get3rdVertexID( excludedTriangleID, vert1, vert2 );

        int sizeN = m_vertProp[vert1]->getTriangleCount();
        for  ( int neighbour = 0; vert1 != vert2 && neighbour < sizeN; neighbour++ )
        {
            size_t newTriID = m_vertProp[vert1]->getAttachedTriangles() [neighbour];
            size_t newTriVertID = get3rdVertexID( newTriID, vert1, vert2 );
            if( newTriID != excludedTriangleID && this->m_vertProp[vert2]->containsTriangle( newTriID )
                    && isValidTriangle( newTriID ) && thirdID != newTriVertID )
            {
                int size = tris.size();
                tris.reserve( size + 1 );
                tris.resize( size + 1 );
                tris[size] = newTriID;
            }
        }
        return tris;
    }

    size_t WVertexFactory::get3rdVertexID( size_t triangleID, size_t vert1, size_t vert2 )
    {
        size_t id0 = m_triangleMesh->getTriVertId0( triangleID ),
            id1 = m_triangleMesh->getTriVertId1( triangleID ),
            id2 = m_triangleMesh->getTriVertId2( triangleID );
        if  ( vert1 != id0 && vert2 != id0 )
            return id0;

        if  ( vert1 != id1 && vert2 != id1 )
            return id1;

        return id2;
    }
    bool WVertexFactory::newVertexExists( size_t vertID1, size_t vertID2 )
    {
        if( vertID1 == vertID2 )
            return true;

        if( vertID1 > vertID2 )
        {
            size_t tmp = vertID1;
            vertID1 = vertID2;
            vertID2 = tmp;
        }
        return this->m_vertProp[vertID1]->newVertexExists( vertID2 );
    }

    bool WVertexFactory::isValidTriangle( size_t triangleID )
    {
        size_t id0 = m_triangleMesh->getTriVertId0( triangleID );
        size_t id1 = m_triangleMesh->getTriVertId1( triangleID );
        size_t id2 = m_triangleMesh->getTriVertId2( triangleID );
        return id0 != id1 && id0 != id2 && id1 != id2;
    }
    void WVertexFactory::attachNewVertex( size_t vertex1, size_t vertex2 )
    {
        if  ( vertex1 == vertex2 )
            return;

        if  ( vertex1 > vertex2 )
        {
            size_t tmp = vertex1;
            vertex1 = vertex2;
            vertex2 = tmp;
        }
        this->m_vertProp[vertex1]->attachNewVertex( vertex2 );
    }

    WVertexProperty* WVertexFactory::getProperty( size_t vertexID )
    {
        return this->m_vertProp[vertexID];
    }
    WNewVertex* WVertexFactory::getNewVertexProperty( size_t vertex1, size_t vertex2 )
    {
        if  ( vertex1 == vertex2 )
        {
            WNewVertex* invalidPoint = new WNewVertex( vertex2 );
            invalidPoint->setValid( false );
            return invalidPoint;
        }
        if  ( vertex1 > vertex2 )
        {
            size_t tmp = vertex1;
            vertex1 = vertex2;
            vertex2 = tmp;
        }
        return m_vertProp[vertex1]->getNewVertexProperty( vertex2 );
    }
    void WVertexFactory::examineStencilRange( size_t fromVertex, size_t toVertex,
            boost::shared_ptr< WProgress > progress )
    {
        for  ( size_t vert = fromVertex; vert <= toVertex; vert++ )
        {
            vector<size_t> attachedTriangles = this->m_vertProp[vert]->getAttachedTriangles();
            size_t totalTriangleCount = 0;
            for  ( size_t triangle = 0; triangle < attachedTriangles.size(); triangle++ )
                if  ( isValidTriangle( attachedTriangles[triangle] ) )
                    totalTriangleCount++;

            progress->increment( 1 );
            size_t startTriID = 0;
            while  (  startTriID < attachedTriangles.size() && !isValidTriangle(attachedTriangles[startTriID] ) )
                startTriID++;

            if  ( startTriID < attachedTriangles.size() )
            {
                size_t firstTri = attachedTriangles[startTriID],
                    vertID0 = this->m_triangleMesh->getTriVertId0( firstTri ),
                    vertBound = ( vertID0 != vert ? vertID0 : this->m_triangleMesh->getTriVertId1( firstTri ) ),
                    firstVert = vertBound;
                vector<size_t> tris = getAttachedTriangleIDs( -1, vert, firstVert );
                bool moreThanTwo = tris.size() > 2;
                size_t firstDirection = tris[0];
                size_t ranTriangles = 0;
                WVertexProperty* property = m_vertProp[vert];
                do
                {
                    ranTriangles++;
                    property->addStencilNeighbourIDToBack( vertBound );
                    vertBound = get3rdVertexID( tris[0], vert, vertBound );
                    tris = getAttachedTriangleIDs( tris[0], vert, vertBound );
                } while( vertBound != firstVert && tris.size() == 1 );

                if  ( vertBound != firstVert && tris.size() < 2 )
                {
                    property->addStencilNeighbourIDToBack( vertBound );
                    vertBound = firstVert;
                    tris = getAttachedTriangleIDs( firstDirection, vert, vertBound );
                    while  ( tris.size() == 1 )
                    {
                        ranTriangles++;
                        vertBound = get3rdVertexID( tris[0], vert, vertBound );
                        property->insertStencilNeighbourIDToFront( vertBound );
                        tris = getAttachedTriangleIDs( tris[0], vert, vertBound );
                    };
                    vertBound = -1;
                }
                if  ( tris.size() > 1)
                    moreThanTwo = true;
                if  ( vertBound == firstVert && !moreThanTwo && ranTriangles == totalTriangleCount )
                {
                    //TODO(schwarzkopf): Two different surfaces hitting in one point are still able to be calculated as such
                    this->m_vertProp[vert]->setBoundClass( 0 );
                }
                else
                {
                    this->m_vertProp[vert]->setBoundClass( ( moreThanTwo || ranTriangles != totalTriangleCount ) ? 2 :1 );
                    if( !moreThanTwo && ranTriangles>totalTriangleCount )
                        m_vertProp[vert]->setBoundClass( -1 );
                }
            }
        }
    }
    float WVertexFactory::getDistance( osg::Vec3 point1, osg::Vec3 point2 )
    {
        float deltaX2 = pow( point2.x() - point1.x(), 2.0f );
        float deltaY2 = pow( point2.y() - point1.y(), 2.0f );
        float deltaZ2 = pow( point2.z() - point1.z(), 2.0f );
        return pow( deltaX2 + deltaY2 + deltaZ2, 0.5f );
    }
    osg::Vec3 WVertexFactory::add( osg::Vec3 base, osg::Vec3 summand, float factor )
    {
        float baseX = base.x(), baseY = base.y(), baseZ = base.z();
        float sumX = summand.x(), sumY = summand.y(), sumZ = summand.z();
        float coordX = baseX + sumX * factor;
        float coordY = baseY + sumY * factor;
        float coordZ = baseZ + sumZ * factor;
        return osg::Vec3( coordX, coordY, coordZ );
    }

} /* namespace butterfly */
