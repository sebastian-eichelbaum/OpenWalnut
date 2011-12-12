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

#include "QtCore/QMimeData"
#include "QtGui/QColorDialog"
#include "QtGui/QGraphicsSceneMouseEvent"
#include "QtGui/QPainter"
#include "QtGui/QStyleOption"

#include "WTransferFunctionWidget.h"

#include "WTransferFunctionColorPoint.h"

#include "WTransferFunctionColorPoint.moc"

WTransferFunctionColorPoint::WTransferFunctionColorPoint( WTransferFunctionWidget *parent )
    : BaseClass(),
    radius( 8.0 ), left( 0x0 ), right( 0x0 ),
    _parent( parent ),
    color( QColor( 0, 0, 1, 255 ) )
{
    this->setFlag( ItemIsMovable );
    // position is always on y axis
    this->setPos( this->pos().x(), 0 );

    //this->setFlag( QGraphicsItem::ItemIsSelectable ); //< do we want selections?
    this->setFlag( QGraphicsItem::ItemIsMovable );
    this->setFlag( QGraphicsItem::ItemSendsGeometryChanges );
    this->setAcceptDrops( true );

    //this->setCursor( Qt::SplitHCursor );

    setZValue( 3 );
}

WTransferFunctionColorPoint::~WTransferFunctionColorPoint()
{
}

void WTransferFunctionColorPoint::setLeft( WTransferFunctionColorPoint *point )
{
    left = point;
}

WTransferFunctionColorPoint *WTransferFunctionColorPoint::getLeft() const
{
    return left;
}

void WTransferFunctionColorPoint::setRight( WTransferFunctionColorPoint *point )
{
    right = point;
}

WTransferFunctionColorPoint* WTransferFunctionColorPoint::getRight() const
{
    return right;
}

QRectF WTransferFunctionColorPoint::boundingRect() const
{
    const double padding( 2 );
    const double diameter( radius * 2 );
    return QRectF( -radius - padding, -radius - padding, diameter + padding, radius + padding );
}

void WTransferFunctionColorPoint::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* )
{
    QRadialGradient gradient( 0.0, 0.0, radius );

    if( option->state & QStyle::State_Selected )
    {
        gradient.setColorAt( 0, Qt::yellow );
        gradient.setColorAt( 1, Qt::red );
        painter->setPen( QPen( Qt::black, 2 ) );
    }
    else if ( option->state & QStyle::State_Sunken )
    {
        gradient.setColorAt( 0, color ); //QColor( Qt::yellow ).light( 120 ) );
        gradient.setColorAt( 1, color ); //QColor( Qt::darkYellow ).light( 120 ) );
        painter->setPen( QPen( Qt::black, 2 ) );
    }
    else
    {
        gradient.setColorAt( 0, color ); //Qt::yellow );
        gradient.setColorAt( 1, color ); //Qt::darkYellow );
        painter->setPen( QPen( Qt::black, 0 ) );
    }

    QColor color = painter->pen().color();
    color.setAlpha( 0.3 );
    painter->setPen( QPen( color, painter->pen().width() ) );
    painter->setBrush( gradient );

    QPolygon triangle;
    triangle.clear();
    triangle << QPoint( -radius, -radius );
    triangle << QPoint( 0, 0 );
    triangle << QPoint( radius, -radius );

    painter->drawPolygon( triangle );

    if( left && right )
    {
        if( option->state & QStyle::State_Sunken )
        {
            painter->setPen(  QPen( Qt::red, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin ) );
        }
        else
        {
            painter->setPen(  QPen( Qt::black, 1, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin ) );
        }
        // FIXME: fixed size in code
        painter->drawLine( QPoint( 0, 0 ), QPoint( 0, scene()->sceneRect().height() ) );
    }
}

QVariant WTransferFunctionColorPoint::itemChange( GraphicsItemChange change, const QVariant &value )
{
    // if ( change == ItemPositionChange )
    // {
    //  QPointF newPos = value.toPointF();
    //
    //  QRectF boundingBox( this->scene()->sceneRect() );
    //  if ( boundingBox.left() > newPos.x() || boundingBox.right() < newPos.x() )
    //  {
    //    newPos.setX( qMin( boundingBox.right(), qMax( newPos.x(), boundingBox.left() ) ) );
    //    if ( newPos.y() != 0 )
    //      newPos.setY( 0 );
    //    return newPos;
    //  }
    //  if ( newPos.y() != 0 )
    //  {
    //    newPos.setY( 0 );
    //
    //    return newPos;
    //  }
    // }

    // // This is the wrong place for such an update :-(
    // if ( _parent )
    // {
    //   _parent->forceRedraw();
    // }

    if( change == ItemPositionChange )
    {
        QPointF newPos = value.toPointF();
        this->setToolTip( QString( "isovalue=" ) + QString::number( newPos.x() ) + " color=("
                + QString::number( color.red() ) +','
                + QString::number( color.green() ) +','
                + QString::number( color.blue() ) + ')' );
        if( _parent )
            _parent->dataChanged();
    }

    return BaseClass::itemChange( change, value );
}

