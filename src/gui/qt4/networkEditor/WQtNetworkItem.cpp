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

const float MINWIDTH = 100;
const float MINHEIGHT = 50;

WQtNetworkItem::WQtNetworkItem()
    : QGraphicsRectItem()
{

    m_text=0;
    //m_width = 100;
    //m_height = 50;
    //setRect(0,0,100,50);

    //
    //

    setAcceptsHoverEvents( true );
    setFlag( QGraphicsItem::ItemIsMovable );

    fitLook();
}

void WQtNetworkItem::hoverEnterEvent(QGraphicsSceneHoverEvent  *event)
{
    if( m_color != Qt::darkBlue )
    {
        changeColor( Qt::darkBlue );
    }
    
    QString str = "Name: ";// + getName() + "\nDescription: ";
    if (toolTip() != str){
        setToolTip(str);
    }
}
void WQtNetworkItem::hoverLeaveEvent(QGraphicsSceneHoverEvent  *event)
{
    if( m_color != Qt::gray )
    {
        changeColor( Qt::gray );
    }
}

void WQtNetworkItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    if( isSelected() && m_color != Qt::darkGreen )
    {
        changeColor( Qt::darkGreen );
    }
 
    QGraphicsRectItem::paint( painter, option, 0 );

    //glass highlight
    //voreen
    painter->setBrush( QBrush( Qt::white ) );
    painter->setPen( QPen( QBrush( Qt::white ), 0.01 ) );
    painter->setOpacity(0.30);
    QRectF rect( 0, 0, m_width, m_height/2.0 );
    painter->drawRect( rect );

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

QVariant WQtNetworkItem::itemChange( GraphicsItemChange change,
        const QVariant &value )
{
    if( change == QGraphicsItem::ItemSelectedHasChanged )
    {
        changeColor( Qt::gray );     
    }
    return value;
}

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
    if( m_text != 0){  
        m_width = m_text->boundingRect().width() + 10;
        m_height = m_text->boundingRect().height() + 10;
        if( m_width < MINWIDTH ) m_width = MINWIDTH;
        if( m_height < MINHEIGHT ) m_height = MINHEIGHT;
        QRectF rect( 0, 0, m_width, m_height );
        m_rect = rect;
        setRect( m_rect );

        qreal x = ( m_width / 2.0 ) - ( m_text->boundingRect().width() / 2.0 );
        qreal y = ( m_height / 2.0 ) - ( m_text->boundingRect().height() / 2.0 );
        m_text->setPos( x, y );
    }

    int portNumber = 1;
    foreach( WQtNetworkPort *port, m_inPorts )
    {
        port->alignPosition( m_inPorts.size(), portNumber, m_rect, false );
        portNumber++;
    }
    
    portNumber = 1;
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->alignPosition( m_outPorts.size(), portNumber, m_rect, true );
        portNumber++;
    }

    changeColor( Qt::gray );
    
}

void WQtNetworkItem::setTextItem( QGraphicsTextItem *text )
{
    m_text = text ;
}

void WQtNetworkItem::changeColor( QColor color )
{
    m_color = color;
    m_gradient.setStart( 0, 0 );
    m_gradient.setFinalStop( 0, m_height );
    
    m_gradient.setColorAt( 0.0, m_color );
    m_gradient.setColorAt( 0.3, Qt::black );
    m_gradient.setColorAt( 0.7, Qt::black );
    m_gradient.setColorAt( 1.0, m_color );
    setBrush( m_gradient );
    setPen( QPen( m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
}
