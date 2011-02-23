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

#include <QtGui/QGraphicsRectItem>

#include "WQtNetworkEditorGlobals.h"
#include "WQtNetworkInputPort.h"

WQtNetworkInputPort::WQtNetworkInputPort( boost::shared_ptr<WModuleInputConnector> connector )
    : WQtNetworkPort()
{
    setRect( 0.0, 0.0, WNETWORKPORT_SIZEX, WNETWORKPORT_SIZEY );
    setBrush( Qt::gray );
    setPen( QPen( Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );

    setAcceptsHoverEvents( true );

    setPortName( connector.get()->getName().c_str() );
    setOutPort( connector.get()->isOutputConnector() );
    m_connector = connector;

    // create tooltip
    QString tmp;
    if( isOutPort() == true ) tmp = "output";
    else if( isOutPort() == false ) tmp = "input";
    else
        tmp = "undefined";

    QString str = "<b>Name: </b> " + getPortName() +
                  "<br/><b>PortType: </b>" + tmp;
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

QList< WQtNetworkArrow *> WQtNetworkInputPort::getArrowList()
{
    return m_arrows;
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

void WQtNetworkInputPort::removeArrows()
{
    foreach( WQtNetworkArrow *arrow, m_arrows )
    {
        int index = m_arrows.indexOf( arrow );
        if ( index != -1 )
        {
            m_arrows.removeAt( index );
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
