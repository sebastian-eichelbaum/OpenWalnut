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

#ifndef WNETWORKLAYOUT_H
#define WNETWORKLAYOUT_H

#include <list>

#include "WNetworkLayoutGlobals.h"
#include "WNetworkLayoutNode.h"

/**
 * This class handels the layout of the module graph: places the nodes and updates the position when
 * the graph is changed. The new layout is calculated in the function traverse().
 **/
class WNetworkLayout
{
    public:
        /**
         * constructor
         **/
        WNetworkLayout();

        /**
         * destructor
         **/
        ~WNetworkLayout();

        /**
         * Add an item to the layout and creates new network layout item. Also updates the
         * layout by calling traverse().
         *
         * \param item the new network item
         **/
        void addItem( WQtNetworkItem *item );

        /**
         * Connect the two items and update the layout.
         *
         * \param start The item thats at a 'higher' position in the layout, i.e. the parent of the end
         * item.
         * \param end the item at the lower position, i.e. the child.
         **/
        void connectItems( WQtNetworkItem *start, WQtNetworkItem *end );

        /**
         * Disconnect the child from the parent item and update the layout.
         *
         * \param parent the parent item
         * \param child the child item
         **/
        void disconnectNodes( WQtNetworkItem *parent, WQtNetworkItem *child );

        /**
         * Remove an item from the layout. Only has an effect if the item has no parents or
         * children.
         *
         * \param item the item thats going to be removed.
         * \return true if the item was removed, i.e. if the degree of the node was zero.
         **/
        bool removeItem( WQtNetworkItem *item );

    protected:

    private:
        /**
         * This function traverses the layout graph and creates the new layout through assigning
         * positions to the individual nodes in a grid layout.
         **/
        void traverse();


        std::list< WNetworkLayoutNode * > m_nodes; //!< all nodes within the layout
};

#endif  // WNETWORKLAYOUT_H

