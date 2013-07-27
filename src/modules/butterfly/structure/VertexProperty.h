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
 * PointProperty.h
 *
 *  Created on: 12.06.2013
 *      Author: renegade
 */

#ifndef VERTEXPROPERTY_H_
#define VERTEXPROPERTY_H_

#include <vector>
#include "MidPoint.h"
#include "osg/Vec3"



using namespace butterfly; //NOLINT
using std::vector;

class VertexProperty
{
public:
    VertexProperty();
    virtual ~VertexProperty();
    size_t getTriangleCount();
    void registerTriangle( size_t idx );
    std::vector<size_t> getAttachedTriangles();
    vector<size_t> getNeighbourVertices();
    void setNeighbourVertices( vector<size_t> vertices );
    int getNeighbourVertexCount();
    int getBoundCountClass();
    void setBoundCountClass( int boundCountClass );
    bool containsTriangle( size_t triangleID );
    void attachMid( size_t toID, size_t midID );
    long getMidID( size_t toID ); // NOLINT
    bool toExists( size_t toID );
    osg::Vec3 getCoords();


private:
    std::vector<size_t> attachedTriangle;
    std::vector<MidPoint*> attachedMid;
    float x, y, z;
    vector<size_t> neighbourVertices;
    int boundCountClass;
};

#endif  // VERTEXPROPERTY_H
