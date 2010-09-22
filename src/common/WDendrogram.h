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

#ifndef WDENDROGRAM_H
#define WDENDROGRAM_H

#include <vector>

/**
 * Hirachical binary tree datastructure with spatial layout information. Since
 * join points in a dendrogram do normally not intersect a special index is
 * needed. Additionally a parameter for each node and leaf, its height, is
 * available.
 *
 * Each leaf and inner node will obtain a new index called TreeIndex, so the
 * hirachy will not overlap or intersect. As a leaf already has an index we
 * call this index the DataIndex. So a leaf with DataIndex 5 is the fifth data
 * element in the dataset of which this dendrogram is used for. Where as a leaf
 * with TreeIndex 5 is the fifth leaf from left.
 */
class WDendrogram
{
friend class WDendrogramTest;
public:
    /**
     * Creates a new Dendrogram with unjoined elements aka leafs.
     *
     * \param numElements The number of unjoined or initial elements
     *
     * TODO(math): Unsigned int is used to save memory here, but can't we use
     * size_t (8bytes) => ask christian..
     */
    explicit WDendrogram( unsigned int numElements );

    /**
     * Destructs a Dendrogram.
     */
    virtual ~WDendrogram();

protected:
    /**
     * Representing a node inside of the Dendrogram.
     *
     * \note Be aware of the different kinds of indices used in here. See
     * Description of the WDendrogram class for more details on TreeIndices and
     * DataIndices.
     */
    struct Node
    {
        /**
         * Default constructor for a new node. So all members are an valid
         * initial value: zero.
         */
        Node();

        /**
         * The TreeIndex of the parent it belongs to.
         */
        unsigned int parentTreeIdx;

        /**
         * All leafs grouped by this node have an bigger or equal TreeIndex
         * then this \e minTreeIdx. In other words: The leftmost leaf of the
         * subtree with this node as root has this TreeIndex.
         */
        unsigned int minTreeIdx;

        /**
         * All leafs grouped by this node have an lower or equal TreeIndex then
         * this \e maxTreeIdx. In other words: The rightmost leaf of the
         * subtree with this node as root has this TreeIndex.
         */
        unsigned int maxTreeIdx;

        /**
         * This is used to have a reference to the data element this node
         * represents. For inner nodes clearly none exists, but for leafs this
         * is the number inside of the dataset.
         */
        unsigned int dataIdx;

        /**
         * The height of each node. Leafs have an default height of zero.
         */
        double height;
    };
private:
    /**
     * Save the whole dendrogram and keep track of the link from leafs to the
     * dataset, as well as information of parents and leafs for each node.
     */
    std::vector< Node > m_tree;
};

#endif  // WDENDROGRAM_H
