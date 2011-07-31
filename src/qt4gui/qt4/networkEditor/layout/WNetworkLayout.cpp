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
    /*for( unsigned int x = 0; x != WNETWORKLAYOUT_GRID_X; ++x )
    {
        for( unsigned int y = 0; y != WNETWORKLAYOUT_GRID_Y; ++y )
        {
            m_array[x][y] = 0;
        }
    }*/
}

WNetworkLayout::~WNetworkLayout()
{
}

void WNetworkLayout::addItem( WQtNetworkItem *item )
{
    WNetworkLayoutNode *layoutNode = new WNetworkLayoutNode( item );
    m_nodes.push_back( layoutNode );
    traverse();
}

void WNetworkLayout::connectItems( WQtNetworkItem *parent, WQtNetworkItem *child )
{
    parent->getLayoutNode()->add( child->getLayoutNode() );
    traverse();
}

void WNetworkLayout::disconnectNodes( WQtNetworkItem *parent, WQtNetworkItem *child )
{
    parent->getLayoutNode()->remove( child->getLayoutNode() );
    traverse();
}

void WNetworkLayout::removeItem( WQtNetworkItem *item )
{
    delete item->getLayoutNode();
}

// privat

void WNetworkLayout::traverse()
{
    std::list< WNetworkLayoutNode * > rootNodes, leafNodes;
    WNetworkLayoutNode dummyNode;
    for( std::list< WNetworkLayoutNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end();
            ++iter )
    {
        if( (*iter)->nParents() == 0 )
        {
            rootNodes.push_back( *iter );
            if( (*iter)->nChildren() > 1 )
            {
                for( int i = 0; i != (*iter)->nChildren() -1; ++i )
                {
                    // add dummy node to create a better layout, just as a simple heuristik
                    rootNodes.push_back( (&dummyNode) );
                }
            }
        }
        if( (*iter)->nChildren() == 0 )
        {
            leafNodes.push_back( *iter );
        }
    }

    // list depicting the final grid, contains a list for each row in the grid
    // rows.size() == number of rows in the grid, when algorithm is done
    std::list< std::list< WNetworkLayoutNode * > > rows;
    rows.push_back( rootNodes );
    bool done = false;
    while( !done )
    {
        std::list< WNetworkLayoutNode * > newRow;
        for( std::list< WNetworkLayoutNode * >::iterator iter = rows.back().begin();
                iter != rows.back().end(); ++iter )
        {
            std::list< WNetworkLayoutNode * > children = ( *iter )->getChildren();
            for( std::list< WNetworkLayoutNode * >::iterator childIter = children.begin();
                    childIter != children.end(); ++childIter )
            {
                std::list< WNetworkLayoutNode * >::iterator findIter =
                    find( rows.back().begin(), rows.back().end(), *childIter );
                if( rows.back().end() != findIter )
                {
                    // the node is already in the layout
                    findIter = rows.back().erase( findIter );
                    rows.back().insert( findIter, &dummyNode );
                    // remove node
                    // add dummy
                    // add to this row
                }
                newRow.push_back( *childIter );
                // TODO: problem if child is in the list before the last
            }

        }
        if( newRow.size() == 0 /* TEST */ )
        {
            done = true;
        }
        else
        {
            rows.push_back( newRow );
        }
    }
    // get children-list of each node
    // get children-children-list ...
    //      add dummy nodes
    // stop when all nodes are done
    // additional work ?

    // set position of each node
    unsigned int x = 0;
    for( std::list< std::list< WNetworkLayoutNode * > >::iterator rowsIter = rows.begin();
            rowsIter != rows.end(); ++rowsIter )
    {
        unsigned int y = 0;
        for( std::list< WNetworkLayoutNode * >::iterator iter = rowsIter->begin(); iter != rowsIter->end(); ++iter )
        {
        std::cout << "x: " <<  x;
        std::cout << " y: " << y << std::endl;
            QPoint pos( x, y );
            std::cout << pos.x() << " " << pos.y() << std::endl;
            (*iter)->setGridPos( pos );
            ++y;
        }

        ++x;
    }
}

/*void WNetworkLayout::updateGrid( unsigned int fixedGraph, unsigned int x, bool shiftRight, int distance )
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
}*/

//void WNetworkLayout::merge( WQtNetworkItem *parent, WQtNetworkItem *child )
//{
//    if( parent->getLayoutItem()->getGridPos().y() + 1 > child->getLayoutItem()->getGridPos().y() )
//    {
//        WQtNetworkItem* help = parent;
//        parent = child;
//        child = help;
//    }
/*    float offsetY = ( parent->getLayoutItem()->getGridPos().y() + 1 ) - child->getLayoutItem()->getGridPos().y();
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
    }*/


    //
    // ------------------------------- REDONE ---------------------------------------------------
    //
/*
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
*/
    // change position of the two graphs that are going to be merged
//    if( first->getLayoutItem()->getGridPos().y() > child->getLayoutItem()->getGridPos().y() ) //firstValues.second > secondValues.second )
//    {
/*        if( parent->getLayoutItem()->getGridPos().x() < child->getLayoutItem()->getGridPos().x() )
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
        if( parent->getLayoutItem()->getGridPos().x() / *+ 1* / != child->getLayoutItem()->getGridPos().x() )
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
}*/

