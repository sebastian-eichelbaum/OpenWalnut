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

#ifndef WNETWORKLAYOUTSUBGRAPH_H
#define WNETWORKLAYOUTSUBGRAPH_H

#include <list>

#include "WNetworkLayoutItem.h"

/**
 * TODO
 **/
class WNetworkLayoutSubgraph
{
    public:
        /**
         * constructor
         **/
        explicit WNetworkLayoutSubgraph( unsigned char id, WNetworkLayoutItem *item = NULL );

        /**
         * destructor
         **/
        ~WNetworkLayoutSubgraph();

        /**
         * add a node to this subgraph, the node needs its grid position set befor adding it
         **/
        void add( WNetworkLayoutItem *item );

        /**
         * allows access to the list of layout items TODO
         **/
        std::list< WNetworkLayoutItem * > * data();

        /**
         * is a item in this subgraph ?
         * returns true if item is in this subgraph
         **/
        bool find( WNetworkLayoutItem *item );

        /**
         * Gives access to the id of the graph.
         *
         * \return id of the subgraph
         **/
        unsigned char getId();

        /**
         * returns the rightmost item in this subgraph, NULL if graph is empty
         **/
        WNetworkLayoutItem * getRightmostItem();

        /**
         * manipulates the position of the subgraph
         **/
        void manipulate( int x, int y = 0 );

        /**
         * merge the subgraph into this one TODO
         * keep position,
         **/
        void merge( WNetworkLayoutSubgraph *subgraph );

        /**
         * remove a node from the subgraph
         **/
        void remove( WNetworkLayoutItem *item );

        /**
         * returns the size of the subgraph
         **/
        unsigned int size();

        /**
         * swap the two items TODO
         **/
        void swap( const QPointF &first, const QPointF &second );

    protected:

    private:
        unsigned char m_id; //<! identifies the subgraph

        std::list< WNetworkLayoutItem * > m_nodes; //<! every node in this subgraph, not ordered

        WNetworkLayoutItem *m_rightmostItem; //<! the right most item of the subgraph
};

#endif  // WNETWORKLAYOUTSUBGRAPH_H

