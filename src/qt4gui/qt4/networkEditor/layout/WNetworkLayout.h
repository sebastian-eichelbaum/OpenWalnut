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
 * TODO
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
         * add an item to the layout, creates new network layout item
         *
         * /param item the new network item
         **/
        void addItem( WQtNetworkItem *item );

        /**
         * connect the two items, moves the 'end' item to the lane of 'start'
         **/
        void connectItems( WQtNetworkItem *start, WQtNetworkItem *end );

        /**
         * disconnects the child from the parent item and moves the item to a new, empty lane
         **/
        void disconnectNodes( WQtNetworkItem *parent, WQtNetworkItem *child );

        /**
         * remove an item from the layout, should only be called when in- and out-degree of the item
         * equals zero, if this is not the case disconnect items beforehand
         *
         * \param item the item thats going to be removed
         **/
        void removeItem( WQtNetworkItem *item );

    protected:

    private:
        /**
         * This function traverses the layout graph and creates the new layout through assigning
         * positions the individual nodes.
         **/
        void traverse();

        std::list< WNetworkLayoutNode * > m_nodes; //<! all nodes within the layout
};

#endif  // WNETWORKLAYOUT_H

