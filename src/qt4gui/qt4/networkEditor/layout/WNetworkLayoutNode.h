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
         * Add a new child to this node.
         *
         * \param node the new child-node
         **/
        void add( WNetworkLayoutNode *node );

        /**
         * Set the parent to this node.
         *
         * \param node the parent node
         **/
        void addParent( WNetworkLayoutNode *node );

        /**
         * Returns all children of this node.
         *
         * \return a list of all child-nodes
         **/
        std::list< WNetworkLayoutNode* > getChildren();

        /**
         * The number of children this item has.
         *
         * \return the number of children
         **/
        int nChildren();

        /**
         * The number of parents this item has.
         *
         * \return the parents of children
         **/
        int nParents();

        /**
         * Removes one of the children if it is a child of this node.
         *
         * \param node the child-node which is to be removed
         **/
        void remove( WNetworkLayoutNode *node );

        /**
         * Sets the position within the grid, updates the position of the corresponding
         * WQtNetworkItem.
         *
         * \param pos the (new) position within the layout
         **/
        void setGridPos( QPoint pos );

        /**
         * Returns the current position within the grid.
         *
         * \return the position within the grid
         **/
        QPointF getGridPos();

    protected:

    private:
        std::list< WNetworkLayoutNode * > m_children; //!< the nodes this node links to ie. children of this node

        std::list< WNetworkLayoutNode * > m_parents; //!< the nodes that link to this node ie. parents of this node

        WQtNetworkItem *m_referencedItem; //!< the corresponding item in the QGraphicsScene ie. the visual representation

        QPoint m_gridPos; //!< abstract pos, the position within the matrix
};

#endif  // WNETWORKLAYOUTNODE_H

