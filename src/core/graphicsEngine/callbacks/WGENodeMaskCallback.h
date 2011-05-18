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

#ifndef WGENODEMASKCALLBACK_H
#define WGENODEMASKCALLBACK_H

#include <boost/signals2.hpp>

#include <osg/Camera>
#include <osg/Node>

#include "../../common/WFlag.h"

#include "../WExportWGE.h"

/**
 * This callback is useful to en-/disable nodes using the node mask based on properties. In contrast to WGEManagedGroupNode, this callback can be
 * added to every kind of node.
 * DO NOT use one instance of this class for multiple nodes. The problem is not very obvious. If a node gets deactivated, its node callback is
 * inactive too. To reactive the node again, the callback somehow needs to remember which node it has deactivated. This is done during the first
 * deactivation in update traversal.
 */
class WGE_EXPORT WGENodeMaskCallback: public osg::NodeCallback
{
public:
    /**
     * Creates new instance.
     *
     * \param flag the bool property which controls activation.
     */
    explicit WGENodeMaskCallback( boost::shared_ptr< WBoolFlag > flag );

    /**
     * Destructor.
     */
    virtual ~WGENodeMaskCallback();

     /**
     * This operator gets called by OSG every update cycle.
     *
     * \param node the osg node
     * \param nv the node visitor
     */
    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
private:
    /**
     * The flag controlling the node mask
     */
     boost::shared_ptr< WBoolFlag > m_flag;

    /**
     * The subscription to the change signal of m_flag.
     */
    boost::signals2::connection m_connection;

    /**
     * This connection gets established during the deactivation in operator() to ensure re-activation.
     */
    boost::signals2::connection m_reactivateConnection;

    /**
     * The type of signal used to reactivate the signal.
     */
    typedef boost::signals2::signal< void() >  ReactivateSignal;

    /**
     * The reactivation signal.
     */
    ReactivateSignal m_reactivateSignal;

    /**
     * Gets called if m_flag changes. This handles activation of the node.
     */
    virtual void activate();
};

#endif  // WGENODEMASKCALLBACK_H

