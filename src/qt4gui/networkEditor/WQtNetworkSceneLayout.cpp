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

#include "core/common/WLogger.h"

#include "WQtNetworkEditor.h"
#include "WQtNetworkScene.h"
#include "WQtNetworkItem.h"
#include "WQtNetworkItemGrid.h"

#include "WQtNetworkSceneLayout.h"

WQtNetworkSceneLayout::WQtNetworkSceneLayout( WQtNetworkScene* scene, WQtNetworkItemGrid* grid ):
    m_scene( scene ),
    m_grid( grid )
{
    // initialize members
}

WQtNetworkSceneLayout::~WQtNetworkSceneLayout()
{
    // cleanup
}

void WQtNetworkSceneLayout::addItem( WQtNetworkItem* item )
{
    // add in first row of the next free column
    int nextX = m_grid->getFirstFreeColumn();
    m_grid->setItem( item, nextX, 0 );
}

void WQtNetworkSceneLayout::removeItem( WQtNetworkItem* item )
{
    // remove from grid
    m_grid->removeItem( item );
}

void WQtNetworkSceneLayout::connection( WQtNetworkItem* outItem, WQtNetworkItem* inItem )
{
    QPoint out = m_grid->whereIs( outItem );
    m_grid->moveItem( inItem, out.x(), out.y() + 1 );
}

void WQtNetworkSceneLayout::disconnection( WQtNetworkItem* outItem, WQtNetworkItem* inItem )
{
}

void WQtNetworkSceneLayout::snapTemporarily( WQtNetworkItem* item, QPointF worldCoords )
{
    // the grid knows where to snap:
    QPoint cell = m_grid->findNearestCell( worldCoords );
    QPoint oldCell = m_grid->whereIs( item );

    if( oldCell != cell )
    {
        bool isUsed = m_grid->isOccupied( cell );
        m_grid->highlightCell( cell, isUsed ? QColor( 230, 40, 40, 128 ) : QColor( 90, 220, 90, 128 ) );
    }
    else
    {
        // explicitly turn of highlight
        m_grid->highlightCell();
    }
    m_grid->physicalMoveTo( item, cell, false );
}

void WQtNetworkSceneLayout::snapAccept( WQtNetworkItem* item )
{
    // disable highlight
    m_grid->highlightCell();

    // something changed?
    QPoint oldCell = m_grid->whereIs( item );
    QPoint newCell = m_grid->findNearestCell( item->pos() );
    if( oldCell == newCell )
    {
        return;
    }

    // move in layout
    if( !m_grid->moveItem( item, newCell ) )
    {
        m_grid->physicalMoveTo( item, oldCell, true );
    }
}

void WQtNetworkSceneLayout::blendIn()
{
    m_grid->blendIn();
}

void WQtNetworkSceneLayout::blendOut()
{
    m_grid->blendOut();
}

QRectF WQtNetworkSceneLayout::getBoundingBox()
{
    return m_grid->boundingRect();
}
