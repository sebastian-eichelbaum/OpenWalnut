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

#ifndef WTRANSFERFUNCTIONPOINT_H
#define WTRANSFERFUNCTIONPOINT_H

#include "QtGui/QGraphicsItem"

class WTransferFunctionWidget;
class WTransferFunctionLine;

/**
 * A control point for the alpha function.
 *
 * Internals:
 * The point has pointers to its left and right neighbors (double-linked list)
 * and a pointer to the line connecting this point to the next point to the right.
 */
class WTransferFunctionPoint : public QGraphicsItem
{
public:
    /** type of the base class */
    typedef QGraphicsItem BaseClass;

    /**
     * Default constructor.
     *
     * \param parent pointer to parent widget
     */
    explicit WTransferFunctionPoint( WTransferFunctionWidget* parent = NULL );

    /**
     * Default destructor.
     */
    virtual ~WTransferFunctionPoint();

    /**
     * Set point to the left.
     *
     * \param left new point to the left
     */
    void setLeft( WTransferFunctionPoint* left );

    /**
     * Get point to the left.
     *
     * \returns point to the left
     */
    WTransferFunctionPoint *getLeft() const;

    /**
     * Set point to the right.
     *
     * \param right new point to the right
     */
    void setRight( WTransferFunctionPoint* right );

     /**
     * Get point to the right.
     *
     * \returns point to the right
     */
    WTransferFunctionPoint *getRight() const;

     /**
      * Set the line pointing to the right.
      *
      * \param line the new line
      */
    void setLine( WTransferFunctionLine* line );

     /**
      * The current line if there is one. NULL if not.
      *
      * \returns the line if any has been set (i.e., may be 0)
      */
    WTransferFunctionLine* getLine() const;

     /**
      * Get the bounding rectangle.
      *
      * \returns the bounding rect of this object
      */
    QRectF boundingRect() const;

    /**
     * Paint the object.
     *
     * \param painter the painter to use
     * \param option the options for painting
     */
    virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* );

    /**
     * Overloaded form base class for debugging.
     *
     * \param point the point that should be set
     */
    virtual void setPos( QPointF point );

protected:
    /**
     * Helper to itemChange.
     *
     * \param pos position that is clamped to rectangle
     * \param rectangle the rectangle
     */
    void clampToRectangle( QPointF* const pos, const QRectF& rectangle ) const;

    /**
     * Helper to itemChange.
     *
     * \param pos the position that is clamped between the left and right neighbors
     */
    void clampToLeftAndRight( QPointF* const pos ) const;

    /**
     * Handle item changes and change requests.
     *
     * \param change the proposed change
     * \param value the proposed value
     * \returns the requested change
     */
    QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    /**
     * Handle mouse press events for selections and highlighting.
     *
     * \param event the handled event
     */
    void mousePressEvent( QGraphicsSceneMouseEvent *event );

private:
    /** the radius of the object */
    double radius;

    /** pointer to point to the left*/
    WTransferFunctionPoint *left;
    /** pointer to point to the right*/
    WTransferFunctionPoint *right;

    /** pointer to line to the right */
    WTransferFunctionLine *line;

    /** reference to the parent widget */
    WTransferFunctionWidget* _parent;
};

#endif  // WTRANSFERFUNCTIONPOINT_H

