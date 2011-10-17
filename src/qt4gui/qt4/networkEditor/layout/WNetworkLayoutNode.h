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

#ifndef WNETWORKLAYOUTNODE_H
#define WNETWORKLAYOUTNODE_H

#include <list>

#include "../WQtNetworkItem.h"

class WQtNetworkItem;

/**
 * TODO
 **/
class WNetworkLayoutNode
{
    public:
        /**
         * constructor
         * without item dummy node
         *
         * \param item the corresponding network item
         **/
        explicit WNetworkLayoutNode( WQtNetworkItem *item = NULL );

        /**
         * destructor
         **/
        ~WNetworkLayoutNode();

        /**
         * add a new child to this node
         **/
        void add( WNetworkLayoutNode *node );

        /**
         *
         **/
        void addParent( WNetworkLayoutNode *node );

        /**
         * return all children
         **/
        std::list< WNetworkLayoutNode* > getChildren();

        int nChildren();

        int nParents();

        /**
         * removes one of the children if they belong to this node
         **/
        void remove( WNetworkLayoutNode *node );

        // DEVELOPMENT
        /**
         * Sets the position within the grid, updates the position of the corresponding
         * WQtNetworkItem
         **/
        void setGridPos( QPoint pos ); // m_matrixPos

        /**
         * returns the position within the grid
         **/
        QPointF getGridPos();

    protected:

    private:
        std::list< WNetworkLayoutNode * > m_children; //<! the nodes this node links to

        std::list< WNetworkLayoutNode * > m_parents; //<! nodes that link to this node

        WQtNetworkItem *m_referencedItem; //<! the corresponding item in the QGraphicsScene ie. the visual representation

        QPoint m_gridPos; //<! abstract pos, within the matrix
};

#endif  // WNETWORKLAYOUTNODE_H

