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
    /** type of our base class */
    typedef QGraphicsItem BaseClass;

    /** default constructor
     * \param parent the parent widget
     */
    explicit WTransferFunctionColorPoint( WTransferFunctionWidget* parent = 0x0 );

    /**
     * default destructor
     */
    virtual ~WTransferFunctionColorPoint();

    /**
     * returns the item left of the this item
     * \returns the item left of us
     */
    WTransferFunctionColorPoint *getLeft() const;

    /**
     * returns the item right of this item
     * \returns the item right of us
     */
    WTransferFunctionColorPoint *getRight() const;

    /**
     * set the item left of us
     * \param left the item left of us
     */
    void setLeft( WTransferFunctionColorPoint* left);

    /**
     * set the item right of us
     * \param right the item right of us
     */
    void setRight( WTransferFunctionColorPoint* right);

    /**
     * the bounding rectangle used for paint and click events
     * \returns the bounding rectangle
     */
    QRectF boundingRect() const;

    /**
     * paint the handle and additional item hints
     * \param painter the painter used
     * \param option the paint options used
     * \param widget the widget used for painting
     */
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget = 0x0 );

    /** \returns the color of the entry */
    QColor getColor() const;

    /**
     * drag enter implementation for items that contain a color
     * \param event the event to handle
     */
    virtual void dragEnterEvent( QGraphicsSceneDragDropEvent* event );

    /**
     * drop implementation for items that contain a color
     * \param event the event to handle
     */
    virtual void dropEvent( QGraphicsSceneDragDropEvent* event );

    /**
     * handler for context menu envents. Currently disabled because it
     * conflicts with other mouse interactions
     * \param event the event to hanlde
     */
    void contextMenuEvent( QGraphicsSceneContextMenuEvent* event );

public slots:
    /**
      * Called by the color dialog every time the user changes the color.
      * \param color the new color
      */
    void colorSelected( const QColor& color);

protected:
    /** helper for item changed
     * \param rectangle a rectangel
     */
    void clampToRectangle( const QRectF& rectangle );

    /** helper for item changed */
    void clampToLeftAndRight( );

    /**
     * handles change notifications
     * \param change the requrested change
     * \param value the requested value
     * \returns the proposed value
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    /**
     * handles mouse move events
     * \param event the event
     */
    void mouseMoveEvent( QGraphicsSceneMouseEvent *event );

    /**
     * handles press move events
     * \param event the event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *event );

    /**
     * handles mouse move events
     * \param event the event
     */
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    /**
     * handles mouse double click events by opening a color selector
     * if the item is clicked.
     * \param event the event
     */
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *event );

    /** show the color picker dialog */
    void showColorPicker();

private:

    /** the radius used for paining of the handle */
    double radius;

    /** the entry left of this entry */
    WTransferFunctionColorPoint *left;

    /** the entry to the right of our entry */
    WTransferFunctionColorPoint *right;

    /** the parent widget */
    WTransferFunctionWidget* _parent;

    /** the color entry */
    QColor color;
};

#endif  // WTRANSFERFUNCTIONCOLORPOINT_H

