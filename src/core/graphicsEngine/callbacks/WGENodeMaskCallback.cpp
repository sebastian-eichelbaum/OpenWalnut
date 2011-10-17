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

#include "WGENodeMaskCallback.h"

WGENodeMaskCallback::WGENodeMaskCallback( boost::shared_ptr< WBoolFlag > flag ):
    osg::NodeCallback(),
    m_flag( flag )
{
    // get change signal
    m_connection = m_flag->getValueChangeCondition()->subscribeSignal(
            boost::bind( &WGENodeMaskCallback::activate, this )
    );
}

WGENodeMaskCallback::~WGENodeMaskCallback()
{
    // cleanup
    m_connection.disconnect();
}

void WGENodeMaskCallback::activate()
{
    if( m_flag->get() ) // only handle activation here
    {
        m_reactivateSignal();
    }
}

void WGENodeMaskCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // Deactivate the node
    if( !m_flag->get() ) // only handle deactivation here
    {
        node->setNodeMask( 0x0 );

        // NOTE: this also deactivates the callback. So reactivating the node is done in a separate method
        // connect the flag's callback to the signal, only this way, we can ensure the reactivation of a node without saving a pointer to it.
        m_reactivateConnection = m_reactivateSignal.connect( boost::bind( &osg::Node::setNodeMask, node, 0xFFFFFFFF ) );
    }

    traverse( node, nv );
}

