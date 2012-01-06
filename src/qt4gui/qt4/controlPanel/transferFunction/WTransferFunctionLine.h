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

#ifndef WTRANSFERFUNCTIONLINE_H
#define WTRANSFERFUNCTIONLINE_H

#include "QtGui/QGraphicsLineItem"

class WTransferFunctionWidget;
class WTransferFunctionPoint;

/**
 * Line object for the connection of alpha control points.
 */
class WTransferFunctionLine : public QGraphicsLineItem
{
    public:
        /** type of the base class */
        typedef QGraphicsLineItem BaseClass;

        /** default constructor
         * \param parent parent widget
         */
        explicit WTransferFunctionLine( WTransferFunctionWidget* parent = 0x0 );

        /** default destructor */
        virtual ~WTransferFunctionLine();

        /**
         * \returns point to the left
         */
        WTransferFunctionPoint *getLeft() const;

        /**
         * \returns point to the right
         */
        WTransferFunctionPoint *getRight() const;

        /**
         * sets point to the left
         * \param left new point to the left
         */
        void setLeft( WTransferFunctionPoint* left );

        /**
         * sets point to the right
         * \param right new point to the right
         */
        void setRight( WTransferFunctionPoint* right );

        /**
         * paint the line
         * \param painter the painter used
         * \param option the style option used for painting
         * \param widget the widget used for painting
         */
        virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* widget );

    protected:
        // to be implemented: how do we want to handle moves of lines?
        // I think a vertical move may be nice, but may even be implemented using
        // itemChange(...)
        //void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
        //void mousePressEvent( QGraphicsSceneMouseEvent *event );
        //void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    private:
        /** the width of the line used for painting */
        double lineWidth;

        /** the point to the left */
        WTransferFunctionPoint *left;

        /** the point to the right */
        WTransferFunctionPoint *right;

        /** our parent widget */
        WTransferFunctionWidget* _parent;
};

#endif  // WTRANSFERFUNCTIONLINE_H

