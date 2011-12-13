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

#include <algorithm>
#include <iostream>
#include <cmath>

#include "WTransferFunctionHistogram.h"
#include "WTransferFunctionWidget.h"

#include "QtGui/QPainter"
#include "QtGui/QStyleOption"
#include "QtGui/QGraphicsSceneMouseEvent"


WTransferFunctionHistogram::WTransferFunctionHistogram( WTransferFunctionWidget * /*parent*/ ) : BaseClass()
{
    setOpacity( 0.3 );
    setZValue( 2 );
}

WTransferFunctionHistogram::~WTransferFunctionHistogram()
{
}

QRectF WTransferFunctionHistogram::boundingRect() const
{
    return scene()->sceneRect();
}

void WTransferFunctionHistogram::paint( QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget* )
{
    const int steps = data.size();
    if( steps > 0 )
    {
        double maxval = *std::max_element( data.begin(), data.end() );
        if( maxval > 0.0 )
        {
            QRadialGradient gradient( 0.0, 0.0, 10 );
            painter->setBrush( gradient );
            gradient.setColorAt( 0, Qt::white );
            gradient.setColorAt( 1, Qt::black );

            painter->setBrush( gradient );

            QPolygon histogram;
            QRectF bb( this->scene()->sceneRect() );
            histogram << QPoint( bb.right(), bb.bottom() );
            histogram << QPoint( bb.left(), bb.bottom() );

            for( int i = 0; i < steps; ++i )
            {
                // logarithmic mapping of histogram to values
                // the added 1.001 is to avoid numerical problems but should work for most data sets
                histogram << QPoint( bb.left()+ ( double )bb.width()*( double )i/( double )steps,
                          bb.bottom() - ( double )bb.height() * std::log( data[ i ]+1 )/std::log( maxval+1.001 ) );
            }
            painter->drawPolygon( histogram );
        }
    }
}
