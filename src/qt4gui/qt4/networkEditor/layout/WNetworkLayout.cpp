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

// private

void WNetworkLayout::traverse()
{
    std::list< WNetworkLayoutNode * > rootNodes, leafNodes;
    // dummy - used to create empty positions in the grid
    WNetworkLayoutNode dummyNode;
    for( std::list< WNetworkLayoutNode * >::iterator iter = m_nodes.begin(); iter != m_nodes.end();
            ++iter )
    {
        if( ( *iter )->nParents() == 0 )
        {
            rootNodes.push_back( *iter );
            if( ( *iter )->nChildren() > 1 )
            {
                for( int i = 0; i != ( *iter )->nChildren() -1; ++i )
                {
                    // add dummy node to create a better layout, just as a simple heuristic
                    rootNodes.push_back( ( &dummyNode ) );
                }
            }
        }
        if( ( *iter )->nChildren() == 0 )
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
        done = true;
        std::list< WNetworkLayoutNode * > newRow;
        for( std::list< WNetworkLayoutNode * >::iterator iter = rows.back().begin();
                iter != rows.back().end(); ++iter )
        {
            std::list< WNetworkLayoutNode * > children = ( *iter )->getChildren();

            if( children.size() == 0 )
            {
                newRow.push_back( &dummyNode );
            }

            for( std::list< WNetworkLayoutNode * >::iterator childIter = children.begin();
                    childIter != children.end(); ++childIter )
            {
                done = false;
                std::list< WNetworkLayoutNode * >::iterator findIter =
                    find( rows.back().begin(), rows.back().end(), *childIter );
                if( rows.back().end() != findIter )
                    // the node is already in the layout
                {
                    // remove node
                    findIter = rows.back().erase( findIter );
                    // add dummy in the parent row, dummy creates an 'empty' position in the grid
                    rows.back().insert( findIter, &dummyNode );
                }
                newRow.push_back( *childIter );
            }
        }
        rows.push_back( newRow );
    }

    // set position of each node
    unsigned int x = 0;
    for( std::list< std::list< WNetworkLayoutNode * > >::iterator rowsIter = rows.begin();
            rowsIter != rows.end(); ++rowsIter )
    {
        unsigned int y = 0;
        for( std::list< WNetworkLayoutNode * >::iterator iter = rowsIter->begin(); iter != rowsIter->end(); ++iter )
        {
            QPoint pos( x, y );
            ( *iter )->setGridPos( pos );
            ++y;
        }

        ++x;
    }
}

