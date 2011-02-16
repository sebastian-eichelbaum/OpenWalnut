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

#ifndef WGEMANAGEDRENDERNODECL_H
#define WGEMANAGEDRENDERNODECL_H

#include <boost/shared_ptr.hpp>

#include "../../common/WFlag.h"
#include "../WExportWGE.h"
#include "WGERenderNodeCL.h"

/**
 * WGEManagedRenderNodeCL extends WGERenderNodeCL by a flag to switch the node on and off.
 * This switch is independent of WGERenderNodeCL's activation state.
 *
 * \ingroup ge
 */
class WGE_EXPORT WGEManagedRenderNodeCL: public WGERenderNodeCL
{
public:

    /**
     * Constructor.
     *
     * \param active The flag denoting the node to be online (true) or offline (false).
     * \param deactivated Set whether the node should be set to deactivated state.
     */
    explicit WGEManagedRenderNodeCL( boost::shared_ptr< WBoolFlag > active, bool deactivated = false );

    /**
     * Copy construcor.
     *
     * \param node The node to copy.
     * \param copyop The optional OSG copy operator.
     */
    WGEManagedRenderNodeCL( const WGEManagedRenderNodeCL& node, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY );

    /**
     * Overrides osg::Object::cloneType().
     *
     * \return The cloned node.
     */
    virtual osg::Object* cloneType() const;

    /**
     * Overrides osg::Object::clone().
     *
     * \param copyop The optional OSG copy operator.
     *
     * \return The cloned node.
     */
    virtual osg::Object* clone( const osg::CopyOp& copyop ) const;

    /**
     * Overrides osg::Object::isSameKindAs().
     *
     * \param object The object to compare with.
     *
     * \return States whether this node and object are of same type.
     */
    virtual bool isSameKindAs( const osg::Object* object ) const;

    /**
     * Overrides osg::Object::className().
     *
     * \return Gives the node's class name.
     */
    virtual const char* className() const;

    /**
     * Overrides osg::Node::accept().
     *
     * \param nv The traversal's NodeVisitor.
     */
    virtual void accept( osg::NodeVisitor& nv );    // NOLINT - non const reference. Derived from OSG. We can't change signature.

protected:

    /**
     * Destructor.
     */
    virtual ~WGEManagedRenderNodeCL();

private:

    /**
     * The flag to control the node's state.
     */
    boost::shared_ptr< WBoolFlag > m_flag;
};

#endif  // WGEMANAGEDRENDERNODECL_H
