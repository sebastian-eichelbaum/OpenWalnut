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

#include "callbacks/WGEViewportCallback.h"

#include "WGEOffscreenRenderNode.h"

WGEOffscreenRenderNode::WGEOffscreenRenderNode( osg::ref_ptr< osg::Camera > reference, size_t width, size_t height, bool noHud ):
    WGEGroupNode(),
    m_referenceCamera( reference ),
    m_hud( new WGETextureHud() ),
    m_textureWidth( width ),
    m_textureHeight( height ),
    m_nextPassNum( 0 )
{
    // initialize members
    if ( !noHud )
    {
        m_hud->addUpdateCallback( new WGEViewportCallback< WGETextureHud >( m_referenceCamera ) );
        m_hud->coupleViewportWithTextureViewport();
        insert( m_hud );
    }
}

WGEOffscreenRenderNode::~WGEOffscreenRenderNode()
{
    // cleanup
}

osg::ref_ptr< WGEOffscreenRenderPass > WGEOffscreenRenderNode::addGeometryRenderPass( osg::ref_ptr< osg::Node > node )
{
    // create a new pass
    osg::ref_ptr< WGEOffscreenRenderPass > pass = new WGEOffscreenRenderPass( m_textureWidth, m_textureHeight, m_hud, m_nextPassNum );
    m_nextPassNum++;

    // this node needs to keep all the pass instances. Only this way, the OSG traverses and renders these nodes in the order specified by
    // m_nextPassNum.
    pass->addChild( node );
    insert( pass );   // insert into this group

    // set clear mask and color according to reference cam
    pass->setClearMask( m_referenceCamera->getClearMask() );
    pass->setClearColor( m_referenceCamera->getClearColor() );

    // ensure proper propagation of viewport changes
    pass->addUpdateCallback( new WGEViewportCallback< WGEOffscreenRenderPass >( m_referenceCamera ) );
    return pass;
}

osg::ref_ptr< WGEOffscreenRenderPass >  WGEOffscreenRenderNode::addTextureProcessingPass()
{
    osg::ref_ptr< WGEOffscreenRenderPass > pass = new WGEOffscreenRenderPass( m_textureWidth, m_textureHeight, m_hud, m_nextPassNum );
    m_nextPassNum++;
    insert( pass );   // insert into this group

    // we need to create a nice quad for texture processing spanning the whole texture space






    // ensure proper propagation of viewport changes
    pass->addUpdateCallback( new WGEViewportCallback< WGEOffscreenRenderPass >( m_referenceCamera ) );
    return pass;
}

