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

#include "WNetworkLayoutNode.h"

WNetworkLayoutNode::WNetworkLayoutNode( WQtNetworkItem *item )
{
    m_referencedItem = item;
    m_referencedItem->m_layoutItem = this; // TODO: change friend in WQtNetworkItem
}

WNetworkLayoutNode::~WNetworkLayoutItem()
{
    m_referencedItem->m_layoutItem = NULL;
}

void WNetworkLayoutNode::add( WNetworkLayoutNode *node )
{
    m_referencedItems.push_back( node );
}

std::list< WNetworkLayoutNode* > * getChildren()
{
    return &m_referencedItems;
}

QPointF WNetworkLayoutItem::getGridPos()
{
    return m_gridPos;
}

void remove( WNetworkLayoutNode *node )
{
    std::list< WNetworkLayoutNode * >::iterator iter;
    iter = std::find( m_referencedItems.begin(), m_referencedItems.end(), node );
    if( iter != m_referencedItems.end() )
    {
        m_referencedItems.erase( iter );
    }
}

void WNetworkLayoutItem::setGridPos( QPointF pos )
{
    m_gridPos = pos;
    // calc positon for m_item
    QPointF newPos;
    newPos.setX( ( pos.x() * WNETWORKLAYOUT_GRID_DISTANCE_X ) + 0.5 * ( WNETWORKLAYOUT_GRID_DISTANCE_X - m_item->m_width ) );
    newPos.setY( pos.y() * WNETWORKLAYOUT_GRID_DISTANCE_Y );
    m_referencedItem->setPos( newPos );

    // dirty hack: update arrow position
    foreach( WQtNetworkPort *port, m_referencedItem->m_inPorts )
    {
        port->updateArrows();
    }
}

