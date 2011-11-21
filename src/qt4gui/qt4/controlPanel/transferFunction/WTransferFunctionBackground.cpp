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

#include <iostream>

#include "QtGui/QPainter"
#include "QtGui/QStyleOption"
#include "QtGui/QGraphicsSceneMouseEvent"

#include "WTransferFunctionBackground.h"
#include "WTransferFunctionWidget.h"

WTransferFunctionBackground::WTransferFunctionBackground( WTransferFunctionWidget *parent ) : BaseClass()
{
    //this->setFlag( ItemIsMovable );
    setOpacity( 1.0 );
    setZValue( -1 );

    setPos( QPointF( 0, 0 ) );
}

void WTransferFunctionBackground::setMyPixmap( const QPixmap& newpixmap )
{
    if ( newpixmap.width() > 0 && newpixmap.height() > 0 )
    {
        // std::cout << "SET MY PIXMAP" << std::endl;
        QTransform m( scene()->sceneRect().width()/( double )newpixmap.width(), 0, 0,
                      0, scene()->sceneRect().height()/( double )newpixmap.height(), 0,
                      0, 0, 1 );
        setTransform( m );
    }
    BaseClass::setPixmap( newpixmap );
}

WTransferFunctionBackground::~WTransferFunctionBackground()
{
}

QRectF WTransferFunctionBackground::boundingRect() const
{
    return scene()->sceneRect();
}

