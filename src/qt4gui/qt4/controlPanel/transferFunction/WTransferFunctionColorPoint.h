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

#ifndef WTRANSFERFUNCTIONCOLORPOINT_H
#define WTRANSFERFUNCTIONCOLORPOINT_H

#include "QtGui/QGraphicsItem"

#include "QtGui/QMenu"

class WTransferFunctionWidget;

/**
 * A control point for the color function.
 *
 * Internals:
 * The point has links to its left and right neighbors (double-linked list).
 */
class WTransferFunctionColorPoint : public QObject, public QGraphicsItem
{
   Q_OBJECT // NOLINT
   Q_INTERFACES( QGraphicsItem ) // NOLINT

  public:
    typedef QGraphicsItem BaseClass;

    explicit WTransferFunctionColorPoint( WTransferFunctionWidget* parent = 0x0 );
    virtual ~WTransferFunctionColorPoint();

    WTransferFunctionColorPoint *getLeft() const;
    WTransferFunctionColorPoint *getRight() const;
    void setLeft( WTransferFunctionColorPoint* );
    void setRight( WTransferFunctionColorPoint* );

    QRectF boundingRect() const;
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget = 0x0 );

    QColor getColor() const;

    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent* event );
    virtual void dropEvent( QGraphicsSceneDragDropEvent* event );

    void contextMenuEvent( QGraphicsSceneContextMenuEvent* event );

  public slots:
    /**
      * Called by the color dialog every time the user changes the color.
      */
    void colorSelected( const QColor& );

  protected:
    void clampToRectangle( const QRectF& rectangle );
    void clampToLeftAndRight( );

    QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
    void mousePressEvent( QGraphicsSceneMouseEvent *event );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );

    void showColorPicker();
  private:
    double radius;
    WTransferFunctionColorPoint *left;
    WTransferFunctionColorPoint *right;

    WTransferFunctionWidget* _parent;

    QColor color;
};

#endif  // WTRANSFERFUNCTIONCOLORPOINT_H

