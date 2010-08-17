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

#include <string>
#include <iostream>

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsRectItem>

#include "WQtNetworkItem.h"
#include "WQtNetworkArrow.h"

WQtNetworkItem::WQtNetworkItem()
    : QGraphicsRectItem()
{
    QLinearGradient lg( 0, 0, 0, 50 );
    lg.setColorAt( 0.0, Qt::gray );
    lg.setColorAt( 0.5, Qt::black );
    lg.setColorAt( 1.0, Qt::gray );

    setRect( 0.0, 0.0, 100.0, 50.0 );
    setBrush( lg );
    setPen( QPen( Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    setAcceptsHoverEvents( false );
    setFlag( QGraphicsItem::ItemIsMovable );
}

WQtNetworkItem::~WQtNetworkItem()
{
}

//void WQtNetworkItem::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
//{
//    QGraphicsItem::mousePressEvent( mouseEvent );
//}

void WQtNetworkItem::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QGraphicsItem::mouseMoveEvent( mouseEvent );

     foreach( WQtNetworkPort *port, m_ports )
     {
        port->update();
        std::cout << "mousemove update" << std::endl;
    }
}

//void WQtNetworkItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
//{
//    QGraphicsItem::mouseReleaseEvent( mouseEvent );
//}

//QVariant WQtNetworkItem::itemChange( GraphicsItemChange change,
//        const QVariant &value )
//{
//     foreach ( WQtNetworkPort *port, m_ports ) {
//        port->update();
//        std::cout << "update" << std::endl;
//    }
//
//    std::cout << "verschoben" << std::endl;
//
//    return value;
//}

void WQtNetworkItem::addPort( WQtNetworkPort *port )
{
    m_ports.append( port );
}

void WQtNetworkItem::removePort( WQtNetworkPort *port )
{
    int index = m_ports.indexOf( port );

    if ( index != -1 )
        m_ports.removeAt( index );
}

void WQtNetworkItem::removePorts()
{
    foreach( WQtNetworkPort *port, m_ports )
    {
        port->update();
    }
}

