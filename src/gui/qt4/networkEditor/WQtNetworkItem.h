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

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsScene>

#include "WQtNetworkPort.h"

class WQtNetworkItem : public QGraphicsRectItem
{
    public:
        explicit WQtNetworkItem();

        virtual ~WQtNetworkItem();

        void addPort( WQtNetworkPort *port );

        QList< WQtNetworkPort *> getPorts();

        void fitLook();
    protected:

//        QVariant itemChange( GraphicsItemChange change, const QVariant &value );

        void removePort( WQtNetworkPort *port );

        void removePorts();

//        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );
        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );
//        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

    private:

        QList< WQtNetworkPort *> m_ports;

        float m_width;
        float m_heigth;
};
#endif  // WQTNETWORKITEM_H
