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

#ifndef WGEGROUPNODE_H
#define WGEGROUPNODE_H

#include <queue>
//#include <utility>

#include <boost/thread.hpp>

#include <osg/MatrixTransform>
#include <osg/NodeCallback>

//#include "../common/WCondition.h"
#include "../common/WPredicateHelper.h"
#include "WExportWGE.h"

/**
 * Class to wrap around the osg Group node and providing a thread safe add/removal mechanism. Please be sure to use
 * addUpdateCallback() to set your own update callbacks instead of setUpdateCallback, as this class already has set a callback,
 * which would be overwritten by a subsequent call to setUpdateCallback(). It is derived from osg::MatrixTransform to allow easy transformations
 * of a whole bunch of nodes.
 *
 * \ingroup GE
 */
class WGE_EXPORT WGEGroupNode: public osg::MatrixTransform
{
public:

    /**
     * Default constructor.
     */
    WGEGroupNode();

    /**
     * Adds the specified node to the child list of this node in a safe manner. OSG officially requires nodes to be added
     * exclusively during update callbacks. Using this method it is ensured to be added during update cycle.
     *
     * \param node the node to add.
     *
     * \note the node may not be added instantly. So do not assume that containsNode ( node ) will return true.
     */
    void insert( osg::ref_ptr< osg::Node > node );

    /**
     * Removes the specified node from this group in a thread safe manner. It returns if the node has been removed.
     *
     * \param node the node to remove
     */
    void remove( osg::ref_ptr< osg::Node > node );

    /**
     * The base type of predicate. Use a specific WPredicateHelper::ArbitraryPredicate instance. For details, see
     * \ref WPredicateHelper::ArbitraryPredicateBase.
     */
    typedef WPredicateHelper::ArbitraryPredicateBase< osg::ref_ptr< osg::Node > const > NodePredicate;

    /**
     * Removes a node if the specified predicate evaluates to true.
     * \see WPredicateHelper::ArbitraryPredicate for details.
     *
     * \param predicate the predicate.
     */
    void remove_if( boost::shared_ptr< WGEGroupNode::NodePredicate > predicate );

    /**
     * Removes all children from this node.
     */
    void clear();

protected:
    /**
     * Destructor.
     */
    virtual ~WGEGroupNode();

    /**
     * Update callback which inserts and removes nodes from m_childRemovalQueue and m_childInsertionQueue to the group node.
     * This ensures thread safe modification of the osg root node.
     */
    class SafeUpdaterCallback : public osg::NodeCallback
    {
    public:

        /**
         * Callback method called by the NodeVisitor when visiting a node.
         * This inserts and removes enqueued nodes from this group node instance.
         *
         * \param node the node calling this update
         * \param nv The node visitor which performs the traversal. Should be an
         * update visitor.
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );
    };

    /**
     * Node callback used to update this root node.
     */
    osg::ref_ptr< SafeUpdaterCallback > m_nodeUpdater;

    /**
     * The type of operation to perform.
     */
    typedef enum
    {
        INSERT = 0,         //! insert the specified node
        REMOVE,             //! remove the specified node
        REMOVE_IF,          //! remove all items where the predicate evaluates to true
        CLEAR               //! clear group node completely
    }
    ChildOperationType;

    /**
     * A struct denoting an operation on this group. The operation itself, as well as the item and predicate are stored.
     */
    struct ChildOperation
    {
        /**
         * Constructs instance and fills members properly.
         *
         * \param what the operation to make
         * \param item the child to delete
         */
        ChildOperation( ChildOperationType what, osg::ref_ptr< osg::Node > item ):
            m_operation( what ),
            m_item( item ),
            m_predicate()
        {
        };

        /**
         * Constructs instance and fills members properly.
         *
         * \param what the operation to make
         * \param predicate the predicate to use for conditional operations (REMOVE_IF)
         */
        ChildOperation( ChildOperationType what, boost::shared_ptr< NodePredicate > predicate ):
            m_operation( what ),
            m_item(),
            m_predicate( predicate )
        {
        };

        ChildOperationType m_operation;                     //!< the operation to take
        osg::ref_ptr< osg::Node > m_item;                   //!< the item to delete/add
        boost::shared_ptr< NodePredicate > m_predicate;     //!< the predicate used by conditional operations
    };

    /**
     * Queue of childs that need to be added/removed during the next update cycle. It is a pair per operation, where the bool is denoting removal
     * or insertion.
     */
    std::queue< boost::shared_ptr< ChildOperation > > m_childOperationQueue;

    /**
     * Lock used for inserting and removing childs into the child insertion/removal queue.
     */
    boost::shared_mutex m_childOperationQueueLock;

    /**
     * Flag denoting whether the m_childOperationQueue should be considered during the next update of the node.
     */
    bool m_childOperationQueueDirty;

    /**
     * True whenever all child nodes should be removed.
     */
    bool m_removeAll;

private:
};

#endif  // WGEGROUPNODE_H

