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

#include "WGEPostprocessingNode.h"

WGEPostprocessingNode::WGEPostprocessingNode( osg::ref_ptr< osg::Camera > reference, size_t width, size_t height, bool noHud ):
    osg::Switch(),
    m_offscreen( new WGEOffscreenRenderNode( reference, width, height, noHud ) ),
    m_childs( new WGEGroupNode() ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Post-processing", "Post-processing properties" ) ) )
{
    // initialize members

    // create both render pass
    m_render = m_offscreen->addGeometryRenderPass(
        m_childs,
        "Rendered"
    );
    m_postprocess = m_offscreen->addFinalOnScreenPass( "Post-processed" );

    // link them together with the corresponding textures
    osg::ref_ptr< osg::Texture2D > renderColorTexture = m_render->attach( osg::Camera::COLOR_BUFFER0 );
    osg::ref_ptr< osg::Texture2D > renderNDTexture = m_render->attach( osg::Camera::COLOR_BUFFER1 );
    m_postprocess->bind( renderColorTexture, 0 );
    m_postprocess->bind( renderNDTexture, 1 );

    // add the offscreen renderer and the original node to the switch
    addChild( m_childs );
    addChild( m_offscreen );

    // add some properties here:
    m_active = m_properties->addProperty( "Enable", "If set, post-processing is enabled.", true );
    m_showHUD = m_properties->addProperty( "Show HUD", "If set, the intermediate textures are shown.", false );

    // let the props control some stuff
    m_switchCallback = new WGESwitchCallback< WPropBool >( m_active );
    m_hudCallback = new WGENodeMaskCallback( m_showHUD );
    addUpdateCallback( m_switchCallback );
    m_offscreen->getTextureHUD()->addUpdateCallback( m_hudCallback );
}

WGEPostprocessingNode::~WGEPostprocessingNode()
{
    // cleanup
    m_render->detach( osg::Camera::COLOR_BUFFER0 );
    m_render->detach( osg::Camera::COLOR_BUFFER1 );
}

WPropGroup WGEPostprocessingNode::getProperties() const
{
    return m_properties;
}

