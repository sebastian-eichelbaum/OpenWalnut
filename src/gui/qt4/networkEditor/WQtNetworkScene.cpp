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

#include <boost/shared_ptr.hpp>

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
#include "../../../kernel/combiner/WDisconnectCombiner.h"
#include "../../../kernel/WKernel.h"

WQtNetworkScene::WQtNetworkScene()
    : QGraphicsScene()
{
}

WQtNetworkScene::~WQtNetworkScene()
{
}

void WQtNetworkScene::keyPressEvent( QKeyEvent *keyEvent )
{
    switch ( keyEvent->key() )
    {
        case Qt::Key_Delete:
            {
                QList< WQtNetworkItem *> itemList;
                QList< WQtNetworkArrow *> arrowList;
                foreach( QGraphicsItem *item, this->selectedItems() )
                {
                    if ( item->type() == WQtNetworkItem::Type )
                    {
                        WQtNetworkItem *netItem = qgraphicsitem_cast<WQtNetworkItem *>( item );
                        itemList.append( netItem );
                    }
                    else if( item->type() == WQtNetworkArrow::Type )
                    {
                        WQtNetworkArrow *netArrow = qgraphicsitem_cast<WQtNetworkArrow *>( item );
                        arrowList.append( netArrow );
                    }
                }

                foreach( WQtNetworkArrow *ar, arrowList )
                {
                    if( ar != 0 )
                    {
                        boost::shared_ptr< WDisconnectCombiner > disconnectCombiner =
                            boost::shared_ptr< WDisconnectCombiner >( new WDisconnectCombiner(
                                        ar->getStartPort()->getConnector()->getModule(),
                                        ar->getStartPort()->getConnector()->getName(),
                                        ar->getEndPort()->getConnector()->getModule(),
                                        ar->getEndPort()->getConnector()->getName() ) );
                        disconnectCombiner->run();
                        disconnectCombiner->wait();
                    }
                }

                foreach( WQtNetworkItem *it, itemList )
                {
                    if( it != 0 )
                    {
                        WKernel::getRunningKernel()->getRootContainer()->remove( it->getModule() );
                    }
                }
                itemList.clear();
                arrowList.clear();
            }
    }
}

void WQtNetworkScene::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    QList<QGraphicsItem *> item = this->items( mouseEvent->scenePos() );
    if( item.isEmpty() )
    {
        emit selectionChanged();
    }

    QGraphicsScene::mousePressEvent( mouseEvent );
}

void WQtNetworkScene::setFakeItem( QGraphicsItem *fake )
{
    m_fakeItem = fake;
}

QGraphicsItem* WQtNetworkScene::getFakeItem()
{
    return m_fakeItem;
}