void WTransferFunctionColorPoint::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event )
{
    BaseClass::mouseDoubleClickEvent( event );

    showColorPicker();
}

void WTransferFunctionColorPoint::colorSelected( const QColor &newcolor )
{
    color = newcolor;
    //this->update();
    if( _parent )
    {
        _parent->dataChanged();
    }
}

void WTransferFunctionColorPoint::mouseMoveEvent( QGraphicsSceneMouseEvent *event )
{
    if( left && right )
    {
        //this->update();
        BaseClass::mouseMoveEvent( event );


#if 1
        QRectF boundingBox( this->scene()->sceneRect() );

        if( !left )
        {
            this->setPos( 0, this->pos().y() );
        }

        if( !right )
        {
            this->setPos( boundingBox.x() + boundingBox.width(), this->pos().y() );
        }
#endif

        this->clampToRectangle( boundingBox );
        this->clampToLeftAndRight();

        // color markers are always at zero
        this->setPos( this->pos().x(), 0 );
    }
}

void WTransferFunctionColorPoint::mousePressEvent( QGraphicsSceneMouseEvent *event )
{
    //this->update();
    BaseClass::mousePressEvent( event );
    _parent->setCurrentColor( this );
}

void WTransferFunctionColorPoint::mouseReleaseEvent( QGraphicsSceneMouseEvent *event )
{
    //this->update();
    BaseClass::mouseReleaseEvent( event );
}

void WTransferFunctionColorPoint::clampToLeftAndRight()
{
    if( left )
    {
        if( this->pos().x() <= left->pos().x() )
        {
            this->setPos( left->pos().x() + 1, this->pos().y() );
        }
    }
    if( right )
    {
        if( this->pos().x() >= right->pos().x() )
        {
            this->setPos( right->pos().x() - 1, this->pos().y() );
        }
    }
}

void WTransferFunctionColorPoint::clampToRectangle( const QRectF& rectangle )
{
    qreal x( this->pos().x() );
    qreal y( this->pos().y() );

    const qreal xMin( rectangle.x() );
    const qreal xMax( xMin + rectangle.width() );
    const qreal yMin( rectangle.y() );
    const qreal yMax( yMin + rectangle.height() );

    x = qMax( x, xMin );
    x = qMin( x, xMax );

    y = qMax( y, yMin );
    y = qMin( y, yMax );

    this->setPos( x, y );
}

QColor WTransferFunctionColorPoint::getColor() const
{
    return color;
}

void WTransferFunctionColorPoint::dragEnterEvent( QGraphicsSceneDragDropEvent* event )
{
    if( event->mimeData()->hasColor() )
    {
        event->setAccepted( true );
    }
}

void WTransferFunctionColorPoint::dropEvent( QGraphicsSceneDragDropEvent* event )
{
    if( event->mimeData()->hasColor() )
    {
        color = qvariant_cast<QColor>( event->mimeData()->colorData() );
        //this->update();
        if( _parent )
            _parent->dataChanged();
    }
}

void WTransferFunctionColorPoint::contextMenuEvent( QGraphicsSceneContextMenuEvent* event )
{
    // I get right clicks as context menu events on OSX which may be intended,
    // but I get both at the same time, so I create a new object and get its
    // context menu as well, which is confusing. Therefore, this is disabled
    // right now.
    // Additionally, I do not know how to remove the item from its own menu
#ifdef ALLOW_CONTEXT_MENU
    event->accept();
    QMenu menu;
    QAction *removeAction = menu.addAction( "Remove" );
    QAction *changeColor  = menu.addAction( "Change Color" );
    QAction *selectedAction = menu.exec( event->screenPos() );

    if( selectedAction == removeAction )
    {
    }
    if( selectedAction == changeColor )
    {
        showColorPicker();
    }
#endif
    BaseClass::contextMenuEvent( event );
}

void WTransferFunctionColorPoint::showColorPicker()
{
    QColorDialog *dialog = new QColorDialog( );
    dialog->setCurrentColor( color );
    dialog->setOption( QColorDialog::NoButtons );
    connect( dialog, SIGNAL( currentColorChanged( const QColor& ) ), this, SLOT( colorSelected( const QColor& ) ) );
    //dialog->open( this, SLOT( colorSelected( const QColor& ) ) );
    dialog->open();
}

