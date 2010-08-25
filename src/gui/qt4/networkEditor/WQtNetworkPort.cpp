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

WQtNetworkPort::WQtNetworkPort( QString name)
    : QGraphicsRectItem()
{
    setRect( 0.0, 0.0, 10.0, 10.0 );
    setBrush( Qt::gray );
    setPen( QPen( Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    
    setAcceptsHoverEvents(true);
 
    setPortName( name );

    QString str = "Name: " + getPortName() + "\nPortType:";
    if (toolTip() != str){
        setToolTip(str);
    }
}

WQtNetworkPort::~WQtNetworkPort()
{
    std::cout << "delete Port" << std::endl;
    removeArrows();
}
 
int WQtNetworkPort::type() const
{
    return Type;
}

void WQtNetworkPort::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( m_isOutPort == true )
    {
        line = new QGraphicsLineItem( QLineF( mouseEvent->scenePos(),
                    mouseEvent->scenePos() ) );
        line->setPen( QPen( Qt::black, 2 ) );
        scene()->addItem( line );
    }
    else if( m_isOutPort == false )
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
            if( endItem.first()->type() == WQtNetworkPort::Type )
            {
                WQtNetworkPort *endPort = qgraphicsitem_cast<WQtNetworkPort *>( endItem.first() );

                if( endPort->isOutPort() == false &&
                    endPort->parentItem() != this->parentItem() &&
                    endPort->getPortName() == this->getPortName() &&
                    endPort->m_arrows.size() < 1 )
                {
                   line->setPen( QPen( Qt::green, 2 ) );
                }
                else
                {
                   line->setPen( QPen( Qt::red, 2 ) );
                }
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
        delete line;

        if( !endItems.isEmpty() &&
             !startItems.isEmpty() &&
             endItems.first()->type() == WQtNetworkPort::Type &&
             startItems.first()->parentItem() != endItems.first()->parentItem() )
        {
            WQtNetworkPort *startPort = qgraphicsitem_cast<WQtNetworkPort *>( startItems.first() );
            WQtNetworkPort *endPort = qgraphicsitem_cast<WQtNetworkPort *>( endItems.first() );

            if( endPort->m_arrows.size() < 1 &&
                    endPort->isOutPort() == false &&
                    startPort->isOutPort() == true &&
                    endPort->getPortName() == startPort->getPortName() )
            {
                WQtNetworkArrow *arrow = new WQtNetworkArrow( startPort, endPort );
                
                arrow->setZValue( -1000.0 );

                startPort->addArrow( arrow );
                endPort->addArrow( arrow );

                scene()->addItem( arrow );
                arrow->updatePosition();
            }
        }
    }
}

void WQtNetworkPort::addArrow( WQtNetworkArrow *arrow )
{
    m_arrows.append( arrow );
}

void WQtNetworkPort::removeArrow( WQtNetworkArrow *arrow )
{
    std::cout << "removeArrow" << std::endl;
    int index = m_arrows.indexOf( arrow );

    if (index != -1)
        m_arrows.removeAt( index );
}

//private
void WQtNetworkPort::removeArrows()
{
    std::cout << "removeArrows" << std::endl;
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {        
        int index = m_arrows.indexOf( arrow );
        if (index != -1){
            m_arrows.removeAt( index );
            delete arrow;
        }
    }
}

void WQtNetworkPort::updateArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        arrow->updatePosition();
    }
}

void WQtNetworkPort::setOutPort( bool type )
{
    m_isOutPort = type;
}

bool WQtNetworkPort::isOutPort()
{
    return m_isOutPort;
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

QString WQtNetworkPort::getPortName()
{
    return m_name;
}

void WQtNetworkPort::setPortName( QString str )
{
    m_name = str;
}
