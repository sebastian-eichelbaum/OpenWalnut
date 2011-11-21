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
        typedef QGraphicsLineItem BaseClass;

        explicit WTransferFunctionLine( WTransferFunctionWidget* parent = 0x0 );
        virtual ~WTransferFunctionLine();

        WTransferFunctionPoint *getLeft() const;
        WTransferFunctionPoint *getRight() const;
        void setLeft( WTransferFunctionPoint* );
        void setRight( WTransferFunctionPoint* );

        virtual void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget* );

    protected:
        // to be implemented: how do we want to handle moves of lines?
        // I think a vertical move may be nice, but may even be implemented using
        // itemChange(...)
        //void mouseMoveEvent( QGraphicsSceneMouseEvent *event );
        //void mousePressEvent( QGraphicsSceneMouseEvent *event );
        //void mouseReleaseEvent( QGraphicsSceneMouseEvent *event );

    private:
        double lineWidth;

        WTransferFunctionPoint *left;
        WTransferFunctionPoint *right;

        WTransferFunctionWidget* _parent;
};

#endif  // WTRANSFERFUNCTIONLINE_H

