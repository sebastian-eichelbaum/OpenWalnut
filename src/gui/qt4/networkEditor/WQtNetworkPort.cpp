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
#include <boost/shared_ptr.hpp>

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>

#include "WQtNetworkPort.h"
#include "WQtNetworkOutputPort.h"
#include "WQtNetworkInputPort.h"
#include "../../../kernel/combiner/WApplyCombiner.h"

WQtNetworkPort::WQtNetworkPort()
{
}

WQtNetworkPort::~WQtNetworkPort()
{
}

void WQtNetworkPort::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QList<QGraphicsItem *> startItem = scene()->items( mouseEvent->scenePos() );
    if( !startItem.isEmpty() )
    {
        mouseEvent->accept();
        if( startItem.first()->type() == WQtNetworkOutputPort::Type &&
            startItem.first()->parentItem()->isEnabled() == true )
        {
            line = new QGraphicsLineItem( QLineF( mouseEvent->scenePos(),
                    mouseEvent->scenePos() ) );
            line->setPen( QPen( Qt::black, 2 ) );
            scene()->addItem( line );
        }
    }
    else
    {
        mouseEvent->ignore();
    }
}

void WQtNetworkPort::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( line != 0 )
    {
        QLineF newLine( line->line().p1(), mouseEvent->scenePos() );

        QList<QGraphicsItem *> endItem = scene()->items( mouseEvent->scenePos() );
        // because line is first item below the curser
        if( !endItem.isEmpty() &&
                endItem.first()->type() == QGraphicsLineItem::Type )
        {
            endItem.removeFirst();
        }

        if( !endItem.isEmpty() )
        {
            if( endItem.first()->type() == WQtNetworkInputPort::Type )
            {
                WQtNetworkInputPort *endPort = qgraphicsitem_cast<WQtNetworkInputPort *>( endItem.first() );

                QList<QGraphicsItem *> startItems = scene()->items( line->line().p1() );
                if( startItems.first()->type() == QGraphicsLineItem::Type )
                {
                    startItems.removeFirst();
                }
                WQtNetworkOutputPort *startPort = qgraphicsitem_cast<WQtNetworkOutputPort *>( startItems.first() );

                if( endPort->parentItem() != this->parentItem() &&
                    endPort->parentItem()->isEnabled() == true &&
                    endPort->getConnector()->connectable( startPort->getConnector() ) == true
                  )
                {
                   line->setPen( QPen( Qt::green, 2 ) );
                }
                else
                {
                   line->setPen( QPen( Qt::red, 2 ) );
                }
            }
            else if( endItem.first()->type() == WQtNetworkOutputPort::Type )
            {
                   line->setPen( QPen( Qt::red, 2 ) );
            }
            else
            {
                line->setPen( QPen( Qt::black, 2 ) );
            }
        }
        else
        {
            line->setPen( QPen( Qt::black, 2 ) );
        }
        line->setLine( newLine );
    }
}

void WQtNetworkPort::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    if( line != 0 )
    {
        QList<QGraphicsItem *> startItems = scene()->items( line->line().p1() );
        QList<QGraphicsItem *> endItems = scene()->items( line->line().p2() );

        // because line is first item below the curser
        if( startItems.first()->type() == QGraphicsLineItem::Type )
        {
            startItems.removeFirst();
        }

        // because line is first item below the curser
        if( endItems.first()->type() == QGraphicsLineItem::Type )
        {
            endItems.removeFirst();
        }

        // remove current line for real connection
        scene()->removeItem( line );

        if( !endItems.isEmpty() &&
             !startItems.isEmpty() &&
             endItems.first()->type() == WQtNetworkInputPort::Type &&
             endItems.first()->parentItem()->isEnabled() == true &&
             startItems.first()->parentItem() != endItems.first()->parentItem() )
        {
            WQtNetworkOutputPort *startPort = dynamic_cast<WQtNetworkOutputPort *>( startItems.first() );
            WQtNetworkInputPort *endPort = dynamic_cast<WQtNetworkInputPort *>( endItems.first() );

            if( endPort->getConnector()->connectable( startPort->getConnector() ) == true )
            {
                  boost::shared_ptr< WApplyCombiner > x = boost::shared_ptr< WApplyCombiner >( new WApplyCombiner(
                                                            startPort->getConnector()->getModule(),
                                                            startPort->getConnector()->getName(),
                                                            endPort->getConnector()->getModule(),
                                                            endPort->getConnector()->getName() ) );
                  x->run();
            }
        }
    }
}

void WQtNetworkPort::alignPosition( int size, int portNumber, QRectF rect, bool outPort )
{
    if( outPort == false )
    {
        setPos( rect.width() / ( size+1 ) * portNumber - 5.0, 0.0 );
    }
    else if( outPort == true )
    {
        setPos( rect.width() / ( size+1 ) * portNumber - 5.0, rect.height() - 5 );
    }
}
