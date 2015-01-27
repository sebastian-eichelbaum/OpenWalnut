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

#include <QDockWidget>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QGraphicsItemGroup>
#include <QGraphicsSceneMouseEvent>

#include "../WMainWindow.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkPort.h"

#include "WQtNetworkScene.h"

WQtNetworkScene::WQtNetworkScene( QObject* parent )
    : QGraphicsScene( parent )
{
    setItemIndexMethod( NoIndex );
    // this takes care of the segfault: QGraphicsSceneFindItemBspTreeVisitor::visit
    // seems to be a Qt bug, testet with qt4 4.7.0
}

WQtNetworkScene::~WQtNetworkScene()
{
}

void WQtNetworkScene::dragEnterEvent( QGraphicsSceneDragDropEvent* event )
{
    // only ask the main window
    if( WMainWindow::isDropAcceptable( event->mimeData() ) )
    {
        event->acceptProposedAction();
    }
}

void WQtNetworkScene::dragMoveEvent( QGraphicsSceneDragDropEvent* event )
{
    if( WMainWindow::isDropAcceptable( event->mimeData() ) )
    {
        event->acceptProposedAction();
    }
}

