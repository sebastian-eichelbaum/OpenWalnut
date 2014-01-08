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

#ifndef WTREENODE_H
#define WTREENODE_H

#include <sstream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../common/datastructures/WDendrogram.h"

/**
 * A node in a tree, holding an index, a level in the tree and pointers to its child nodes
 */
class WTreeNode : public boost::enable_shared_from_this<WTreeNode>
{
public:
    /**
     * Shared pointer abbreviation.
     */
    typedef boost::shared_ptr< WTreeNode > SPtr;

    /**
     * Constructs a new TreeNode.
     *
     * \param index the index of the new Node.
     * \param level the level of the Node in the Tree
     */
    WTreeNode( size_t index, double level );

    /**
     * Constructs a tree of WTreeNodes from a WDendrogram with this WTreeNode as root
     *
     * \param dendrogram Reference to the dendrogram to construct the tree from
     */
    WTreeNode( const WDendrogram &dendrogram );

    /**
     * Default destructor.
     */
    ~WTreeNode();

    /**
     * Adds a childnode to this node
     *
     * \param child the child node to add
     */
    void addChild( WTreeNode::SPtr child );

    /**
     * Returns the index of the TreeNode
     *
     * \return the node's index
     */
    size_t index();

    /**
     * Returns the level of the TreeNode. All level-0-nodes are leaves.
     *
     * \return the node's level
     */
    double level();

    /**
     * Returns the child nodes of this node
     *
     * \return the child nodes of this node
     */
    std::vector< WTreeNode::SPtr > getChildren();

    /**
     * Returns the parent node of this node
     *
     * \return the parent node of this node
     */
    WTreeNode::SPtr getParent();

private:
    /**
     * Stores the childnodes of this node
     */
    std::vector< WTreeNode::SPtr > m_children;

    /**
     * Stores the level of this node
     */
    double m_level;

    /**
     * Stores the index of this node
     */
    size_t m_index;

    /**
     * Stores the parent node
     */
    WTreeNode::SPtr m_parent;
};

#endif  // WTREENODE_H
