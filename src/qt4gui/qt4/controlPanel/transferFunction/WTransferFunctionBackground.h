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

#ifndef WTRANSFERFUNCTIONBACKGROUND_H
#define WTRANSFERFUNCTIONBACKGROUND_H

#include "QtGui/QGraphicsPixmapItem"

class WTransferFunctionWidget;

/**
 * A QGraphicsItem that displays a pixmap in the background of the scene.
 *
 * Compared to the QT implementation, the function setMyPixmap (which has
 * to be used because setPixmap is not virutal) scales the item to the size
 * of the scene, so it covers the entire area.
 */
class WTransferFunctionBackground : public QGraphicsPixmapItem
{
    public:
        /** type of base class */
        typedef QGraphicsPixmapItem BaseClass;

        /** default constructor
         * \param parent pointer to parent widget
         */
        explicit WTransferFunctionBackground( WTransferFunctionWidget* parent = 0x0 );

        /**
         * default destructor
         */
        virtual ~WTransferFunctionBackground();

        /**
         * get the bounding rectangle
         * \returns the bounding rectangle
         */
        QRectF boundingRect() const;

        /**
         * Set the background pixmap that will be displayed.
         * In our case, this is the transfer function
         * \param pixmap the pixmap to display
         */
        void setMyPixmap( const QPixmap& pixmap );
};

#endif  // WTRANSFERFUNCTIONBACKGROUND_H

