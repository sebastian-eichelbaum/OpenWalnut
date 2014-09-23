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

#include <boost/shared_ptr.hpp>

#include <QGraphicsSceneMouseEvent>

#include "WQtNetworkPort.h"
#include "WQtNetworkOutputPort.h"
#include "WQtNetworkInputPort.h"
#include "WQtNetworkColors.h"
#include "WQtNetworkEditorGlobals.h"

#include "core/kernel/combiner/WApplyCombiner.h"

WQtNetworkPort::WQtNetworkPort():
    QGraphicsRectItem()
{
    setRect( 0.0, 0.0, WNETWORKPORT_SIZEX, WNETWORKPORT_SIZEY );
    setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    m_brushNotSet = true;

    setAcceptsHoverEvents( true );
    m_arrow = NULL;
}

WQtNetworkPort::~WQtNetworkPort()
{
    removeArrows();
}

void WQtNetworkPort::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    // as the command setBrush forces a re-paint, permanently calling setbrush causes the widget to permanently redraw itself.
    if( m_brushNotSet )
    {
        m_brushNotSet = false;
        if( isOutPort() )
        {
            setBrush( WQtNetworkColors::OutputConnector );
        }
        else
        {
            setBrush( WQtNetworkColors::InputConnector );
        }
    }

    QGraphicsRectItem::paint( painter, option, widget );

    // add some nice triangle?
    // QPolygonF triangle;
    // triangle.clear();
    // triangle << QPointF( 0, 0 );
    // triangle << QPointF( WNETWORKPORT_SIZEX, 0 );
    // triangle << QPointF( static_cast< float >( WNETWORKPORT_SIZEX ) / 2.0 , WNETWORKPORT_SIZEY / 2.0 );
    // painter->setBrush( QBrush( Qt::black ) );
    // painter->drawPolygon( triangle );
}

void WQtNetworkPort::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* mouseEvent )
{
    QGraphicsItem::mouseDoubleClickEvent( mouseEvent );

    // ignore all buttons but the left one
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }

    QList<QGraphicsItem *> startItem = scene()->items( mouseEvent->scenePos() );
    if( !startItem.isEmpty() )
    {
        mouseEvent->accept();
        WQtNetworkOutputPort* outP = dynamic_cast< WQtNetworkOutputPort* >( startItem.first() );
        WQtNetworkInputPort* inP = dynamic_cast< WQtNetworkInputPort* >( startItem.first() );
        // delete all connections
        if( inP )
        {
            inP->getConnector()->disconnectAll();
        }
        if( outP )
        {
            outP->getConnector()->disconnectAll();
        }
    }
    else
    {
        mouseEvent->ignore();
    }
}

void WQtNetworkPort::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }
    QList<QGraphicsItem *> startItem = scene()->items( mouseEvent->scenePos() );
    if( !startItem.isEmpty() )
    {
        mouseEvent->accept();
        if( startItem.first()->type() == WQtNetworkOutputPort::Type &&
            startItem.first()->parentItem()->isEnabled() == true )
        {
            // use might have started to drag
            m_arrow = new WQtNetworkArrow( qgraphicsitem_cast< WQtNetworkOutputPort* >( startItem.first() ), NULL );
            m_arrow->startDrag( mouseEvent->scenePos() );
            scene()->addItem( m_arrow );
        }
    }
    else
    {
        mouseEvent->ignore();
    }
}

void WQtNetworkPort::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( m_arrow )
    {
        m_arrow->moveDrag( mouseEvent->scenePos() );
    }
}

void WQtNetworkPort::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    if( m_arrow )
    {
        m_arrow->doneDrag( mouseEvent->scenePos() );
        scene()->removeItem( m_arrow );
        delete m_arrow;
    }
}

void WQtNetworkPort::alignPosition( int size, int portNumber, QRectF rect, bool outPort )
{
    if( outPort == false )
    {
        setPos( rect.width() / ( size + 1 ) * portNumber - 5.0, -this->rect().height() / 2 + 3 );
    }
    else if( outPort == true )
    {
        setPos( rect.width() / ( size + 1 ) * portNumber - 5.0, rect.height() - this->rect().height() / 2 - 3 );
    }
}

float WQtNetworkPort::getMultiplePortWidth( size_t nbPorts )
{
    return ( 5 + WNETWORKPORT_SIZEX ) * nbPorts;
}

void WQtNetworkPort::removeArrow( WQtNetworkArrow *arrow )
{
    int index = m_arrows.indexOf( arrow );

    if( index != -1 )
    {
        m_arrows.removeAt( index );
    }
}

void WQtNetworkPort::removeArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        int index = m_arrows.indexOf( arrow );
        if( index != -1 )
        {
            m_arrows.removeAt( index );
        }
        delete arrow;
    }
}

QList< WQtNetworkArrow *> WQtNetworkPort::getArrowList()
{
    return m_arrows;
}

void WQtNetworkPort::addArrow( WQtNetworkArrow *arrow )
{
    m_arrows.append( arrow );
}

void WQtNetworkPort::updateArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        arrow->updatePosition();
    }
}

int WQtNetworkPort::getNumberOfArrows()
{
    return m_arrows.size();
}

QString WQtNetworkPort::getPortName()
{
    return m_name;
}

void WQtNetworkPort::setPortName( QString str )
{
    m_name = str;
}

void WQtNetworkPort::setOutPort( bool type )
{
    m_isOutPort = type;
}

bool WQtNetworkPort::isOutPort()
{
    return m_isOutPort;
}

