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
#include <cmath>

#include <algorithm>
#include <string>
#include <iostream>

#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsPathItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QPainterPath>

#include "core/kernel/combiner/WApplyCombiner.h"

#include "../WQt4Gui.h"
#include "../WMainWindow.h"

#include "WQtNetworkScene.h"
#include "WQtNetworkEditor.h"
#include "WQtNetworkInputPort.h"
#include "WQtNetworkOutputPort.h"

#include "WQtNetworkArrow.h"

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
    if( m_endPort )
    {
        m_endPort->removeArrow( this );
    }
}

int WQtNetworkArrow::type() const
{
    return Type;
}

WQtNetworkInputPort* WQtNetworkArrow::findNearestCompatibleInput( QPointF pos, float maxDistance )
{
    WQtNetworkScene* scene = WQt4Gui::getMainWindow()->getNetworkEditor()->getScene();

    // find items in area:
    QList<QGraphicsItem *> items = scene->items( pos.x() - ( maxDistance / 2.0 ),
                                                 pos.y() - ( maxDistance / 2.0 ),
                                                 maxDistance, maxDistance );

    // find all the connectors:
    WQtNetworkInputPort* nearest = NULL;
    float nearestDist = maxDistance;
    for( int i = 0; i < items.size(); ++i )
    {
        QGraphicsItem* item = items[ i ];

        // is this a connector?
        WQtNetworkInputPort* con = dynamic_cast< WQtNetworkInputPort* >( item );
        if( !con )
        {
            continue;
        }

        // is it compatible?
        if( !con->getConnector()->connectable( m_startPort->getConnector() ) )
        {
            continue;
        }

        // nearer than the previous one?
        QPointF conPos = mapFromItem( con, con->rect().bottomRight() * 0.5 );
        QPointF vec = pos - conPos;
        float dist = sqrt( ( vec.x() * vec.x() ) + ( vec.y() * vec.y() ) );
        // as we want euclidean dist:
        if( nearestDist >= dist )
        {
            nearestDist = dist;
            nearest = con;
        }
    }

    return nearest;
}

void WQtNetworkArrow::updatePosition( QPointF deviate )
{
    updatePosition( mapFromItem( m_endPort, m_startPort->rect().bottomRight() * 0.5 ), deviate );
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
    double dx = std::abs( m_line.dx() );
    double dy = std::abs( m_line.dy() );
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

void WQtNetworkArrow::startDrag( const QPointF& pos )
{
    m_snappedOff = false;
    m_connectionDisconnect = false;
    m_connectTo = NULL;

    // highlight
    changeColor( owGreen, 3 );

    // click Point
    m_clickPoint = pos;
}

void WQtNetworkArrow::moveDrag( const QPointF& pos )
{
    m_connectionDisconnect = false;
    m_connectTo = NULL;

    QPointF currentPoint = pos;

    // deviate according to start pos
    QPointF deviate = currentPoint - m_clickPoint;
    float l = sqrt( ( deviate.x() * deviate.x() ) + ( deviate.y() * deviate.y() ) );

    // if moved far enough, snap to mouse
    // NOTE: always be snapped of if there is no endport
    m_snappedOff = !m_endPort || m_snappedOff || ( l > 100.0 ); // when snapped of once, never snap on again

    if( m_snappedOff )
    {
        changeColor( owRed, 3 );

        // can we snap somewhere?
        WQtNetworkInputPort* nearestPort = findNearestCompatibleInput( currentPoint, 50.0 );
        if( nearestPort )
        {
            changeColor( owGreen, 3 );
            m_connectTo = nearestPort;
            updatePosition( mapFromItem( nearestPort, nearestPort->rect().bottomRight() * 0.5 ), QPointF() );
        }
        else
        {
            m_connectionDisconnect = true;
            updatePosition( currentPoint, QPointF() );
        }
    }
    else
    {
        // m_snappedOff is defined to ensure that we have m_endPoint if not snapped
        updatePosition( deviate );
    }
}

void WQtNetworkArrow::doneDrag( const QPointF& /*pos*/ )
{
    if( m_endPort && ( m_connectTo == m_endPort ) )
    {
        updatePosition();
        changeColor( Qt::black );

        m_connectTo = NULL;
        m_connectionDisconnect = false;
        return;
    }

    // apply operations
    if( m_connectTo )
    {
        // connect new
        boost::shared_ptr< WApplyCombiner > x( new WApplyCombiner( m_startPort->getConnector()->getModule(),
              m_startPort->getConnector()->getName(), m_connectTo->getConnector()->getModule(), m_connectTo->getConnector()->getName() ) );

        // remove old connection if needed
        if( m_endPort )
        {
            m_startPort->getConnector()->disconnect( m_endPort->getConnector() );
        }

        // apply combiner
        x->run();
        return;
    }

    // disconnect?
    if( m_connectionDisconnect && m_endPort )
    {
        m_startPort->getConnector()->disconnect( m_endPort->getConnector() );
        m_connectionDisconnect = false;
        return;
    }

    m_connectionDisconnect = false;
    m_connectTo = NULL;
}

void WQtNetworkArrow::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }
    mouseEvent->accept();

    startDrag( mouseEvent->pos() );
}

void WQtNetworkArrow::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    mouseEvent->accept();
    moveDrag( mouseEvent->pos() );
}

void WQtNetworkArrow::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( mouseEvent->button() != Qt::LeftButton )
    {
        mouseEvent->ignore();
        return;
    }
    mouseEvent->accept();

    doneDrag( mouseEvent->pos() );
}

