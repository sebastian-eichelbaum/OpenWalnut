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

#include "WNetworkLayoutGlobals.h"

#include "WNetworkLayoutNode.h"

WNetworkLayoutNode::WNetworkLayoutNode( WQtNetworkItem *item )
{
    m_referencedItem = item;
    if( item != NULL )
    {
        m_referencedItem->m_layoutNode = this;
        m_referencedItem->setPos( QPointF( 0, 0 ) );
    }
}

WNetworkLayoutNode::~WNetworkLayoutNode()
{
    if( m_referencedItem != NULL )
    {
        m_referencedItem->m_layoutNode = NULL;
        for( std::list< WNetworkLayoutNode * >::iterator iter = m_parents.begin();
                iter != m_parents.end(); ++iter )
        {
            ( *iter )->remove( this );
        }
    }
}

void WNetworkLayoutNode::add( WNetworkLayoutNode *node )
{
    m_children.push_back( node );
    node->addParent( this );
}

void WNetworkLayoutNode::addParent( WNetworkLayoutNode *node )
{
    m_parents.push_back( node );
}

std::list< WNetworkLayoutNode* > WNetworkLayoutNode::getChildren()
{
    return m_children;
}

QPointF WNetworkLayoutNode::getGridPos()
{
    return m_gridPos;
}

int WNetworkLayoutNode::nChildren()
{
    return m_children.size();
}

int WNetworkLayoutNode::nParents()
{
    return m_parents.size();
}

void WNetworkLayoutNode::remove( WNetworkLayoutNode *node )
{
    std::list< WNetworkLayoutNode * >::iterator iter;
    iter = std::find( m_children.begin(), m_children.end(), node );
    if( iter != m_children.end() )
    {
        m_children.erase( iter );
    }
    else
    {
        iter = std::find( m_parents.begin(), m_parents.end(), node );
        if( iter != m_children.end() )
        {
            m_parents.erase( iter );
        }
    }
}

void WNetworkLayoutNode::setGridPos( QPoint pos )
{
    if( m_referencedItem != NULL )
        // if this is a dummy node do nothing
    {
        m_gridPos = pos;
        // calc positon for m_item
        QPointF newPos;
        // TODO(rfrohl): look into this: x => y and y => x
        //newPos.setX( ( pos.y() * WNETWORKLAYOUT_GRID_DISTANCE_X ) +
        //        0.5 * ( WNETWORKLAYOUT_GRID_DISTANCE_X - m_referencedItem->m_width ) );
        //newPos.setY( pos.x() * WNETWORKLAYOUT_GRID_DISTANCE_Y );
        newPos.setX( ( pos.x() * WNETWORKLAYOUT_GRID_DISTANCE_X ) +
                0.5 * ( WNETWORKLAYOUT_GRID_DISTANCE_X - m_referencedItem->m_width ) );
        newPos.setY( pos.y() * WNETWORKLAYOUT_GRID_DISTANCE_Y );
        m_referencedItem->setPos( newPos );

        // dirty hack: update arrow position
        foreach( WQtNetworkPort *port, m_referencedItem->m_inPorts )
        {
            port->updateArrows();
        }
    }
}

