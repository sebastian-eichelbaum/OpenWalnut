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

#ifndef WGEMANAGEDGROUPNODE_H
#define WGEMANAGEDGROUPNODE_H

#include <boost/shared_ptr.hpp>

#include <osg/NodeCallback>

#include "WGEGroupNode.h"
#include "../common/WCondition.h"
#include "../common/WFlag.h"
#include "WExportWGE.h"

/**
 * This class adds some convenience methods to WGEGroupNode. It can handle bool properties which switch the node on and off.
 *
 * \ingroup GE
 */
class WGE_EXPORT WGEManagedGroupNode: public WGEGroupNode
{
public:

    /**
     * Default constructor.
     *
     * \param active the flag denoting the node to be activated ( if flag == true ) or turned off.
     */
    explicit WGEManagedGroupNode( boost::shared_ptr< WBoolFlag > active );

    /**
     * Destructor.
     */
    virtual ~WGEManagedGroupNode();

protected:

    /**
     * Update callback which sets the node mask of the node according to the specified property.
     */
    class NodeMaskCallback : public osg::NodeCallback
    {
    public:
        /**
         * Constructor.
         *
         * \param flag the flag denoting whether the node should be active or not
         */
        explicit NodeMaskCallback( boost::shared_ptr< WBoolFlag > flag ): m_activeFlag( flag )
        {
        };

        /**
         * Callback method called by the NodeVisitor when visiting a node.
         * Sets the nodemask of the node according to the specified property.
         *
         * \param node the node calling this update
         * \param nv The node visitor which performs the traversal. Should be an
         * update visitor.
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * Flag denoting whether the node is turned on or off. We store the copy here to avoid permanent cast to WGEManagedGroupNode.
         */
        boost::shared_ptr< WBoolFlag > m_activeFlag;
    };

    /**
     * Gets called if m_activeFlag changes. This handles activation of the node.
     */
    virtual void activate();

    /**
     * Flag denoting whether the node is turned on or off
     */
    boost::shared_ptr< WBoolFlag > m_activeFlag;

    /**
     * The subscription to the change signal of m_activeFlag.
     */
    boost::signals2::connection m_connection;

private:
};

#endif  // WGEMANAGEDGROUPNODE_H

