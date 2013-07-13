//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2013 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

/*
 * VertexFactory.cpp
 *
 *  Created on: 15.06.2013
 *      Author: renegade
 */

#include <vector>

#include "VertexFactory.h"

namespace butterfly
{
    VertexFactory::VertexFactory()
    {
        vertexCount = 0;
    }

    VertexFactory::VertexFactory( boost::shared_ptr< WTriangleMesh > triangleMesh )
    {
        this->triangleMesh = triangleMesh;
        vertexCount = 0;
    }

    VertexFactory::~VertexFactory()
    {
        // TODO(schwarzkopf): Auto-generated destructor stub
    }

    void VertexFactory::resize( size_t finalSize )
    {
        int size = this->vertProp.size();
        this->vertProp.reserve( finalSize );
        this->vertProp.resize( finalSize );
        for( size_t i = size; i < finalSize; i++ )
            this->vertProp[i] = new VertexProperty();
    }

    void VertexFactory::registerTriangle( size_t vertexID, size_t triangleID )
    {
        if( (size_t)vertexID >= vertProp.size() )
            resize( vertexID + 1 );
        this->vertProp[vertexID]->registerTriangle( triangleID );
    }

    size_t VertexFactory::getVertexCount()
    {
        return this->vertProp.size();
    }

    size_t VertexFactory::getTriangleCount( size_t vertexID )
    {
        return this->vertProp[vertexID]->getTriangleCount();
    }

    size_t VertexFactory::getTriangleID( size_t vertID, int triangleIndex )
    {
        return ( this->vertProp[vertID]->getAttachedTriangles() )[triangleIndex];
    }

    vector<size_t> VertexFactory::getAttachedTriangleIDs( size_t triangleID, size_t vert1, size_t vert2 )
    {
        vector<size_t> tris;
        int sizeN = getTriangleCount( vert1 );
        for( int i = 0; i < sizeN; i++ )
        {
            size_t newTriID = getTriangleID( vert1, i );
            if( newTriID != triangleID && this->vertProp[vert2]->containsTriangle( newTriID ) )
            {
                int size = tris.size();
                tris.reserve( size + 1 );
                tris.resize( size + 1 );
                tris[size] = newTriID;
            }
        }
        return tris;
    }

    size_t VertexFactory::get3rdVertexID( size_t triangleID, size_t vert1, size_t vert2 )
    {
        size_t id0 = triangleMesh->getTriVertId0( triangleID ),
            id1 = triangleMesh->getTriVertId1( triangleID ),
            id2 = triangleMesh->getTriVertId2( triangleID );
        if( vert1 != id0 && vert2 != id0 )
            return id0;
        if( vert1 != id1 && vert2 != id1 )
            return id1;
        return id2;
    }

    // TODO(schwarzkopf): Method isTriangleAttached
    size_t VertexFactory::getNeighbourIndex( size_t vertexID, size_t neighbourID )
    {
        VertexProperty* prop = this->vertProp[vertexID];
        for( int i = 0; i < prop->getNeighbourVertexCount(); i++ )
            if( prop->getNeighbourVertices()[i] == neighbourID )
                return i;
        return -1;
    }

    bool VertexFactory::neighborExists( size_t fromID, size_t toID )
    {
        if( fromID == toID )
            return true;
        if( fromID > toID )
        {
            size_t tmp = fromID;
            fromID = toID;
            toID = tmp;
        }
        return this->vertProp[fromID]->toExists(toID);
    }

    void VertexFactory::attachMid( size_t fromID, size_t toID, size_t midID )
    {
        if( fromID == toID )
            return;
        if( fromID > toID )
        {
            size_t tmp = fromID;
            fromID = toID;
            toID = tmp;
        }
        this->vertProp[fromID]->attachMid( toID, midID );
    }

    VertexProperty* VertexFactory::getVertexProperty( size_t vertexID )
    {
        return this->vertProp[vertexID];
    }

    size_t VertexFactory::getMidID( size_t fromID, size_t toID )
    {
        if( fromID == toID )
            return fromID;
        if( fromID > toID )
        {
            size_t tmp = fromID;
            fromID = toID;
            toID = tmp;
        }
        return vertProp[fromID]->getMidID( toID );
    }

    void VertexFactory::examineCircularityAll()
    {
        for( size_t vert = 0; vert < this->vertProp.size(); vert++ )
        {
            size_t firstTri = this->vertProp[vert]->getAttachedTriangles()[0],
                id0 = this->triangleMesh->getTriVertId0( firstTri ),
                vertBound = ( id0 != vert ? id0 : this->triangleMesh->getTriVertId1( firstTri ) ),
                firstVert = vertBound;
            vector<size_t> tris = getAttachedTriangleIDs( -1, vert, firstVert ),
                neighbours;
            bool moreThanTwo = tris.size() > 2;
            size_t firstDirection = tris[0];
            size_t ranTriangles = 0;
            do
            {
                ranTriangles++;
                int size = neighbours.size();
                neighbours.reserve( size + 1 );
                neighbours.resize( size + 1 );
                neighbours[size] = vertBound;
                vertBound = get3rdVertexID( tris[0], vert, vertBound );
                tris = getAttachedTriangleIDs( tris[0], vert, vertBound );
            } while( vertBound != firstVert && tris.size() == 1 );

            if( vertBound != firstVert && tris.size() < 2 )
            {
                size_t size = neighbours.size() + 1;
                neighbours.reserve( size );
                neighbours.resize( size );
                neighbours[size-1] = vertBound;
                for( size_t i = 0; i < size/2; i++ )
                {
                    size_t tmp = neighbours[neighbours.size()-i-1];
                    neighbours[neighbours.size()-i-1] = neighbours[i];
                    neighbours[i] = tmp;
                }
                vertBound = firstVert;
                tris = getAttachedTriangleIDs( firstDirection, vert, vertBound );
                while( tris.size() == 1 )
                {
                    ranTriangles++;
                    vertBound = get3rdVertexID( tris[0], vert, vertBound );
                    int size = neighbours.size();
                    neighbours.reserve( size + 1 );
                    neighbours.resize( size + 1 );
                    neighbours[size] = vertBound;
                    tris = getAttachedTriangleIDs( tris[0], vert, vertBound );
                };
                vertBound = -1;
            }
            vertProp[vert]->setNeighbourVertices( neighbours );
            if( tris.size() > 1)
                moreThanTwo = true;
            if( vertBound == firstVert && !moreThanTwo )
            {
                this->vertProp[vert]->setBoundCountClass( 0 );
            }
            else
            {
                this->vertProp[vert]->setBoundCountClass( ( moreThanTwo || ranTriangles != vertProp[vert]->getTriangleCount() ) ? 2 :1 );
                if( !moreThanTwo && ranTriangles>vertProp[vert]->getTriangleCount() )
                    vertProp[vert]->setBoundCountClass( -1 );
            }
        }
    }

    size_t VertexFactory::getNeighbourVertexCount( size_t vertexID )
    {
        return vertProp[vertexID]->getNeighbourVertexCount();
    }

    size_t VertexFactory::getBoundCountClass( size_t vertexID )
    {
        return vertProp[vertexID]->getBoundCountClass();
    }

} /* namespace butterfly */
