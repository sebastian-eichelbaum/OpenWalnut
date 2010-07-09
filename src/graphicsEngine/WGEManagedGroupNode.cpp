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

#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Geode>

#include "WGEManagedGroupNode.h"

WGEManagedGroupNode::WGEManagedGroupNode( boost::shared_ptr< WBoolFlag > active ):
    WGEGroupNode(),
    m_activeFlag( active )
{
    setDataVariance( osg::Object::DYNAMIC );

    // get change signal
    m_connection = m_activeFlag->getValueChangeCondition()->subscribeSignal(
            boost::bind( &WGEManagedGroupNode::activate, this )
    );

    // setup an update callback
    addUpdateCallback( osg::ref_ptr< NodeMaskCallback >( new NodeMaskCallback( m_activeFlag ) ) );
}

WGEManagedGroupNode::~WGEManagedGroupNode()
{
    // cleanup
    m_connection.disconnect();
}

void WGEManagedGroupNode::activate()
{
    if ( m_activeFlag->get() ) // only handle activation here
    {
        setNodeMask( 0xFFFFFFFF );
    }
}

void WGEManagedGroupNode::NodeMaskCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // Deactivate the node
    if ( !m_activeFlag->get() ) // only handle deactivation here
    {
        node->setNodeMask( 0x0 );

        // NOTE: this also deactivates the callback. So reactivating the node is done in a separate method
    }

    // forward the call
    traverse( node, nv );
}

