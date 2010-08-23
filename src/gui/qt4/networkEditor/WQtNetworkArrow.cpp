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
#include <math.h>

#include <QtGui/QGraphicsLineItem>

#include "WQtNetworkArrow.h"

const qreal Pi = 3.14;

WQtNetworkArrow::WQtNetworkArrow( WQtNetworkPort *startPort, WQtNetworkPort *endPort )
    : QGraphicsLineItem()
{
    m_startPort = startPort;
    m_endPort = endPort;

    setFlag( QGraphicsItem::ItemIsSelectable, true );
    setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    setAcceptsHoverEvents( true );
}

WQtNetworkArrow::~WQtNetworkArrow()
{
}

void WQtNetworkArrow::updatePosition()
{
    QRectF sRect = m_startPort->rect();
    QRectF eRect = m_startPort->rect();
    QLineF line( mapFromItem( m_startPort, sRect.bottomRight()/2 ),
                    mapFromItem( m_endPort, eRect.bottomRight()/2 ) );
    setLine( line );
}

WQtNetworkPort* WQtNetworkArrow::getStartPort()
{
    return m_startPort;
}

WQtNetworkPort* WQtNetworkArrow::getEndPort()
{
    return m_endPort;
}

void WQtNetworkArrow::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    if( isSelected() &&
            m_color != Qt::green )
    {
        m_color = Qt::green;
    }
    else if ( !isSelected() &&
            m_color != Qt::black )
    {
        m_color = Qt::black;
    }

setPen( QPen( m_color, 2, Qt::SolidLine ) );
painter->setBrush( m_color );

    QGraphicsLineItem::paint( painter, option, 0 );

    qreal arrowSize = 10;
    double angle = ::acos(line().dx() / line().length());
    if (line().dy() >= 0)
        angle = (Pi * 2) - angle;

    QPointF arrowP1 = line().p2() - QPointF(sin(angle + Pi / 3) * arrowSize,
            cos(angle + Pi / 3) * arrowSize);
    QPointF arrowP2 = line().p2() - QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
            cos(angle + Pi - Pi / 3) * arrowSize);

    arrowHead.clear();
    arrowHead << line().p2() << arrowP1 << arrowP2;

    painter->setPen( QPen( m_color, 1, Qt::SolidLine ) );
    painter->setBrush( m_color );

    painter->drawPolygon(arrowHead);
}

 QRectF WQtNetworkArrow::boundingRect() const
 {
    QRectF rect = shape().boundingRect();

    // add a few extra pixels for the arrow and the pen
    qreal penWidth = 1;
    qreal extra = (penWidth + 10) / 2.0;
    rect.adjust(-extra, -extra, extra, extra);

    return rect;
}

 QPainterPath WQtNetworkArrow::shape() const
 {
     QPainterPath path = QGraphicsLineItem::shape();
     path.addPolygon(arrowHead);
     return path;
 }
        
void WQtNetworkArrow::hoverEnterEvent ( QGraphicsSceneHoverEvent * event )
{
    event->accept();
}
