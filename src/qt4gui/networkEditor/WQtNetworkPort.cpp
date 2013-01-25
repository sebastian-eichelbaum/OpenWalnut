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

#include <QtGui/QGraphicsSceneMouseEvent>

#include "WQtNetworkPort.h"
#include "WQtNetworkOutputPort.h"
#include "WQtNetworkInputPort.h"
#include "WQtNetworkColors.h"

#include "core/kernel/combiner/WApplyCombiner.h"

WQtNetworkPort::WQtNetworkPort()
{
    setRect( 0.0, 0.0, WNETWORKPORT_SIZEX, WNETWORKPORT_SIZEY );
    setPen( QPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    setBrush( WQtNetworkColors::OutputConnector );
    m_brushNotSet = true;

    setAcceptsHoverEvents( true );
    m_line = NULL;
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
            m_line = new QGraphicsLineItem( QLineF( mouseEvent->scenePos(),
                    mouseEvent->scenePos() ) );
            m_line->setPen( QPen( Qt::black, 2 ) );
            scene()->addItem( m_line );
        }
    }
    else
    {
        mouseEvent->ignore();
    }
}

void WQtNetworkPort::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if( m_line )
    {
        QLineF newLine( m_line->line().p1(), mouseEvent->scenePos() );

        QList<QGraphicsItem *> endItem = scene()->items( mouseEvent->scenePos() );
        // because m_line is first item below the curser
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

                QList<QGraphicsItem *> startItems = scene()->items( m_line->line().p1() );
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
                   m_line->setPen( QPen( Qt::green, 2 ) );
                }
                else
                {
                   m_line->setPen( QPen( Qt::red, 2 ) );
                }
            }
            else if( endItem.first()->type() == WQtNetworkOutputPort::Type )
            {
                   m_line->setPen( QPen( Qt::red, 2 ) );
            }
            else
            {
                m_line->setPen( QPen( Qt::black, 2 ) );
            }
        }
        else
        {
            m_line->setPen( QPen( Qt::black, 2 ) );
        }
        m_line->setLine( newLine );
    }
}

void WQtNetworkPort::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    Q_UNUSED( mouseEvent );

    if( m_line != 0 )
    {
        QList<QGraphicsItem *> startItems = scene()->items( m_line->line().p1() );
        QList<QGraphicsItem *> endItems = scene()->items( m_line->line().p2() );

        // because m_line is first item below the curser
        if( startItems.first()->type() == QGraphicsLineItem::Type )
        {
            startItems.removeFirst();
        }

        // because m_line is first item below the curser
        if( endItems.first()->type() == QGraphicsLineItem::Type )
        {
            endItems.removeFirst();
        }

        // remove current m_line for real connection
        scene()->removeItem( m_line );
        delete m_line;

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
        setPos( rect.width() / ( size + 1 ) * portNumber - 5.0, 0.0 );
    }
    else if( outPort == true )
    {
        setPos( rect.width() / ( size + 1 ) * portNumber - 5.0, rect.height() - this->rect().height() / 2 - 2 );
    }
}

float WQtNetworkPort::getMultiplePortWidth( size_t nbPorts )
{
    return ( 5 + WNETWORKPORT_SIZEX ) * nbPorts;
}

void WQtNetworkPort::removeArrow( WQtNetworkArrow *arrow )
{
    int index = m_arrows.indexOf( arrow );

    if(index != -1)
        m_arrows.removeAt( index );
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

