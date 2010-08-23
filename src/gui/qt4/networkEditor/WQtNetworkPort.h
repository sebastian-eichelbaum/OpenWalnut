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

        explicit WQtNetworkPort();

        virtual ~WQtNetworkPort();
        
        enum
        {
            Type = UserType + 10
        };

        int type() const
        {
            return Type;
        }

        void update();

        void removeArrow( WQtNetworkArrow *arrow );

        void removeArrows();

        void alignPosition( int size, int portNumber, QRectF rect, bool inOut );

        void setType( bool type );
        bool portType();

        QString getName();
        void setName( QString str );

    protected:

        void mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent );

        void mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent );

        void mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent );

        //QVariant itemChange(GraphicsItemChange change, const QVariant &value);

        void addArrow( WQtNetworkArrow *arrow );

        void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );


    private:

        QGraphicsLineItem *line;
        QPointF startPoint;

        bool m_portType; // 0-input , 1-output

        QList< WQtNetworkArrow *> m_arrows;

        QString m_name;
};
#endif  // WQTNETWORKPORT_H
