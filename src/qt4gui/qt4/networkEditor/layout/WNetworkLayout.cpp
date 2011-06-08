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
    for( unsigned int x = 0; x != WNETWORKLAYOUT_GRID_X; ++x )
    {
        for( unsigned int y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
        {
            m_array[x][y] = 0;
        }
    }
}

WNetworkLayout::~WNetworkLayout()
{
}

void WNetworkLayout::addItem( WQtNetworkItem *item )
{
    WNetworkLayoutItem *layoutItem = new WNetworkLayoutItem( item );
    unsigned int x = 0;
    if( m_subgraphList.size() > 0 )
    {
        x = m_subgraphList.back().getRightmostItem()->getGridPos().x() + 1;
    }

    unsigned char id = static_cast< unsigned char >( m_subgraphList.size() + 1 );
    m_array[x][0] = id;
    layoutItem->setGridPos( QPointF( x, 0 ) );
    m_subgraphList.push_back( WNetworkLayoutSubgraph( id, layoutItem ) );
}

void WNetworkLayout::connectItems( WQtNetworkItem *parent, WQtNetworkItem *child )
{
    //if( parent->getLayoutItem()->getId() == child->getLayoutItem()->getId() )
    //{
    //    // both item belong already to the same graph
    //}
    if( child->getLayoutItem()->getGridPos().y() != 0 )
        // child belongs to a graph consisting of only more than one node
    {
        merge( parent, child );
    }
    else
    {
        unsigned int x = child->getLayoutItem()->getGridPos().x();
        m_array[x][0] = 0;
        for( std::list< WNetworkLayoutSubgraph >::iterator iter = m_subgraphList.begin();
                iter != m_subgraphList.end(); ++iter )
        {
            if( iter->find( child->getLayoutItem() ) )
            {
                iter->remove( child->getLayoutItem() );
                if( iter->size() == 0 )
                {
                    updateGrid( iter->getId(), x, false );
                    iter = m_subgraphList.erase( iter );
                    // update subgraph positions, those who were to the right of the removed
                    // subgraph
                    for( ; iter != m_subgraphList.end(); ++iter )
                    {
                        iter->manipulate( -1 );
                        // OMG update m_array
                    }
                }
                break;
            }
        }

        // add the child to the subgraph of parent
        std::list< WNetworkLayoutSubgraph >::iterator parentSubgraph;
        unsigned char graphId;
        for( parentSubgraph = m_subgraphList.begin(); parentSubgraph != m_subgraphList.end(); ++parentSubgraph )
        {
            if( parentSubgraph->find( parent->getLayoutItem() ) )
            {
                graphId = parentSubgraph->getId();
                break;
            }
        }

        // TODO remove offset if not needed
        QPointF parentPos = parent->getLayoutItem()->getGridPos();
        for( x = parentPos.x(); x != WNETWORKLAYOUT_GRID_X; ++x )
        {
            unsigned int y = parentPos.y() + 1;
            if( m_array[x][y] == 0 )
            {
                if( m_array[x][0] != 0 && m_array[x][0] != graphId )
                {
                    updateGrid( graphId, x );
                    std::list< WNetworkLayoutSubgraph >::iterator iter = parentSubgraph;
                    for( ++iter; iter != m_subgraphList.end(); ++iter )
                    {
                        iter->manipulate( 1 ); // move one to the right
                    }
                }
                child->getLayoutItem()->setGridPos( QPointF( x, y ) );
                parentSubgraph->add( child->getLayoutItem() );
                m_array[x][y] = graphId;
                break;
            }
        }
    }
}

