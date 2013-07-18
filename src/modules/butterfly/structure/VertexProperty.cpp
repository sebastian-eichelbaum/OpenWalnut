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
 * PointProperty.cpp
 *
 *  Created on: 12.06.2013
 *      Author: renegade
 */

#include <vector>

#include "VertexProperty.h"

VertexProperty::VertexProperty()
{
    x=-1, y=-1, z=-1;
}

VertexProperty::~VertexProperty()
{
}

size_t VertexProperty::getTriangleCount()
{
    return attachedTriangle.size();
}

osg::Vec3 VertexProperty::getCoords()
{
    return osg::Vec3( this->x, this->y, this->z );
}

std::vector<size_t> VertexProperty::getAttachedTriangles()
{
    return attachedTriangle;
}

vector<size_t> VertexProperty::getNeighbourVertices()
{
    return neighbourVertices;
}

void VertexProperty::setNeighbourVertices( vector<size_t> vertices )
{
    this->neighbourVertices = vertices;
}

int VertexProperty::getNeighbourVertexCount()
{
    return neighbourVertices.size();
}

int VertexProperty::getBoundCountClass()
{
    return boundCountClass;
}
void VertexProperty::setBoundCountClass( int boundCount )
{
    boundCountClass = boundCount;
}

void VertexProperty::registerTriangle( size_t idx )
{
    int size = attachedTriangle.size();
    attachedTriangle.reserve( size + 1 );
    attachedTriangle.resize( size + 1 );
    attachedTriangle[size] = idx;
}

bool VertexProperty::containsTriangle( size_t triangleID )
{
    for( size_t i = 0; i < getTriangleCount(); i++ )
    {
        size_t id = attachedTriangle[i];
        if( id == triangleID )
            return true;
    }
    return false;
}
void VertexProperty::attachMid( size_t toID, size_t midID )
{
    size_t idx = 0;
    while( idx < this->attachedMid.size() && this->attachedMid[idx]->getToID() != toID )
        idx++;

    if( idx >= attachedMid.size() )
    {
        unsigned int oldSize = attachedMid.size();
        attachedMid.reserve( oldSize + 1 );
        attachedMid.resize( oldSize + 1 );
        unsigned int newSize = attachedMid.size();
        attachedMid[newSize-1] = new MidPoint( toID, midID );
    }
}

bool VertexProperty::toExists( size_t toID )
{
    for( size_t i = 0; i < this->attachedMid.size(); i++ )
        if( this->attachedMid[i]->getToID() == toID )
            return true;
    return false;
}

size_t VertexProperty::getMidID( size_t toID )
{
    unsigned int size = this->attachedMid.size();
    for( unsigned int i = 0; i < size; i++ )
        if( this->attachedMid[i]->getToID() == toID )
            return attachedMid[i]->getMidID();
    return -1;
}


