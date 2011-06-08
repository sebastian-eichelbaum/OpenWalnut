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

#ifndef WNETWORKLAYOUTITEM_H
#define WNETWORKLAYOUTITEM_H

#include <list>

#include "../WQtNetworkItem.h"

class WQtNetworkItem;

/**
 * TODO
 **/
class WNetworkLayoutItem
{
    public:
        /**
         * constructor
         *
         * \param item the corresponding network item
         **/
        explicit WNetworkLayoutItem( WQtNetworkItem *item );

        /**
         * destructor
         **/
        ~WNetworkLayoutItem();

//        /**
//         * adds a new child to this item
//         *
//         *
//         **/
//        void addChild( WNetworkLayoutItem *child, float width );
//
//        /**
//         * disconnects the item form the current parent and moves the network item to a new 'lane'
//         *
//         * \param newPos the position for the new 'lane'
//         **/
//        void disconnectFromParent( QPointF newPos );
//
//        /**
//         * to access the parent item
//         *
//         * \return the parent of this item, NULL if there is no parent (items with in-degree == 0 )
//         **/
//        WNetworkLayoutItem * getParent();
//
//        /**
//         * to access the right neighbour
//         *
//         * \return the right neighbour of this item, NULL if there is no right neighbour ( i.e. item is the rightmost item )
//         **/
//        WNetworkLayoutItem * getNeighbour();
//
//        /**
//         * returns the position for a new child
//         **/
//        QPointF getNewChildPos();
//
//        /**
//         * to access the position of the item
//         **/
//        QPointF getPos();
//
//        /**
//         * to access the width of the item
//         **/
//        float getWidth();
//
//        /**
//         * sets the parent of this item, updates the parents width(can cause an update for the whole
//         * layout)
//         *
//         *
//         **/
//        void setParent( WNetworkLayoutItem *parent );
//
//        /**
//         * to change the position of the item
//         **/
//        void setPos( QPointF pos );
//
//        /**
//         * set the neighbour of this item. That parent equals NULL is expected for other parts of
//         * the layout, but is not tested.
//         *
//         * /param neighbour the new neighbour to the right of this item
//         **/
//        void setNeighbour( WNetworkLayoutItem *neighbour );
//
//        /**
//         * removes the child, done when changing the layout graph
//         *
//         * \param child the item which is removed
//         **/
//        void removeChild( WNetworkLayoutItem *child );
//
//        /**
//         * called when a child changes its width
//         **/
//        void updateWidth();

        // DEVELOPMENT
        /**
         * Sets the position within the grid, updates the position of the corresponding
         * WQtNetworkItem
         **/
        void setGridPos( QPointF pos ); // m_matrixPos

        /**
         * returns the position within the grid
         **/
        QPointF getGridPos();


    protected:

    private:
//        /**
//         * updates the position of the item in the scene
//         **/
//        void updateLayoutPos();
//
//        QPointF m_pos; //<! below this position in the layout TODO
//
//        float m_width; //<! space occupied be the item and its childreen
//
//
//        WNetworkLayoutItem *m_parent; //<! the item thats above this one, item that is connected to this items in-port
//
//        WNetworkLayoutItem *m_neighbour; //<! neighbour to the right of this item, only the source

        WNetworkLayoutItem *m_source; //<! the source of this subgraph, the initial element

        WQtNetworkItem *m_item; //<! the corresponding NetworkItem in the scene, which is manipulated by this LayoutItem

        std::list< WNetworkLayoutItem * > m_children; //<! the children of this item;

        std::list< WNetworkLayoutItem * > m_parents; //<! the parents of this item;

        QPointF m_gridPos; //<! abstract pos, within the matrix
};

#endif  // WNETWORKLAYOUTITEM_H

