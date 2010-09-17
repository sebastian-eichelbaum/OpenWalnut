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

#ifndef WQTNETWORKARROW_H
#define WQTNETWORKARROW_H

#include <string>
#include <iostream>

#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>

#include "WQtNetworkPort.h"

// forward declaration
class WQtNetworkPort;

/**
 * This Class is needed for connecting two ports, and drawing a line between
 * them.
 */
class WQtNetworkArrow : public QGraphicsLineItem
{
    public:

        /**
         * This customize the return value of type()
         */
        enum
        {
           Type = UserType + 4
        };

        /**
         * Constructor
         */
        explicit WQtNetworkArrow( WQtNetworkPort *outPort, WQtNetworkPort *inPort );

        /**
         * Destructor
         */ 
        ~WQtNetworkArrow();

        /**
         * Reimplementation from QGraphicsItem
         * \return the type of the item as int
         */
        int type() const;

        /**
         * Calculated the new position of the lines endpoints in the scene.
         * Is called everytime the parentItem is changed or after construction.
         */
        void updatePosition();

        /**
         * Returns the start port.
         *\return the port where the connection starts
         */
        WQtNetworkPort* getStartPort();

        /**
         * Returns the end port.
         *\return the port where the connection ends
         */
        WQtNetworkPort* getEndPort();

        /**
         * Reimplementation form QGraphicsItem, because the arrowhead is added
         * to the line. Its needed that QGraphicsView knows which area needs to
         * be redrawn.
         * \return a rect that bounds the drawn object
         */
        QRectF boundingRect() const;

        /**
         * Reimplementation from QGraphicsItem.
         * Return the shape of this item as a QPainterPath in local coordinates
         * \return shape of this
         */
        QPainterPath shape() const;

        /**
         * If the item is changed we want to get notified.
         *
         * \param change
         * \param value
         * \return
         */
        QVariant itemChange( GraphicsItemChange change, const QVariant &value );

    protected:

        /**
         * Draw some customized stuff in the scene.
         *
         * \param painter
         * \param option
         * \param w
         */
        void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w );

         /**
         * If the cursor enters the arrow, the arrow becomes geen.
         *
         * \param event the hover event
         */
        void hoverEnterEvent( QGraphicsSceneHoverEvent * event );

        /**
         * If the cursor leaves the arrow, the arrow gets his default color.
         *
         * \param event the hover event
         */
        void hoverLeaveEvent( QGraphicsSceneHoverEvent * event );

    private:

        /**
         * This method changes the color of the arrow.
         * 
         * \param color the choosen color
         */
        void changeColor( QColor color );

        WQtNetworkPort *m_startPort; //!< the start port
        WQtNetworkPort *m_endPort; //!< the end port

        QColor m_color; //!< the current color
        QPolygonF arrowHead; //!< the arrowhead
};
#endif  // WQTNETWORKARROW_H
