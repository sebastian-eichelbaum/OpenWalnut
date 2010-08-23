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

#include "WQtNetworkPort.h"

class WQtNetworkItem : public QGraphicsRectItem
{
    public:
        explicit WQtNetworkItem();

        virtual ~WQtNetworkItem();

        void addPort( WQtNetworkPort *port );

        QList< WQtNetworkPort *> getInPorts();
        QList< WQtNetworkPort *> getOutPorts();

        void fitLook();
        
        void setTextItem( QGraphicsTextItem *text );

    protected:

        QVariant itemChange( GraphicsItemChange change, const QVariant &value );

//        void removePort( WQtNetworkPort *port );
//        void removePorts();

//        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );
        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );
//        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );
        
        void hoverEnterEvent(QGraphicsSceneHoverEvent  *event);
        void hoverLeaveEvent(QGraphicsSceneHoverEvent  *event);
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget=0 );

        void changeColor( QColor color );


    private:

        QList< WQtNetworkPort *> m_inPorts; // input ports
        QList< WQtNetworkPort *> m_outPorts; // output ports

        QLinearGradient m_gradient;
        QColor m_color;

        QRectF m_rect;
        float m_width;
        float m_height;

        QGraphicsTextItem *m_text;
};
#endif  // WQTNETWORKITEM_H
