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

#include <math.h>

#include <algorithm>
#include <string>
#include <iostream>

#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QPainterPath>

#include "WQtNetworkInputPort.h"
#include "WQtNetworkOutputPort.h"

#include "WQtNetworkArrow.h"

const qreal Pi = 3.14;
const QColor owRed( 248, 87, 87 );
const QColor owGreen( 115, 225, 115 );

WQtNetworkArrow::WQtNetworkArrow( WQtNetworkOutputPort *startPort, WQtNetworkInputPort *endPort )
    : QGraphicsPathItem()
{
    m_startPort = startPort;
    m_endPort = endPort;

    setFlag( QGraphicsItem::ItemIsSelectable, true );
    setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    setAcceptsHoverEvents( true );
}

WQtNetworkArrow::~WQtNetworkArrow()
{
    m_startPort->removeArrow( this );
    m_endPort->removeArrow( this );
}

int WQtNetworkArrow::type() const
{
    return Type;
}

void WQtNetworkArrow::updatePosition( QPointF deviate )
{
    QRectF eRect = m_startPort->rect();
    updatePosition( mapFromItem( m_endPort, eRect.bottomRight() * 0.5 ), deviate );
}

void WQtNetworkArrow::updatePosition( QPointF targetPoint, QPointF deviate )
{
    QRectF sRect = m_startPort->rect();
    QLineF tmpLine( mapFromItem( m_startPort, sRect.bottomRight() * 0.5 ),
                    targetPoint );

    m_line = QLineF( tmpLine.x1(), tmpLine.y1()+5, tmpLine.x2(), tmpLine.y2()-5 );

    QPainterPath path( m_line.p1() );

    // the control points
    // Change some of these values to modify the bezier effect
    double dx = abs( m_line.dx() );
    double dy = abs( m_line.dy() );
    double minCDist = 50.0;
    double maxCDist = 250.0;
    // this magic code is the result of try and error
    dy = std::min( maxCDist, std::max( minCDist, std::max( dx * 0.5, dy ) * 0.5 ) );

    QPointF c1( m_line.p1() + QPointF( 0, +dy ) );
    QPointF c2( m_line.p2() + QPointF( 0, -dy ) );

    // cubic bezier
    path.cubicTo( c1, c2 + deviate, m_line.p2() );
    setPath( path );
}

WQtNetworkOutputPort* WQtNetworkArrow::getStartPort()
{
    return m_startPort;
}

WQtNetworkInputPort* WQtNetworkArrow::getEndPort()
{
    return m_endPort;
}

QVariant WQtNetworkArrow::itemChange( GraphicsItemChange change,
        const QVariant &value )
{
    if( change == QGraphicsItem::ItemSelectedHasChanged )
    {
        changeColor( Qt::black );
    }
    return value;
}

void WQtNetworkArrow::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w )
{
    QStyleOptionGraphicsItem *o = const_cast<QStyleOptionGraphicsItem*>( option );
    o->state &= ~QStyle::State_Selected;

    QGraphicsPathItem::paint( painter, o, w );

    qreal arrowSize = 10;
    QPointF arrowP1 = m_line.p2() + QPointF(  0.5 * arrowSize, -arrowSize );
    QPointF arrowP2 = m_line.p2() + QPointF( -0.5 * arrowSize, -arrowSize );
    m_arrowHead.clear();
    m_arrowHead << m_line.p2() << arrowP1 << arrowP2;

    painter->setPen( QPen( m_color, 1, Qt::SolidLine ) );
    painter->setBrush( m_color );
    painter->drawPolygon( m_arrowHead );
}

QRectF WQtNetworkArrow::boundingRect() const
{
    QRectF rect = shape().boundingRect();

    // add a few extra pixels for the arrow and the pen
    qreal penWidth = 1;
    qreal extra = ( penWidth + 10 ) / 2.0;
    rect.adjust( -extra, -extra, extra, extra );

    return rect;
}

QPainterPath WQtNetworkArrow::shape() const
{
     QPainterPath path = QGraphicsPathItem::shape();
     path.addPolygon( m_arrowHead );
     return path;
}

void WQtNetworkArrow::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
    Q_UNUSED( event );

    changeColor( Qt::black, 3 );
    updatePosition();
}

void WQtNetworkArrow::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
    Q_UNUSED( event );

    changeColor( Qt::black );
}

void WQtNetworkArrow::changeColor( QColor color )
{
    m_color = color;
    setPen( QPen( m_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
}

void WQtNetworkArrow::changeColor( QColor color, float penWidth )
{
    m_color = color;
    setPen( QPen( m_color, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
}

void WQtNetworkArrow::mouseDoubleClickEvent( QGraphicsSceneMouseEvent* mouseEvent )
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
        getStartPort()->getConnector()->disconnect( getEndPort()->getConnector() );
    }
    else
    {
        mouseEvent->ignore();
    }
}

void WQtNetworkArrow::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }
    mouseEvent->accept();

    m_snappedOff = false;
    m_connectionDisconnect = false;

    // highlight
    changeColor( owGreen, 3 );

    // clickPoint
    m_clickPoint = mouseEvent->pos();
}

void WQtNetworkArrow::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    mouseEvent->accept();

    QPointF currentPoint = mouseEvent->pos();

    // deviate according to start pos
    QPointF deviate = currentPoint - m_clickPoint;

    float l = sqrt( ( deviate.x() * deviate.x() ) + ( deviate.y() * deviate.y() ) );

    // if moved far enough, snap to mouse
    m_snappedOff = m_snappedOff || ( l > 100.0 ); // when snapped of once, never snap on again

    // are we near the original connector?

    QPointF diffToConnector = currentPoint - mapFromItem( m_endPort, m_endPort->rect().bottomRight() * 0.5 );
    float lDiffToConnector = sqrt( ( diffToConnector.x() * diffToConnector.x() ) + ( diffToConnector.y() * diffToConnector.y() ) );
    bool snapBack = ( lDiffToConnector < 50.0 );

    if( m_snappedOff && !snapBack )
    {
        updatePosition( currentPoint, QPointF() );
    }
    else
    {
        updatePosition( deviate );
    }

    if( snapBack )
    {
        changeColor( owGreen, 3 );
        m_connectionDisconnect = false;
    }
    if( m_snappedOff && !snapBack )
    {
        changeColor( owRed, 3 );
        m_connectionDisconnect = true;
    }
}

void WQtNetworkArrow::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }
    mouseEvent->accept();

    updatePosition();
    changeColor( Qt::black );

    // what to do
    if( m_connectionDisconnect )
    {
        m_startPort->getConnector()->disconnect( m_endPort->getConnector() );
    }
}

