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

#ifndef WQTNETWORKPORT_H
#define WQTNETWORKPORT_H

#include <QtGui/QGraphicsRectItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsScene>

#include "WQtNetworkArrow.h"

class WQtNetworkArrow;

class WQtNetworkPort : public QGraphicsRectItem
{
    public:
        enum
        {
            Type = UserType + 10
        };

        explicit WQtNetworkPort();

        virtual ~WQtNetworkPort();

        int type() const
        {
            return Type;
        }

        void update();


    protected:

        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );

        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

        //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        void addArrow( WQtNetworkArrow *arrow );
        void removeArrows();
        void removeArrow( WQtNetworkArrow *arrow );

    private:

        QGraphicsLineItem *line;
        QPointF startPoint;

        QList< WQtNetworkArrow *> m_arrows;
};
#endif  // WQTNETWORKPORT_H
