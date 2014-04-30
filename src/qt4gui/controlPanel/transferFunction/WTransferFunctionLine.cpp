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

#include "WTransferFunctionLine.h"
#include "WTransferFunctionPoint.h"
#include "WTransferFunctionWidget.h"

#include "QtGui/QPainter"
#include "QtGui/QStyleOption"
#include "QtGui/QGraphicsSceneMouseEvent"

WTransferFunctionLine::WTransferFunctionLine( WTransferFunctionWidget* /* parent */ )
    : BaseClass(), lineWidth( 2.0 ), left( 0x0 ), right( 0x0 )
{
    setPen( QPen( Qt::black, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
    //this->setFlag( ItemIsMovable ); //< this has to be implemented
    setZValue( 2 );
    setOpacity( 0.6 );
}

WTransferFunctionLine::~WTransferFunctionLine()
{
}

void WTransferFunctionLine::setLeft( WTransferFunctionPoint *point )
{
    left = point;
}

WTransferFunctionPoint *WTransferFunctionLine::getLeft() const
{
    return left;
}

void WTransferFunctionLine::setRight( WTransferFunctionPoint *point )
{
    right = point;
}

WTransferFunctionPoint* WTransferFunctionLine::getRight() const
{
    return right;
}

void WTransferFunctionLine::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget )
{
    if( left && right )
    {
        // update the line before painting according to our neighboring points
        QLineF line( left->pos(), right->pos() );
        setLine( line );
    }
    else
    {
        //std::cout << "line not initialized, yet" << std::endl;
    }
    // base class does the pointing for us.
    BaseClass::paint( painter, option, widget );
}

