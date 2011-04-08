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

#include <QtCore/QPointF>

#include "WNetworkLayoutGlobals.h"

#include "WNetworkLayoutItem.h"

WNetworkLayoutItem::WNetworkLayoutItem( WQtNetworkItem *item )
{
    m_item = item;
    m_width = m_item->m_width * 1.2; // 20% is boarder
    m_parent = NULL;
    m_neighbour = NULL;
    m_item->m_layoutItem = this;
}

WNetworkLayoutItem::~WNetworkLayoutItem()
{
    for( std::list< WNetworkLayoutItem * >::iterator iter = children.begin(); iter != children.end(); ++iter )
    {
        delete *iter;
    }
    m_item->m_layoutItem = NULL;
}

void WNetworkLayoutItem::addChild( WNetworkLayoutItem *child, float width )
{
    children.push_back( child );
    if( children.size() > 1 )
    {
        m_width += width;
        updateLayoutPos();
    }
    else
    {
        if( m_width != width )
        {
            m_width = width;
            updateLayoutPos();
        }
    }
}

void WNetworkLayoutItem::disconnectFromParent( QPointF newPos )
{
    m_parent->removeChild( this );
    m_parent = NULL;
    setPos( newPos );
}

WNetworkLayoutItem * WNetworkLayoutItem::getParent()
{
    return m_parent;
}

WNetworkLayoutItem * WNetworkLayoutItem::getNeighbour()
{
    return m_neighbour;
}

QPointF WNetworkLayoutItem::getNewChildPos()
{
    QPointF newPos;
    newPos.setY( m_pos.y() + ( 2 * m_item->m_height ) );
    if( children.size() == 0 )
    {
        newPos.setX( m_pos.x() );
    }
    else
    {
        newPos.setX( m_pos.x() + m_width );
    }
    return newPos;
}

QPointF WNetworkLayoutItem::getPos()
{
    return m_pos;
}

float WNetworkLayoutItem::getWidth()
{
    return m_width;
}

void WNetworkLayoutItem::setParent( WNetworkLayoutItem *parent )
{
    if( m_parent != NULL )
    {
        float width = m_parent->getWidth();
        width += parent->getWidth();
        // TODO(rfrohl): we have two parents ...
    }
    else
    {
        m_parent = parent;
        setPos( m_parent->getNewChildPos() );
        m_parent->addChild( this, m_width );
    }

    // dirty hack: update arrow position
    foreach( WQtNetworkPort *port, m_item->m_inPorts )
    {
        port->updateArrows();
    }
    // remove comment when moving a whole graph instead of one node
    //foreach( WQtNetworkPort *port, m_item->m_outPorts )
    //{
    //    port->updateArrows();
    //}
}

void WNetworkLayoutItem::setPos( QPointF pos )
{
    if( m_pos != pos )
    {
        m_pos = pos;
        if( m_neighbour )
        {
            QPointF newPos( m_pos );
            newPos.setX( m_pos.x() + m_width );
            m_neighbour->setPos( newPos );
        }
        QPointF childrenPos;
        childrenPos.setY( m_pos.y() - ( 2 * m_item->m_height ) );
        for( std::list< WNetworkLayoutItem * >::iterator iter = children.begin();
                iter != children.end(); ++iter )
        {
            ( *iter )->setPos( childrenPos );
            childrenPos.setX( childrenPos.x() + ( *iter )->getWidth() );
        }

        // update the position in the layout
        QPointF newPos( m_pos );
        newPos.setX( m_pos.x() + 0.5 * ( m_width - m_item->m_width ) );
        m_item->setPos( newPos );
    }
}

void WNetworkLayoutItem::setNeighbour( WNetworkLayoutItem *neighbour )
{
    m_neighbour = neighbour;
}

void WNetworkLayoutItem::removeChild( WNetworkLayoutItem *child )
{
    children.remove( child );
    updateWidth();
}

void WNetworkLayoutItem::updateWidth()
{
    float newWidth = 0;
    for( std::list< WNetworkLayoutItem * >::iterator child = children.begin(); child != children.end(); ++child )
    {
        newWidth += ( *child )->getWidth();
        // set child pos ???
    }
    if( newWidth > m_width )
    {
        // update item pos
        m_width = newWidth;
        updateLayoutPos();
    }
}

// private

void WNetworkLayoutItem::updateLayoutPos()
{
    if( m_parent )
    {
        m_parent->updateWidth();
    }
    else
    {
        if( m_neighbour )
        {
            QPointF newPos( m_pos );
            newPos.setX( m_pos.x() + m_width );
            m_neighbour->setPos( newPos );
        }
    }
    QPointF newPos( m_pos );
    newPos.setX( m_pos.x() + 0.5 * ( m_width - m_item->m_width ) );
    m_item->setPos( newPos );
}

