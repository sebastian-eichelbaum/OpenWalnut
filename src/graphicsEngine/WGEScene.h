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

#ifndef WGESCENE_H
#define WGESCENE_H

#include <set>

#include <boost/thread.hpp>

#include <osg/Group>
#include <osg/NodeCallback>

/**
 * Class for managing the OpenSceneGraph root node. It can handle new nodes, removing nodes and so on.
 * \ingroup ge
 */
class WGEScene: public osg::Group
{
public:

    /**
     * Default constructor.
     */
    WGEScene();

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
     * Removes the specified node from this group in a thread safe manner. See insert() for more details.
     *
     * \param node the node to remove
     */
    void remove( osg::ref_ptr< osg::Node > node );

protected:


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
     * Destructor.
     */
    virtual ~WGEScene();

    /**
    * Node callback used to update this root node.
    */
    osg::ref_ptr< SafeUpdaterCallback > m_nodeUpdater;

    /**
    * Queue of childs that need to be added during the next update cycle.
    */
    std::set< osg::ref_ptr< osg::Node > > m_childInsertionQueue;

    /**
    * Queue of childs that need to be removed during the next update cycle.
    */
    std::set< osg::ref_ptr< osg::Node > > m_childRemovalQueue;

    /**
    * Lock used for inserting childs into the child insertion queue.
    */
    boost::shared_mutex m_childInsertionQueueLock;

    /**
     * Lock used for inserting childs into the child removal queue.
     */
    boost::shared_mutex m_childRemovalQueueLock;

    /**
     * Flag denoting whether the m_childInsertionQueue should be considered during the next update of the node.
     */
    bool m_insertionQueueDirty;

    /**
     * Flag denoting whether the m_childRemovalQueue should be considered during the next update of the node.
     */
    bool m_removalQueueDirty;

private:
};

#endif  // WGESCENE_H

