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
 * VertexFactory.h
 *
 *  Created on: 15.06.2013
 *      Author: renegade
 */

#ifndef VERTEXFACTORY_H_
#define VERTEXFACTORY_H_

#include <vector>
#include "VertexProperty.h"
#include "core/graphicsEngine/WTriangleMesh.h"
#include "osg/Vec3"

namespace butterfly
{
    class VertexFactory
    {
    public:
        explicit VertexFactory( boost::shared_ptr< WTriangleMesh > triangleMesh );
        VertexFactory();
        virtual ~VertexFactory();
        void resize( size_t count );
        void registerTriangle( size_t vertexID, size_t triangleID );
        vector<size_t> getAttachedTriangleIDs( size_t triangleID, size_t vertex1, size_t vertex2 );
        size_t get3rdVertexID( size_t triangleID, size_t vertex1, size_t vertex2 );
        size_t getVertexCount();
        size_t getTriangleCount( size_t vertexID );
        size_t getTriangleID( size_t vertexID, int triangleIndex );
        int getNeighbourIndex( size_t vertexID, size_t neighbourID );
        bool neighborExists( size_t fromID, size_t toID );
        void attachMid( size_t fromID, size_t toID, size_t midID );
        VertexProperty* getVertexProperty( size_t vertexID );
        long getMidID( size_t fromID, size_t toID ); // NOLINT
        void examineCircularityAll();
        size_t getNeighbourVertexCount( size_t vertexID );
        size_t getBoundCountClass( size_t vertexID );
    private:
        std::vector<VertexProperty*> vertProp;
        boost::shared_ptr< WTriangleMesh > triangleMesh;
        size_t vertexCount;
    };
} /* namespace butterfly */
#endif  // VERTEXFACTORY_H
