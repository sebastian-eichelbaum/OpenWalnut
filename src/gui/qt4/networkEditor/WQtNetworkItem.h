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

#ifndef WQTNETWORKITEM_H
#define WQTNETWORKITEM_H

#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsTextItem>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>
#include <QtGui/QColor>
#include "../../../kernel/WModule.h"

#include "WQtNetworkInputPort.h"
#include "WQtNetworkOutputPort.h"

/**
 * This class represents a WModule as graphicitem and
 * contain a reference to its in- and outports as well as
 */
class WQtNetworkItem : public QGraphicsRectItem
{
    public:
        /**
         * Constructor of the item.
         */
        explicit WQtNetworkItem( WModule *module );

        /**
         * Destructor.
         */
        virtual ~WQtNetworkItem();

        /**
         * Add a port to the item.
         */
        void addInputPort( WQtNetworkInputPort *inPort );
        
        /**
         * Add a port to the item.
         */
        void addOutputPort( WQtNetworkOutputPort *outPort );

        /**
         * Returns the item inports.
         * \return the item inports
         */
        QList< WQtNetworkInputPort *> getInPorts();

        /**
         * Returns the item outports
         * \return the item outports
         */
        QList< WQtNetworkOutputPort *> getOutPorts();

        /**
         * This method aligns the in- and outports as well as the modulename
         * in a regular way.
         */
        void fitLook();

        /**
         * Set the QGraphicsTextItem ( the caption ) of the item
         */
        void setTextItem( QGraphicsTextItem *text );

        /**
         * Get the caption as QString
         */
        QString getText();

        WModule* getModule();

        void activate( bool active );

    protected:

        /**
         * If the item is changed we want to get notified.
         *
         * \param change
         * \param value
         * \return
         */
        QVariant itemChange( GraphicsItemChange change, const QVariant &value );

        /**
         * If the WQtNetworkItem is moved, then the contained ports have to update
         * the connected WQtNetworkArrows for correct alignment.
         *
         * \param mouseEvent the mouse event
         */
        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );
//        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );
//        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

        /**
         * If the cursor enters the item, the item gets a green color.
         *
         * \param event the hover event
         */
        void hoverEnterEvent( QGraphicsSceneHoverEvent  *event );

        /**
         * If the cursor leaves the item, the item gets his default color.
         *
         * \param event the hover event
         */
        void hoverLeaveEvent( QGraphicsSceneHoverEvent  *event );

        /**
         * This method changes the coloration of gradient.
         * 
         * \param color the choosen color
         */
        void changeColor( QColor color );

        /**
         * Draw some customized stuff in the scene.
         *
         * \param painter
         * \param option
         * \param w
         */
        void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* w );


    private:
        
        WModule *m_module; //!< the module

        QList< WQtNetworkInputPort *> m_inPorts; //!< the input ports of the item
        QList< WQtNetworkOutputPort *> m_outPorts; //!< the output ports of the item

        QLinearGradient m_gradient; //!< the gradient for a nice coloring of the item
        QColor m_color; //!< the current color

        QRectF m_rect; //!< the size of the items rect
        float m_width; //!< the width of the rect
        float m_height; //!< the height of the rect

        QGraphicsTextItem *m_text; //!< the caption
};
#endif  // WQTNETWORKITEM_H
