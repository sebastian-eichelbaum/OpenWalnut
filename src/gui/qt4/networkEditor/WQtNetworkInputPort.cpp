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
#include "WQtNetworkOutputPort.h"

#include "WQtNetworkInputPort.h"

const float SIZE = 10.0;

WQtNetworkInputPort::WQtNetworkInputPort( boost::shared_ptr<WModuleInputConnector> connector )
    : WQtNetworkPort()
{
    setRect( 0.0, 0.0, SIZE, SIZE );
    setBrush( Qt::gray );
    setPen( QPen( Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    setAcceptsHoverEvents( true );

    setPortName( connector.get()->getName().c_str() );
    setOutPort( connector.get()->isOutputConnector() );
    m_connector = connector;

    QString tmp;
    if( isOutPort() == true ) tmp = "output";
    else if( isOutPort() == false ) tmp = "input";
    else
        tmp = "undefined";

    //TODO
    QString str = "Name: " + getPortName() + "\nPortType: " + tmp;
    if( toolTip() != str )
    {
        setToolTip( str );
    }
}

WQtNetworkInputPort::~WQtNetworkInputPort()
{
    //removeArrows();
}

int WQtNetworkInputPort::type() const
{
    return Type;
}

void WQtNetworkInputPort::addArrow( WQtNetworkArrow *arrow )
{
    m_arrows.append( arrow );
}

void WQtNetworkInputPort::removeArrow( WQtNetworkArrow *arrow )
{
    int index = m_arrows.indexOf( arrow );

    if (index != -1)
        m_arrows.removeAt( index );
}

QString WQtNetworkInputPort::getPortName()
{
    return m_name;
}

void WQtNetworkInputPort::setPortName( QString str )
{
    m_name = str;
}

int WQtNetworkInputPort::getNumberOfArrows()
{
    return m_arrows.size();
}

boost::shared_ptr<WModuleInputConnector> WQtNetworkInputPort::getConnector()
{
    return m_connector;
}


//private
void WQtNetworkInputPort::removeArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        int index = m_arrows.indexOf( arrow );
        if ( index != -1 )
        {
            m_arrows.removeAt( index );
            //delete arrow;
        }
    }
}

void WQtNetworkInputPort::updateArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        arrow->updatePosition();
    }
}

void WQtNetworkInputPort::setOutPort( bool type )
{
    m_isOutPort = type;
}

bool WQtNetworkInputPort::isOutPort()
{
    return m_isOutPort;
}
/*
void WQtNetworkInputPort::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
        line = new QGraphicsLineItem( QLineF( mouseEvent->scenePos(), mouseEvent->scenePos() ) );
        line->setPen( QPen( Qt::black, 2 ) );
        scene()->addItem( line );
}

void WQtNetworkInputPort::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
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
                //TODO!!!
                WQtNetworkInputPort *endPort = qgraphicsitem_cast<WQtNetworkInputPort *>( endItem.first() );

                if( //endPort->isOutPort() == false &&
                    endPort->parentItem() != this->parentItem() &&
                    //endPort->getPortName() == this->getPortName() &&
                    endPort->getNumberOfArrows() < 1
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

void WQtNetworkInputPort::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
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
             endItems.first()->type() == WQtNetworkInputPort::Type &&
             startItems.first()->parentItem() != endItems.first()->parentItem() )
        {
            WQtNetworkOutputPort *startPort = qgraphicsitem_cast<WQtNetworkOutputPort *>( startItems.first() );
            WQtNetworkInputPort *endPort = qgraphicsitem_cast<WQtNetworkInputPort *>( endItems.first() );

            std::cout << "PORTS CONNECTBAR: " << endPort->getConnector()->connectable( startPort->getConnector() ) << std::endl;

            //TODO skiunke: isOutput realy needed?
            if( endPort->getNumberOfArrows() < 1 &&
                    endPort->isOutPort() == false &&
                    startPort->isOutPort() == true &&
                    //endPort->getPortName() == startPort->getPortName() )
                    endPort->getConnector()->connectable( startPort->getConnector() ) == true )
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
*/
