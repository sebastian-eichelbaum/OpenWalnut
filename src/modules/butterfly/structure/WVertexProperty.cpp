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

#include <vector>

#include "WVertexProperty.h"


WVertexProperty::WVertexProperty()
{
    m_boundClass = 0;
    m_maxNeighbourDistance = 0.0f;
    m_sumNeighbourDistance = 0.0f;
}

WVertexProperty::~WVertexProperty()
{
    //TODO(schwarzkopf): Memory is still not freed as it should
    m_attachedTriangle.resize( 0 );
    m_attachedTriangle.reserve( 0 );
    for  ( size_t index = 0; index < m_newVertices.size(); index++ )
    {
        m_newVertices[index]->~WNewVertex();
        delete m_newVertices[index];
    }
    m_newVertices.resize( 0 );
    m_newVertices.reserve( 0 );
    m_stencilOnNeighbours.resize( 0 );
    m_stencilOnNeighbours.reserve( 0 );
}

size_t WVertexProperty::getTriangleCount()
{
    return m_attachedTriangle.size();
}

std::vector<size_t> WVertexProperty::getAttachedTriangles()
{
    return m_attachedTriangle;
}

size_t WVertexProperty::getStencilNeighbourID( size_t index )
{
    return m_stencilOnNeighbours[index];
}

void WVertexProperty::insertStencilNeighbourIDToFront( size_t stencilNeighbourID )
{
    addStencilNeighbourIDToBack( 0 );
    for  ( size_t index = m_stencilOnNeighbours.size() - 1; index > 0; index-- )
        m_stencilOnNeighbours[index] = m_stencilOnNeighbours[index - 1];
    m_stencilOnNeighbours[0] = stencilNeighbourID;
}
void WVertexProperty::addStencilNeighbourIDToBack( size_t stencilNeighbourID )
{
    int size = m_stencilOnNeighbours.size();
    m_stencilOnNeighbours.reserve( size + 1 );
    m_stencilOnNeighbours.resize( size + 1 );
    m_stencilOnNeighbours[size] = stencilNeighbourID;
}

size_t WVertexProperty::getValence()
{
    return m_stencilOnNeighbours.size();
}

int WVertexProperty::getBoundClass()
{
    return m_boundClass;
}
void WVertexProperty::setBoundClass( int boundClass )
{
    this->m_boundClass = boundClass;
}

void WVertexProperty::attachTriangle( size_t triangleID )
{
    size_t size = m_attachedTriangle.size();
    for  ( size_t index = 0; index < size; index++ )
        if  ( m_attachedTriangle[index] == triangleID )
            return;

    m_attachedTriangle.reserve( size + 1 );
    m_attachedTriangle.resize( size + 1 );
    m_attachedTriangle[size] = triangleID;
}
void WVertexProperty::detachTriangle( size_t triangleID )
{
    size_t size = m_attachedTriangle.size();
    for  ( size_t index = 0; index < size; index++ )
    {
        if  ( m_attachedTriangle[index] == triangleID )
        {
            for  ( size_t popIdx = index; popIdx < size-1; popIdx++ )
                m_attachedTriangle[popIdx] = m_attachedTriangle[popIdx + 1];

            size--;
            m_attachedTriangle.resize( size );
            m_attachedTriangle.reserve( size );
        }
    }
}

bool WVertexProperty::containsTriangle( size_t triangleID )
{
    for  ( size_t index = 0; index < getTriangleCount(); index++ )
    {
        size_t id = m_attachedTriangle[index];
        if  ( id == triangleID )
            return true;
    }
    return false;
}
void WVertexProperty::attachNewVertex( size_t toID )
{
    size_t index = 0;
    while  ( index < this->m_newVertices.size() && this->m_newVertices[index]->getToID() != toID )
        index++;

    if  ( index >= m_newVertices.size() )
    {
        size_t oldSize = m_newVertices.size();
        m_newVertices.reserve( oldSize + 1 );
        m_newVertices.resize( oldSize + 1 );
        size_t newSize = m_newVertices.size();
        m_newVertices[newSize-1] = new WNewVertex( toID );
    }
}

bool WVertexProperty::newVertexExists( size_t toID )
{
    for  ( size_t index = 0; index < this->m_newVertices.size(); index++ )
        if  ( this->m_newVertices[index]->getToID() == toID )
            return true;
    return false;
}
WNewVertex* WVertexProperty::getNewVertexProperty( size_t toID )
{
    unsigned int size = this->m_newVertices.size();
    for  ( unsigned int index = 0; index < size; index++ )
        if  ( this->m_newVertices[index]->getToID() == toID )
            return m_newVertices[index];

    return 0;
}
float WVertexProperty::getMaxNeighbourDistance()
{
    return this->m_maxNeighbourDistance;
}
float WVertexProperty::getSumNeighbourDistance()
{
    return this->m_sumNeighbourDistance;
}
void WVertexProperty::setMaxNeighbourDistance( float maxNeighbourDistance )
{
    this->m_maxNeighbourDistance = maxNeighbourDistance;
}
void WVertexProperty::setSumNeighbourDistance( float sumNeighbourDistance )
{
    this->m_sumNeighbourDistance = sumNeighbourDistance;
}

int WVertexProperty::getStencilNeighbourIndex( size_t neighbourID )
{
    for  ( size_t index = 0; index < getValence(); index++ )
        if  ( getStencilNeighbourID( index ) == neighbourID )
            return index;

    return -1;
}

std::vector<WNewVertex*> WVertexProperty::getNewVerticesToHigherNeighborID()
{
    return m_newVertices;
}
