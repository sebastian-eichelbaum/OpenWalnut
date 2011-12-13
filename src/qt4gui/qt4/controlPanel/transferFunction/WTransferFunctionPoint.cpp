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

#include "WTransferFunctionWidget.h"

#include "WTransferFunctionPoint.h"

WTransferFunctionPoint::WTransferFunctionPoint( WTransferFunctionWidget *parent )
    : BaseClass(), radius( 6.0 ), left( 0x0 ), right( 0x0 ), line( 0x0 ), _parent( parent )
{
    this->setFlag( ItemIsMovable );
    this->setFlag( ItemSendsGeometryChanges );
    //this->setFlag( ItemIsSelectable ); //< we should be able to use this framework, but we do not make use of it right now

    setZValue( 4 );
}

WTransferFunctionPoint::~WTransferFunctionPoint()
{
}

void WTransferFunctionPoint::setLine( WTransferFunctionLine * line )
{
    this->line = line;
}

WTransferFunctionLine *WTransferFunctionPoint::getLine() const
{
    return line;
}


void WTransferFunctionPoint::setLeft( WTransferFunctionPoint *point )
{
    left = point;
}

WTransferFunctionPoint *WTransferFunctionPoint::getLeft() const
{
    return left;
}

void WTransferFunctionPoint::setRight( WTransferFunctionPoint *point )
{
    right = point;
}

WTransferFunctionPoint* WTransferFunctionPoint::getRight() const
{
    return right;
}

QRectF WTransferFunctionPoint::boundingRect() const
{
    const double padding( 2 );
    const double diameter( radius * 2 );
    return QRectF( -radius - padding, -radius - padding, diameter + padding, diameter + padding );
}

void WTransferFunctionPoint::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* )
{
    QRadialGradient gradient( 0.0, 0.0, radius );

    if( option->state & QStyle::State_Sunken )
    {
        gradient.setColorAt( 0, QColor( Qt::yellow ).light( 120 ) );
        gradient.setColorAt( 1, QColor( Qt::darkYellow ).light( 120 ) );
        painter->setPen( QPen( Qt::red, 0 ) );
    }
    else
    {
        gradient.setColorAt( 0, Qt::yellow );
        gradient.setColorAt( 1, Qt::darkYellow );
        painter->setPen( QPen( Qt::black, 0 ) );
    }

    painter->setBrush( gradient );

    const double diameter( radius*2 );
    painter->drawEllipse( -radius, -radius, diameter, diameter );
}

QVariant WTransferFunctionPoint::itemChange( GraphicsItemChange change, const QVariant &value )
{
    if( !scene() )
    {
        return value; //< we are still initializing things, no changes at this point
    }

    if( _parent )
    {
        _parent->dataChanged();
    }

    if( !left && !right )
    {
        return BaseClass::itemChange(  change, value );
    }
    if( ( change == ItemPositionHasChanged || change == ItemPositionChange ) && scene() )
    {
        QPointF newPos = value.toPointF();

        QRectF boundingBox( this->scene()->sceneRect() );

        if( !left )
        {
            newPos = ( QPointF( 0, newPos.y() ) );
        }

        if( !right )
        {
            newPos = ( QPointF( boundingBox.x() + boundingBox.width(), newPos.y() ) );
        }

        this->clampToRectangle( &newPos, boundingBox );
        this->clampToLeftAndRight( &newPos );

        // if ( _parent )
        //    _parent->dataChanged();

        this->setToolTip( QString( "isovalue=" ) + QString::number( newPos.x() ) + " alpha=" + QString::number( newPos.y() ) );
        return newPos;
    }
    return BaseClass::itemChange( change, value );
}

void WTransferFunctionPoint::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    //this->update();
    BaseClass::mousePressEvent( event );
    if( _parent )
        _parent->setCurrent( this );
}

void WTransferFunctionPoint::clampToLeftAndRight( QPointF* const pos ) const
{
    Q_ASSERT( pos );
    if( left )
    {
        if( pos->x() <= left->pos().x() )
        {
            ( *pos ) = QPointF( left->pos().x() + 1, pos->y() );
        }
    }
    if( right )
    {
        if( pos->x() >= right->pos().x() )
        {
            ( *pos ) = QPointF( right->pos().x() - 1, pos->y() );
        }
    }
}

void WTransferFunctionPoint::clampToRectangle( QPointF* const pos, const QRectF& rectangle ) const
{
    Q_ASSERT( pos );
    qreal x( pos->x() );
    qreal y( pos->y() );

    const qreal xMin( rectangle.x() );
    const qreal xMax( xMin + rectangle.width() );
    const qreal yMin( rectangle.y() );
    const qreal yMax( yMin + rectangle.height() );

    x = qMax( x, xMin );
    x = qMin( x, xMax );

    y = qMax( y, yMin );
    y = qMin( y, yMax );

    ( *pos ) = QPointF( x, y );
}

void WTransferFunctionPoint::setPos( QPointF point )
{
    BaseClass::setPos( point );
}

