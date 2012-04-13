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

void WQtNetworkArrow::updatePosition()
{
    QRectF sRect = m_startPort->rect();
    QRectF eRect = m_startPort->rect();
    QLineF tmpLine( mapFromItem( m_startPort, sRect.bottomRight() * 0.5 ),
                    mapFromItem( m_endPort, eRect.bottomRight() * 0.5 ) );
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
    path.cubicTo( c1, c2, m_line.p2() );
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
    if( isSelected() &&
        m_color != Qt::green )
    {
        changeColor( Qt::green );
    }

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

    changeColor( Qt::green );
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

