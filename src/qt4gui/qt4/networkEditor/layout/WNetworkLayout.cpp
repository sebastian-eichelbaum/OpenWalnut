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

#include "WNetworkLayoutGlobals.h"

#include "WNetworkLayout.h"

WNetworkLayout::WNetworkLayout()
{
}

WNetworkLayout::~WNetworkLayout()
{
    for( std::list< WNetworkLayoutItem * >::iterator iter = laneList.begin(); iter != laneList.end(); ++iter )
    {
        delete *iter;
    }
}

void WNetworkLayout::addItem( WQtNetworkItem *item )
{
    WNetworkLayoutItem *layoutItem = new WNetworkLayoutItem( item );
    if( laneList.size() != 0 )
    {
        laneList.back()->setNeighbour( layoutItem );
        QPointF pos( laneList.back()->getPos() );
        pos.setX( pos.x() + laneList.back()->getWidth() );
        layoutItem->setPos( pos );
    }
    else
    {
        layoutItem->setPos( QPointF( 0, 0 ) );
    }
    laneList.push_back( layoutItem );
}

void WNetworkLayout::connectItems( WQtNetworkItem *parent, WQtNetworkItem *child )
{
    if( child->getLayoutItem()->getParent() == NULL )
    {
        // update linked list & move item into the parents lane
        for( std::list< WNetworkLayoutItem * >::iterator iter = laneList.begin(); iter != laneList.end();
                ++iter )
        {
            if( ( *iter )->getNeighbour() == child->getLayoutItem() )
            {
                ( *iter )->setNeighbour( child->getLayoutItem()->getNeighbour() );
            }
        }
    }
    child->getLayoutItem()->setParent( parent->getLayoutItem() );
    laneList.pop_back();
}

void WNetworkLayout::disconnectItem( WQtNetworkItem *child )
{
    // disconnect items
    // move child to new lane
    laneList.back()->setNeighbour( child->getLayoutItem() );
    QPointF pos( laneList.back()->getPos() );
    pos.setX( pos.x() + laneList.back()->getWidth() );
    child->getLayoutItem()->disconnectFromParent( pos );
    laneList.push_back( child->getLayoutItem() );
}

void WNetworkLayout::removeItem( WQtNetworkItem *item )
{
    // remove the item:
    // update lane info
    if( item->getLayoutItem() == laneList.front() )
    {
        if( laneList.size() > 1 )
        {
            item->getLayoutItem()->getNeighbour()->setPos( item->getLayoutItem()->getPos() );
        }
        laneList.pop_front();
    }
    else
    {
        for( std::list< WNetworkLayoutItem * >::iterator iter = laneList.begin(); iter != laneList.end(); ++iter )
        {
            if( ( *iter )->getNeighbour() == item->getLayoutItem() )
            {
                ( *iter )->setNeighbour( item->getLayoutItem()->getNeighbour() );
                if( item->getLayoutItem()->getNeighbour() != NULL )
                {
                    // update the position of all lanes( -> items ) following the removed item
                    ( *iter )->getNeighbour()->setPos( item->getLayoutItem()->getPos() );
                }
                laneList.erase( ++iter );
                break;
            }
        }
    }
}

