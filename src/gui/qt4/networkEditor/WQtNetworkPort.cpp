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
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>

#include "WQtNetworkPort.h"

WQtNetworkPort::WQtNetworkPort()
    : QGraphicsRectItem()
{
    setRect( 0.0, 0.0, 10.0, 10.0 );
    setBrush( Qt::green );
    setPen( QPen( Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
}

WQtNetworkPort::~WQtNetworkPort()
{
}

void WQtNetworkPort::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
        line = new QGraphicsLineItem( QLineF( mouseEvent->scenePos(),
                    mouseEvent->scenePos() ) );
        line->setPen( QPen( Qt::red, 2 ) );
        scene()->addItem( line );
}

void WQtNetworkPort::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( line != 0 )
    {
        QLineF newLine( line->line().p1(), mouseEvent->scenePos() );
        line->setLine( newLine );
    }
}

void WQtNetworkPort::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( line != 0 )
    {
        QList<QGraphicsItem *> startItems = scene()->items( line->line().p1() );
        QList<QGraphicsItem *> endItems = scene()->items( line->line().p2() );

        if( startItems.first()->type() == QGraphicsLineItem::Type )
        {
            startItems.removeFirst();
        }

        if( endItems.first()->type() == QGraphicsLineItem::Type )
        {
            endItems.removeFirst();
        }

        scene()->removeItem( line );
        delete line;

        std::cout << "count: " << endItems.count() << std::endl;

        if( !endItems.isEmpty() &&
             !startItems.isEmpty() &&
             endItems.first()->type() == WQtNetworkPort::Type &&
             startItems.first() != endItems.first() )
        {
            WQtNetworkPort *out = qgraphicsitem_cast<WQtNetworkPort *>( startItems.first() );
            WQtNetworkPort *in = qgraphicsitem_cast<WQtNetworkPort *>( endItems.first() );

             WQtNetworkArrow *arrow = new WQtNetworkArrow( out, in );
             arrow->setZValue( -1000.0 );

             out->addArrow( arrow );
             in->addArrow( arrow );

             scene()->addItem( arrow );
             arrow->updatePosition();

             std::cout << "zeichne" << std::endl;
        }
        else
        {
            std::cout << "zeichne nicht" << std::endl;
        }
    }
}

void WQtNetworkPort::addArrow( WQtNetworkArrow *arrow )
{
    m_arrows.append( arrow );
}

void WQtNetworkPort::removeArrow( WQtNetworkArrow *arrow )
{
    int index = m_arrows.indexOf( arrow );

    if (index != -1)
        m_arrows.removeAt( index );
}

void WQtNetworkPort::removeArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        arrow->startItem()->removeArrow( arrow );
        arrow->endItem()->removeArrow( arrow );
        scene()->removeItem( arrow );
        delete arrow;
    }
}

void WQtNetworkPort::update()
{
    std::cout << "arrow update " << std::endl;
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        arrow->updatePosition();
        std::cout << "arrow update " << std::endl;
    }
}
