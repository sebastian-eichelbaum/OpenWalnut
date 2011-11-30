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

#include <iostream>

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>

#include "WGEGroupNode.h"

WGEGroupNode::WGEGroupNode():
    osg::MatrixTransform(),
    m_childOperationQueueDirty( false ),
    m_removeAll( false )
{
    setDataVariance( osg::Object::DYNAMIC );

    // setup an update callback
    m_nodeUpdater = osg::ref_ptr< SafeUpdaterCallback >( new SafeUpdaterCallback() );
    addUpdateCallback( m_nodeUpdater );

    osg::Matrix m;
    m.makeIdentity();
    setMatrix( m );
}

WGEGroupNode::~WGEGroupNode()
{
    // cleanup
}

void WGEGroupNode::insert( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( boost::shared_ptr< ChildOperation >( new ChildOperation( INSERT, node ) ) );
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::remove( osg::ref_ptr< osg::Node > node )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( boost::shared_ptr< ChildOperation >( new ChildOperation( REMOVE, node ) ) );
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::remove_if( boost::shared_ptr< WGEGroupNode::NodePredicate > predicate )
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( boost::shared_ptr< ChildOperation >( new ChildOperation( REMOVE_IF, predicate ) ) );
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::clear()
{
    boost::unique_lock<boost::shared_mutex> lock = boost::unique_lock<boost::shared_mutex>( m_childOperationQueueLock );
    m_childOperationQueue.push( boost::shared_ptr< ChildOperation >( new ChildOperation( CLEAR, osg::ref_ptr< osg::Node >() ) ) );
    // this encodes the remove all feature
    m_childOperationQueueDirty = true;
    lock.unlock();
}

void WGEGroupNode::SafeUpdaterCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // the node also is a WGEGroupNode
    WGEGroupNode* rootNode = static_cast< WGEGroupNode* >( node );

    // write lock the insertion list
    boost::unique_lock<boost::shared_mutex> lock;

    // write lock the removal list
    if( rootNode->m_childOperationQueueDirty )
    {
        lock = boost::unique_lock<boost::shared_mutex>( rootNode->m_childOperationQueueLock );
        // insert/remove children which requested it
        while( !rootNode->m_childOperationQueue.empty() )
        {
            // remove or insert or remove all?
            if( rootNode->m_childOperationQueue.front()->m_operation == INSERT )
            {
                // add specified child
                rootNode->addChild( rootNode->m_childOperationQueue.front()->m_item );
            }

            if( rootNode->m_childOperationQueue.front()->m_operation == REMOVE )
            {
                // remove specified child
                rootNode->removeChild( rootNode->m_childOperationQueue.front()->m_item );
            }

            if( rootNode->m_childOperationQueue.front()->m_operation == REMOVE_IF )
            {
                // remove children where m_predicate is true
                for( size_t i = 0; i < rootNode->getNumChildren(); )
                {
                    if( ( *rootNode->m_childOperationQueue.front()->m_predicate )( rootNode->getChild( i ) ) )
                    {
                        // remove item but do not increment index
                        rootNode->removeChild( i );
                    }

                    // this was not removed. Go to next one.
                    ++i;
                }
            }

            if( rootNode->m_childOperationQueue.front()->m_operation == CLEAR )
            {
                // remove all
                rootNode->removeChild( 0, rootNode->getNumChildren() );
            }

            // pop item
            rootNode->m_childOperationQueue.pop();
        }

        rootNode->dirtyBound();

        // all children added/removed -> clear
        rootNode->m_childOperationQueueDirty = false;
        rootNode->m_removeAll = false;

        lock.unlock();
    }

    // forward the call
    traverse( node, nv );
}

