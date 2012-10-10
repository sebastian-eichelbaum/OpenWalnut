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

#include <string>

#include <osg/Geode>

#include "../../common/WAssert.h"

#include "WGEOffscreenRenderNode.h"

bool isPowerOfTwo( size_t x )
{
    return ( (x != 0 ) && ( (x & ( ~x + 1 ) ) == x ) );
}

bool checkTextureSize( size_t size )
{
    return !( ( size > 8192 ) || ( size < 8 ) );
}

WGEOffscreenRenderNode::WGEOffscreenRenderNode( osg::ref_ptr< osg::Camera > reference, size_t width, size_t height, bool noHud ):
    WGEGroupNode(),
    m_referenceCamera( reference ),
    m_hud(),
    m_textureWidth( width ),
    m_textureHeight( height ),
    m_nextPassNum( 0 )
{
    WAssert( checkTextureSize( width ) && checkTextureSize( height ), "Invalid offscreen texture size. Must be power of two and in [8,4096]." );

    // initialize members
    m_hud = new WGETextureHud();
    if( !noHud )
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

osg::ref_ptr< WGEOffscreenRenderPass > WGEOffscreenRenderNode::addGeometryRenderPass( osg::ref_ptr< osg::Node > node, std::string name )
{
    // create a plain render pass and add some geometry
    osg::ref_ptr< WGEOffscreenRenderPass > pass = addRenderPass< WGEOffscreenRenderPass >( name );
    pass->addChild( node );
    return pass;
}

osg::ref_ptr< WGEOffscreenRenderPass > WGEOffscreenRenderNode::addGeometryRenderPass( osg::ref_ptr< osg::Node > node,
                                                                                      osg::ref_ptr< WGEShader > shader,
                                                                                      std::string name )
{
    // create a plain render pass and add some geometry
    osg::ref_ptr< WGEOffscreenRenderPass > pass = addRenderPass< WGEOffscreenRenderPass >( name );
    pass->addChild( node );
    shader->apply( pass );
    return pass;
}

osg::ref_ptr< WGEOffscreenTexturePass > WGEOffscreenRenderNode::addTextureProcessingPass( std::string name )
{
    osg::ref_ptr< WGEOffscreenTexturePass > pass = addRenderPass< WGEOffscreenTexturePass >( name );
    return pass;
}

osg::ref_ptr< WGEOffscreenTexturePass > WGEOffscreenRenderNode::addTextureProcessingPass( osg::ref_ptr< WGEShader > shader, std::string name )
{
    osg::ref_ptr< WGEOffscreenTexturePass > pass = addRenderPass< WGEOffscreenTexturePass >( name );
    shader->apply( pass );
    return pass;
}

osg::ref_ptr< WGEOffscreenFinalPass > WGEOffscreenRenderNode::addFinalOnScreenPass( std::string name )
{
    osg::ref_ptr< WGEOffscreenFinalPass > pass = addRenderPass< WGEOffscreenFinalPass >( name );
    return pass;
}

osg::ref_ptr< WGEOffscreenFinalPass > WGEOffscreenRenderNode::addFinalOnScreenPass( osg::ref_ptr< WGEShader > shader, std::string name )
{
    osg::ref_ptr< WGEOffscreenFinalPass > pass = addRenderPass< WGEOffscreenFinalPass >( name );
    shader->apply( pass );
    return pass;
}

osg::ref_ptr< WGETextureHud > WGEOffscreenRenderNode::getTextureHUD() const
{
    return m_hud;
}

