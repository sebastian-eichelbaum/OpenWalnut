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

#include "WQtNetworkItemActivator.h"
#include "WQtNetworkOutputPort.h"
#include "WQtNetworkInputPort.h"
#include "WQtNetworkColors.h"

#include "core/kernel/combiner/WApplyCombiner.h"

WQtNetworkItemActivator::WQtNetworkItemActivator( boost::shared_ptr< WModule > module )
    : m_module( module ), m_activeColor( Qt::darkYellow ), m_inactiveColor( Qt::gray )
{
    setRect( 0.0, 0.0, WNETWORKPORT_SIZEX, WNETWORKPORT_SIZEY );
    setPen( QPen( Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    setAcceptsHoverEvents( true );
}

WQtNetworkItemActivator::~WQtNetworkItemActivator()
{
}

int WQtNetworkItemActivator::type() const
{
    return Type;
}

void WQtNetworkItemActivator::paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget )
{
    if( m_module->getProperties()->getProperty( "active" )->toPropBool()->get() )
    {
        setBrush( QBrush( m_activeColor ) );
    }
    else
    {
        setBrush( QBrush( m_inactiveColor ) );
    }
    QGraphicsEllipseItem::paint( painter, option, widget );
}

void WQtNetworkItemActivator::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QList<QGraphicsItem *> startItem = scene()->items( mouseEvent->scenePos() );
    if( !startItem.isEmpty() )
    {
        if( m_module->getProperties()->getProperty( "active" )->toPropBool()->get() )
        {
            m_module->getProperties()->getProperty( "active" )->toPropBool()->set( false );
        }
        else
        {
            m_module->getProperties()->getProperty( "active" )->toPropBool()->set( true );
        }

        mouseEvent->accept();
    }
    else
    {
        mouseEvent->ignore();
    }
}
