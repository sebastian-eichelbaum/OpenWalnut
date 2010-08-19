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
    m_width = 100.0;
    m_heigth = 50.0;

    QLinearGradient lg( 0, 0, 0, 50 );
    lg.setColorAt( 0.0, Qt::gray );
    lg.setColorAt( 0.5, Qt::black );
    lg.setColorAt( 1.0, Qt::gray );

    setRect( 0.0, 0.0, m_width, m_heigth );
    setBrush( lg );
    setPen( QPen( Qt::white, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    setAcceptsHoverEvents( false );
    setFlag( QGraphicsItem::ItemIsMovable );

    fitLook();
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
    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->update();
    }
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->update();
    }
}

//void WQtNetworkItem::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
//{
//    QGraphicsItem::mouseReleaseEvent( mouseEvent );
//}

//QVariant WQtNetworkItem::itemChange( GraphicsItemChange change,
//        const QVariant &value )
//{
//    if (change == QGraphicsItem::ItemPositionChange) {
//        foreach ( WQtNetworkPort *port, m_ports ) {
//            port->update();
//            std::cout << "update" << std::endl;
//        }
//    }
//
//    std::cout << "verschoben" << std::endl;
//
//    return value;
//}

void WQtNetworkItem::addPort( WQtNetworkPort *port )
{
    if( port->portType() == false ) m_inPorts.append( port );
    else if( port->portType() == true ) m_outPorts.append( port );
}
/*
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
*/

QList< WQtNetworkPort *> WQtNetworkItem::getInPorts()
{
    return m_inPorts;
}

QList< WQtNetworkPort *> WQtNetworkItem::getOutPorts()
{
    return m_outPorts;
}

void WQtNetworkItem::fitLook()
{
    int portNumber = 1;
    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->alignPosition( m_inPorts.size(), portNumber, m_width, false );
        portNumber++;
    }
    
    portNumber = 1;
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->alignPosition( m_outPorts.size(), portNumber, m_width, true );
        portNumber++;
    }
}
