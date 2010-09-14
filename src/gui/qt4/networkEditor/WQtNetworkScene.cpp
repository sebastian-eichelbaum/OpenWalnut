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

#include <string>
#include <iostream>

#include <QtGui/QDockWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsView>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsItemGroup>
#include <QtGui/QGraphicsSceneMouseEvent>

#include "WQtNetworkScene.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"

WQtNetworkScene::WQtNetworkScene()
    : QGraphicsScene()
{
//    line = 0;
}

WQtNetworkScene::~WQtNetworkScene()
{
}

void WQtNetworkScene::keyPressEvent( QKeyEvent *keyEvent )
{
    if( keyEvent->key() == Qt::Key_Delete )
    {
        std::cout << "delete" << std::endl;
    
        foreach (QGraphicsItem *item, this->selectedItems()) {
         if ( item->type() == WQtNetworkItem::Type)
         {
             WQtNetworkItem *netItem = qgraphicsitem_cast<WQtNetworkItem *>( item );
             
             foreach( WQtNetworkPort *port, netItem->getInPorts() )
             {
                 port->removeArrows();
                 delete port;
             }
             foreach( WQtNetworkPort *port, netItem->getOutPorts() )
             {
                 port->removeArrows();
                 delete port;
             }
 
             removeItem( item );
         }
         else if( item->type() == WQtNetworkArrow::Type)
         {
             WQtNetworkArrow *netArrow = qgraphicsitem_cast<WQtNetworkArrow *>( item );
             netArrow->getStartPort()->removeArrow( netArrow );
             netArrow->getEndPort()->removeArrow( netArrow );
             removeItem( item );
         }
         delete item;
     }
    }
}

//void WQtNetworkScene::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
//{
//    QGraphicsItem* item = itemAt( mouseEvent->pos() );
//    if ( item->type() == WQtNetworkPort::Type ){
//        std::cout << "ja" << std::endl;
//    } else {
//        QGraphicsScene::mousePressEvent( mouseEvent );
//    }
//    std::cout << item->type() << std::endl;
//    std::cout << WQtNetworkPort::Type << std::endl;
//}

//void WQtNetworkScene::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
//{
//}

//void WQtNetworkScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *mouseEvent )
//{
//}