void WNetworkLayout::disconnectItem( WQtNetworkItem *item )
{
    // remove old pos data
    unsigned int x = item->getLayoutItem()->getGridPos().x();
    unsigned int y = item->getLayoutItem()->getGridPos().y();
    m_array[x][y] = 0;

    // test if positions of the other subgraphs has to be updated
    if( !m_subgraphList.back().find( item->getLayoutItem() ) )
    {
        bool empty = true;
        for( y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
        {
            if( m_array[x][y] != 0 )
            {
                empty = false;
                break;
            }
        }
        for( std::list< WNetworkLayoutSubgraph >::iterator iter = m_subgraphList.begin();
                iter != m_subgraphList.end(); ++iter )
        {
            if( iter->find( item->getLayoutItem() ) )
            {
                iter->remove( item->getLayoutItem() );
                if( empty )
                {
                    updateGrid( iter->getId(), x, false );
                    // update righthand neighbours
                    iter++;
                    for( ; iter != m_subgraphList.end(); ++iter )
                    {
                        iter->manipulate( -1 ); // move one to the left
                    }
                }
                break;
            }
        }
    }
    else
    {
        m_subgraphList.back().remove( item->getLayoutItem() );
    }

    // set new pos data
    x = m_subgraphList.back().getRightmostItem()->getGridPos().x() + 1;
    unsigned char id = static_cast< unsigned char >( m_subgraphList.size() + 1 );
    m_array[x][0] = id;
    item->getLayoutItem()->setGridPos( QPointF( x, 0 ) );
    m_subgraphList.push_back( WNetworkLayoutSubgraph( id, item->getLayoutItem() ) );
}

void WNetworkLayout::removeItem( WQtNetworkItem *item )
{
    unsigned int x = item->getLayoutItem()->getGridPos().x();
    unsigned int y = item->getLayoutItem()->getGridPos().y();
    m_array[x][y] = 0;

    for( std::list< WNetworkLayoutSubgraph >::iterator iter = m_subgraphList.begin();
            iter != m_subgraphList.end(); ++iter )
    {
        if( iter->find( item->getLayoutItem() ) )
        {
            iter->remove( item->getLayoutItem() );
            if( iter->size() == 0 )
            {
                updateGrid( iter->getId(), x, false );
                iter = m_subgraphList.erase( iter );
                // update subgraph positions, thos who were to the right of the removed
                // subgraph
                for( ; iter != m_subgraphList.end(); ++iter )
                {
                    iter->manipulate( -1 );
                }
            }
            break;
        }
    }
}

// privat

void WNetworkLayout::updateGrid( unsigned int fixedGraph, unsigned int x, bool shiftRight, int distance )
{
    if( shiftRight )
    {
        for( ; x != 0 ; --x )
        {
            bool stop = false;
            for( unsigned int y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
            {
                if( m_array[x][y] != fixedGraph )
                {
                    m_array[ x + distance ][y] = m_array[x][y];
                    m_array[x][y] = 0;
                }
                else
                {
                    stop = true;
                    break;
                }
            }
            if( stop )
            {
                break;
            }
        }
    }
    else
    {
        for( ; x > m_subgraphList.back().getRightmostItem()->getGridPos().x(); ++x )
        {
            for( unsigned int y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
            {
                if( m_array[x][y] != fixedGraph )
                {
                    m_array[x][y] = m_array[ x + 1 ][y];
                    m_array[ x + 1 ][y] = 0;
                }
            }
        }
    }
}

void WNetworkLayout::merge( WQtNetworkItem *parent, WQtNetworkItem *child )
{
//    if( parent->getLayoutItem()->getGridPos().y() + 1 > child->getLayoutItem()->getGridPos().y() )
//    {
//        WQtNetworkItem* help = parent;
//        parent = child;
//        child = help;
//    }
    float offsetY = ( parent->getLayoutItem()->getGridPos().y() + 1 ) - child->getLayoutItem()->getGridPos().y();
    if( offsetY > 0 )
    {
        // change child
        QPointF pos = child->getLayoutItem()->getGridPos();
        pos.setY( pos.y() + offsetY );
        child->getLayoutItem()->setGridPos( pos );
        // offsetY auf child
    }
    else if( offsetY < 0 )
    {
        // change parent
        QPointF pos = parent->getLayoutItem()->getGridPos();
        pos.setY( pos.y() - offsetY );
        parent->getLayoutItem()->setGridPos( pos );
        // offset auf parent
    }

    // calculate size
    // first we need to know to which subgraph both items belong
    std::list< WNetworkLayoutSubgraph >::iterator parentSubgraph = m_subgraphList.end();
    std::list< WNetworkLayoutSubgraph >::iterator childSubgraph = m_subgraphList.end();
    unsigned int distance; // distance between the two subgraphs
    std::list< WNetworkLayoutSubgraph >::iterator iter;
    for( iter = m_subgraphList.begin(); iter != m_subgraphList.end(); ++iter )
    {
        if( parentSubgraph != childSubgraph )
        {
            distance++;
        }
        if( iter->find( parent->getLayoutItem() ) )
        {
            parentSubgraph = iter;
            if( childSubgraph != m_subgraphList.end() )
            {
                break;
            }
        }
        if( iter->find( child->getLayoutItem() ) )
        {
            childSubgraph = iter;
            if( parentSubgraph != m_subgraphList.end() )
            {
                break;
            }
        }
    }
    if( parentSubgraph->getId() == childSubgraph->getId() )
    {
        return;
    }


    //
    // ------------------------------- REDONE ---------------------------------------------------
    //

    // calculate the width of the subgraphs and the height where the item is
    std::pair< unsigned int, unsigned int > firstValues, secondValues; // first = dx; second = dy
    for( int x = parent->getLayoutItem()->getGridPos().x(); x < 0; --x )
    {
        if( m_array[x][0] != 0 ) // &&  == subgraphID
        {
            firstValues.first = parentSubgraph->getRightmostItem()->getGridPos().x() - x + 1;
            break;
        }
    }
    firstValues.second = parent->getLayoutItem()->getGridPos().y();

    for( int x = child->getLayoutItem()->getGridPos().x(); x < 0; --x )
    {
        if( m_array[x][0] != 0 ) // &&  == subgraphID
        {
            secondValues.first = childSubgraph->getRightmostItem()->getGridPos().x() - x + 1;
            break;
        }
    }
    secondValues.second = child->getLayoutItem()->getGridPos().y();

    // change position of the two graphs that are going to be merged
//    if( first->getLayoutItem()->getGridPos().y() > child->getLayoutItem()->getGridPos().y() ) //firstValues.second > secondValues.second )
//    {
        if( parent->getLayoutItem()->getGridPos().x() < child->getLayoutItem()->getGridPos().x() )
        {
            distance *= -1;
        }
        updateGrid( parentSubgraph->getId(), parentSubgraph->getRightmostItem()->getGridPos().x(),
                false, secondValues.first );
        iter = parentSubgraph;
        for( ++iter; iter != m_subgraphList.end(); ++iter )
        {
            iter->manipulate( secondValues.first ); // make the space for the second subgraph
        }

        // change position of the second subgraph
        // TODO deltaY never equals zero, make sure
        unsigned int deltaY =
            parent->getLayoutItem()->getGridPos().y() - child->getLayoutItem()->getGridPos().y();
        for( unsigned int x = child->getLayoutItem()->getGridPos().x();
                x != child->getLayoutItem()->getGridPos().x() + secondValues.first; ++x )
        {
            for( unsigned int y = deltaY; y != WNETWORKLAYOUT_GRID_Y; ++y )
            {
                m_array[ x + distance ][y] = parentSubgraph->getId();
                m_array[x][ y - deltaY ] = 0;
            }
        }

        // close the gap if needed
        if( !m_subgraphList.back().find( child->getLayoutItem() ) )
        {
            // TODO direction
            iter = childSubgraph;
            ++iter;
            updateGrid( iter->getId(), iter->getRightmostItem()->getGridPos().x(),
                    false, secondValues.first );
            for( unsigned int x = child->getLayoutItem()->getGridPos().x();
                    x <= m_subgraphList.back().getRightmostItem()->getGridPos().x(); ++x )
            {
                for( unsigned int y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
                {
                    m_array[x][y] = m_array[ x + secondValues.first ][y];
                    m_array[ x + secondValues.first ][y] = 0;
                }
            }
            iter = childSubgraph;
            for( ++iter; childSubgraph != m_subgraphList.end(); ++iter )
            {
                iter->manipulate( secondValues.first ); // length of the second subgraph in x direction
            }
        }

        // TODO
        if( parent->getLayoutItem()->getGridPos().x() < parentSubgraph->getRightmostItem()->getGridPos().x() )
        {
            QPointF newPos = parent->getLayoutItem()->getGridPos();
            newPos.setY( parentSubgraph->getRightmostItem()->getGridPos().y() );

            QPoint oldPos = parent->getLayoutItem()->getGridPos().toPoint();
            unsigned char help = m_array[oldPos.x()][oldPos.y()];
            m_array[oldPos.x()][oldPos.y()] = m_array[newPos.toPoint().x()][newPos.toPoint().y()];
            m_array[newPos.toPoint().x()][newPos.toPoint().y()] = help;

            parentSubgraph->swap( parent->getLayoutItem()->getGridPos(), newPos );
        }
        // TODO remove comment
        if( parent->getLayoutItem()->getGridPos().x() /*+ 1*/ != child->getLayoutItem()->getGridPos().x() )
        {
            QPointF newPos = child->getLayoutItem()->getGridPos();
            newPos.setX( parent->getLayoutItem()->getGridPos().x() + 1 );

            QPoint oldPos = parent->getLayoutItem()->getGridPos().toPoint();
            unsigned char help = m_array[oldPos.x()][oldPos.y()];
            m_array[oldPos.x()][oldPos.y()] = m_array[newPos.toPoint().x()][newPos.toPoint().y()];
            m_array[newPos.toPoint().x()][newPos.toPoint().y()] = help;

            childSubgraph->swap( child->getLayoutItem()->getGridPos(), newPos );
        }
        // change position within the graph
        childSubgraph->manipulate( distance, deltaY ); // where comes this from ?
        parentSubgraph->merge( &( *childSubgraph ) );
        m_subgraphList.erase( childSubgraph );
//    }
//    else
//    {
//        // TODO really needed ?
//        if( child->getLayoutItem()->getGridPos().x() > first->getLayoutItem()->getGridPos().x() )
//        {
//            distance *= -1;
//        }
//        updateGrid( parentSubgraph->getId(), parentSubgraph->getRightmostItem()->getGridPos().x(),
//                false, secondValues.first );
//        iter = parentSubgraph;
//        for( ++iter; iter != m_subgraphList.end(); ++iter )
//        {
//            iter->manipulate( secondValues.first ); // make the space for the second subgraph
//        }
//    }

    // change position of the two nodes if need be
    // change the ID of one graph
}

