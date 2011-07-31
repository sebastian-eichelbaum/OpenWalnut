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

#include <algorithm>

#include "WNetworkLayoutGraph.h"

WNetworkLayoutGraph::WNetworkLayoutGraph( WQtNetworkItem *item )
{
    m_root = new WNetworkLayoutNode();
    if( item )
    {
        //add( item );
        m_root->add( new WNetworkLayoutNode( item ) );
    }
}

WNetworkLayoutGraph::~WNetworkLayoutGraph()
{
    delete m_root;
}

void WNetworkLayoutGraph::add( WQtNetworkItem *item )
{
    // TODO: add item at the right position
    // include the item in the graph
}

//std::list< WNetworkLayoutItem * > * WNetworkLayoutSubgraph::data()
//{
//    return &m_nodes;
//}
//
//bool WNetworkLayoutSubgraph::find( WNetworkLayoutItem *item )
//{
//    return ( std::find( m_nodes.begin(), m_nodes.end(), item ) != m_nodes.end() );
//}
//
//unsigned char WNetworkLayoutSubgraph::getId()
//{
//    return m_id;
//}
//
//WNetworkLayoutItem * WNetworkLayoutSubgraph::getRightmostItem()
//{
//    return m_rightmostItem;
//}
//
//void WNetworkLayoutSubgraph::manipulate( int x, int y )
//{
//    for( std::list< WNetworkLayoutItem * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
//    {
//        QPointF pos = ( *iter )->getGridPos();
//        pos.setX( pos.x() + x );
//        pos.setY( pos.y() + y );
//        ( *iter )->setGridPos( pos );
//    }
//}
//
//
//void WNetworkLayoutSubgraph::merge( WNetworkLayoutSubgraph *subgraph )
//{
//    std::list< WNetworkLayoutItem * > *nodes = subgraph->data();
//    for( std::list< WNetworkLayoutItem * >::iterator iter = nodes->begin(); iter != nodes->end(); ++iter )
//    {
//        m_nodes.push_back( *iter );
//    }
//}


void WNetworkLayoutGraph::remove( WNetworkLayoutNode *item )
{
   /* m_nodes.remove( item );
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
    }*/
}

/*unsigned int WNetworkLayoutGraph::size()
{
    return m_nodes.size();
}*/

void WNetworkLayoutGraph::traverse()
{
    // list depicting the final grid, contains a list for each row in the grid
    // rows.size() == number of rows in the grid, when algorithm is done
    std::list< std::list< WNetworkLayoutNode * > > rows;
    rows.push_back( m_root->getChildren() );
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
                    rows.back().insert( findIter, new WNetworkLayoutNode ); // TODO
                    rows.back().erase( findIter );
                    rows.back().insert( findIter, new WNetworkLayoutNode ); // TODO
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
            (*iter)->setGridPos( QPoint( x, y ) ); // TODO QPointF => QPoint
            ++y;
        }
        ++x;
    }
}

//void WNetworkLayoutSubgraph::swap( const QPointF &first, const QPointF &second )
//{
//    if( first == second )
//    {
//        return;
//    }
//
//    // TODO redo this whole section, problem if: no node is at the position, no 'out of range' check
//    WNetworkLayoutItem *firstItem = NULL, *secondItem = NULL;
//    for( std::list< WNetworkLayoutItem * >::iterator iter = m_nodes.begin(); iter != m_nodes.end(); ++iter )
//    {
//        if( ( *iter )->getGridPos() == first )
//        {
//            firstItem = *iter;
//            if( secondItem != NULL )
//            {
//                break;
//            }
//        }
//
//        if( ( *iter )->getGridPos() == second )
//        {
//            secondItem = *iter;
//            if( firstItem != NULL )
//            {
//                break;
//            }
//        }
//    }
//    if( firstItem != NULL )
//    {
//        firstItem->setGridPos( second );
//    }
//    if( secondItem != NULL )
//    {
//        secondItem->setGridPos( first );
//    }
//}


