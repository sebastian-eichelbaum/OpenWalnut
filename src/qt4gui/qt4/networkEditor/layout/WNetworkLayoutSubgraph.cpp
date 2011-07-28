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

#include "WNetworkLayoutSubgraph.h"

WNetworkLayoutSubgraph::WNetworkLayoutSubgraph( unsigned char id, WNetworkLayoutItem *item )
{
    m_id = id;
    if( item )
    {
        add( item );
        m_rightmostItem = item;
    }
}

WNetworkLayoutSubgraph::~WNetworkLayoutSubgraph()
{
}

void WNetworkLayoutSubgraph::add( WNetworkLayoutItem *item )
{
    if( m_nodes.size() == 0 )
    {
        m_rightmostItem = item;
    }
    else if( item->getGridPos().x() > m_rightmostItem->getGridPos().x() )
    {
        m_rightmostItem = item;
    }
    m_nodes.push_back( item );
    // TODO if m_source is used, change here
}

std::list< WNetworkLayoutItem * > * WNetworkLayoutSubgraph::data()
{
    return &m_nodes;
}

bool WNetworkLayoutSubgraph::find( WNetworkLayoutItem *item )
{
    return ( std::find( m_nodes.begin(), m_nodes.end(), item ) != m_nodes.end() );
}

unsigned char WNetworkLayoutSubgraph::getId()
{
    return m_id;
}

WNetworkLayoutItem * WNetworkLayoutSubgraph::getRightmostItem()
{
    return m_rightmostItem;
}

void WNetworkLayoutSubgraph::manipulate( int x, int y )
{
    for( std::list< WNetworkLayoutItem * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
    {
        QPointF pos = ( *iter )->getGridPos();
        pos.setX( pos.x() + x );
        pos.setY( pos.y() + y );
        ( *iter )->setGridPos( pos );
    }
}


void WNetworkLayoutSubgraph::merge( WNetworkLayoutSubgraph *subgraph )
{
    std::list< WNetworkLayoutItem * > *nodes = subgraph->data();
    for( std::list< WNetworkLayoutItem * >::iterator iter = nodes->begin(); iter != nodes->end(); ++iter )
    {
        m_nodes.push_back( *iter );
    }
}


void WNetworkLayoutSubgraph::remove( WNetworkLayoutItem *item )
{
    m_nodes.remove( item );
    if( m_rightmostItem == item )
    {
        m_rightmostItem = m_nodes.front();
        for( std::list< WNetworkLayoutItem * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
        {
            if( m_rightmostItem->getGridPos().x() < ( *iter )->getGridPos().x() )
            {
                m_rightmostItem = *iter;
            }
        }
    }
}

unsigned int WNetworkLayoutSubgraph::size()
{
    return m_nodes.size();
}


void WNetworkLayoutSubgraph::swap( const QPointF &first, const QPointF &second )
{
    if( first == second )
    {
        return;
    }

    // TODO redo this whole section, problem if: no node is at the position, no 'out of range' check
    WNetworkLayoutItem *firstItem = NULL, *secondItem = NULL;
    for( std::list< WNetworkLayoutItem * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
    {
        if( ( *iter )->getGridPos() == first )
        {
            firstItem = *iter;
            if( secondItem != NULL )
            {
                break;
            }
        }

        if( ( *iter )->getGridPos() == second )
        {
            secondItem = *iter;
            if( firstItem != NULL )
            {
                break;
            }
        }
    }
    if( firstItem != NULL )
    {
        firstItem->setGridPos( second );
    }
    if( secondItem != NULL )
    {
        secondItem->setGridPos( first );
    }
}


