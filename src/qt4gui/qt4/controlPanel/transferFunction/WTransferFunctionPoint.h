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
        typedef QGraphicsItem BaseClass;

        explicit WTransferFunctionPoint( WTransferFunctionWidget* parent = 0x0 );
        virtual ~WTransferFunctionPoint();

        void setLeft( WTransferFunctionPoint* );
        WTransferFunctionPoint *getLeft() const;

        void setRight( WTransferFunctionPoint* );
        WTransferFunctionPoint *getRight() const;

        void setLine( WTransferFunctionLine* line );
        WTransferFunctionLine* getLine() const;

        QRectF boundingRect() const;

        virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* );

        //! overloaded form base class for debugging
        virtual void setPos( QPointF point );
    protected:

        void clampToRectangle( QPointF* const pos, const QRectF& rectangle ) const;
        void clampToLeftAndRight( QPointF* const pos ) const;

        QVariant itemChange( GraphicsItemChange change, const QVariant &value );

        void mousePressEvent( QGraphicsSceneMouseEvent *event );

    private:
        double radius;

        WTransferFunctionPoint *left;
        WTransferFunctionPoint *right;

        WTransferFunctionLine *line;

        WTransferFunctionWidget* _parent;
};

#endif  // WTRANSFERFUNCTIONPOINT_H

