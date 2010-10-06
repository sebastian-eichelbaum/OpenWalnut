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
#include <QtGui/QStyleOptionGraphicsItem>

#include "../../../kernel/WKernel.h"

#include "WQtNetworkArrow.h"
#include "WQtNetworkItem.h"

const float MINWIDTH = 100;
const float MINHEIGHT = 50;

WQtNetworkItem::WQtNetworkItem( WModule *module )
    : QGraphicsRectItem()
{
    m_module = module;
    changeColor( Qt::white );

    //caption
    m_text = new QGraphicsTextItem( module->getName().c_str() );
    m_text->setParentItem( this );
    m_text->setDefaultTextColor( Qt::white );

    //add input ports
    WModule::InputConnectorList cons = module->getInputConnectors();
    for ( WModule::InputConnectorList::const_iterator iter = cons.begin(); iter != cons.end(); ++iter )
    {        
        WQtNetworkInputPort *port = new WQtNetworkInputPort( *iter );
        port->setParentItem( this );
        this->addInputPort( port );
    }
 
    //add output ports
    WModule::OutputConnectorList outCons = module->getOutputConnectors();
    for ( WModule::OutputConnectorList::const_iterator iter = outCons.begin(); iter != outCons.end(); ++iter )
    {        
        WQtNetworkOutputPort *port = new WQtNetworkOutputPort( *iter );
        port->setParentItem( this );
        this->addOutputPort( port );
    }
    

    fitLook();
}

WQtNetworkItem::~WQtNetworkItem()
{
    std::cout << "delete item" << std::endl;

    foreach( WQtNetworkPort *port, m_inPorts )
    {
        delete port;
    }

    foreach( WQtNetworkPort *port, m_outPorts )
    {
        delete port;
    }
}

void WQtNetworkItem::hoverEnterEvent( QGraphicsSceneHoverEvent  *event )
{
    if( m_color != Qt::darkBlue )
    {
        changeColor( Qt::darkBlue );
    }

    QString str = "Name: " + getText() + "\nDescription: ";
    if ( toolTip() != str )
    {
        setToolTip( str );
    }
}
void WQtNetworkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent *event )
{
    if( m_color != Qt::gray )
    {
        changeColor( Qt::gray );
    }
}

void WQtNetworkItem::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w )
{
    if( isSelected() && m_color != Qt::darkGreen )
    {
        changeColor( Qt::darkGreen );
    }

    QStyleOptionGraphicsItem *o = const_cast<QStyleOptionGraphicsItem*>( option );
    o->state &= ~QStyle::State_Selected;
    QGraphicsRectItem::paint( painter, o, w );

    //glass highlight
    //voreen
    painter->setBrush( QBrush( Qt::white ) );
    painter->setPen( QPen( QBrush( Qt::white ), 0.01 ) );
    painter->setOpacity( 0.30 );
    QRectF rect( 0, 0, m_width, m_height/2.0 );
    painter->drawRect( rect );
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
        port->updateArrows();
    }
    foreach( WQtNetworkPort *port, m_outPorts )
    {
        port->updateArrows();
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

//TODO
void WQtNetworkItem::addInputPort( WQtNetworkInputPort *port )
{
    m_inPorts.append( port );
}

void WQtNetworkItem::addOutputPort( WQtNetworkOutputPort * port )
{
    m_outPorts.append( port );
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

QList< WQtNetworkInputPort *> WQtNetworkItem::getInPorts()
{
    return m_inPorts;
}

QList< WQtNetworkOutputPort *> WQtNetworkItem::getOutPorts()
{
    return m_outPorts;
}

void WQtNetworkItem::fitLook()
{
    if( m_text != 0)
    {
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
    m_text = text;
}

QString WQtNetworkItem::getText()
{
    return m_text->toPlainText();
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

WModule* WQtNetworkItem::getModule()
{
    return m_module;
}

void WQtNetworkItem::activate( bool active )
{
    if( active == true )
    {
        setAcceptsHoverEvents( true );
        setFlag( QGraphicsItem::ItemIsSelectable );
        setFlag( QGraphicsItem::ItemIsMovable );
        changeColor( Qt::gray );
    }
    if( active == false )
    {
        setAcceptsHoverEvents( false );
        setFlag( QGraphicsItem::ItemIsSelectable, false );
        setFlag( QGraphicsItem::ItemIsMovable, false );
        changeColor( Qt::black );
    }
}
