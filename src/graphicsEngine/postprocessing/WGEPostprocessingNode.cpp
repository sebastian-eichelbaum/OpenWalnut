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

#include "../../common/WPropertyHelper.h"

#include "../shaders/WGEShaderPropertyDefineOptions.h"

#include "WGEPostprocessingNode.h"

WGEPostprocessingNode::WGEPostprocessingNode( osg::ref_ptr< osg::Camera > reference, size_t width, size_t height, bool noHud ):
    osg::Switch(),
    m_offscreen( new WGEOffscreenRenderNode( reference, width, height, noHud ) ),
    m_childs( new WGEGroupNode() ),
    m_postProcessShader( new WGEShader( "WGEPostprocessor" ) ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Post-processing", "Post-processing properties" ) ) )
{
    // create both render pass
    m_render = m_offscreen->addGeometryRenderPass(
        m_childs,
        "Rendered"
    );
    m_postprocess = m_offscreen->addFinalOnScreenPass( m_postProcessShader, "Post-processed" );

    // link them together with the corresponding textures
    osg::ref_ptr< osg::Texture2D > renderColorTexture = m_render->attach( osg::Camera::COLOR_BUFFER0 );
    osg::ref_ptr< osg::Texture2D > renderNormalTexture = m_render->attach( osg::Camera::COLOR_BUFFER1 );
    osg::ref_ptr< osg::Texture2D > renderDepthTexture = m_render->attach( osg::Camera::DEPTH_BUFFER );
    m_postprocess->bind( renderColorTexture, 0 );
    m_postprocess->bind( renderNormalTexture, 1 );
    m_postprocess->bind( renderDepthTexture, 2 );

    // add the offscreen renderer and the original node to the switch
    addChild( m_childs );
    addChild( m_offscreen );

    // add some properties here:
    m_active = m_properties->addProperty( "Enable", "If set, post-processing is enabled.", true );
    m_showHUD = m_properties->addProperty( "Show HUD", "If set, the intermediate textures are shown.", false );

    // Post-processings:
    // First: Create a list with name, description and shader define which is used to enable it
    typedef WGEShaderPropertyDefineOptions< WPropSelection >::NameDescriptionDefineTuple Tuple;
    std::vector< Tuple > namesAndDefs;
    namesAndDefs.push_back( Tuple( "Color Only",   "No Post-Processing.", "WGE_POSTPROCESSOR_COLOR" ) );
    namesAndDefs.push_back( Tuple( "Depth",        "Depth Value only.",   "WGE_POSTPROCESSOR_DEPTH" ) );
    namesAndDefs.push_back( Tuple( "Normal",       "Geometry Normal.",    "WGE_POSTPROCESSOR_NORMAL" ) );
    namesAndDefs.push_back( Tuple( "Edge",         "Edge of Rendered Geometry.",    "WGE_POSTPROCESSOR_EDGE" ) );
    namesAndDefs.push_back( Tuple( "Gaussed Color", "Smoothed Color Image using Gauss Filter.",    "WGE_POSTPROCESSOR_GAUSSEDCOLOR" ) );
    namesAndDefs.push_back( Tuple( "Custom", "Provide Your Own Post-processing-Code.",    "WGE_POSTPROCESSOR_CUSTOM" ) );

    // Second: create the Shader option object and the corresponding property automatically:
    WGEShaderPropertyDefineOptions< WPropSelection >::SPtr activePostprocessorsOpts(
        WGEShaderPropertyDefineOptions< WPropSelection >::createSelection(
            "Post-processors",
            "Select the post-processings you want.",
            m_properties,
            namesAndDefs
        )
    );
    m_activePostprocessors = activePostprocessorsOpts->getProperty();
    // avoid that a user selects nothing
    WPropertyHelper::PC_NOTEMPTY::addTo( m_activePostprocessors );

    // let the props control some stuff
    addUpdateCallback( new WGESwitchCallback< WPropBool >( m_active ) );
    m_offscreen->getTextureHUD()->addUpdateCallback( new WGENodeMaskCallback( m_showHUD ) );
    // let the activePostprocessors property control the options in the shader:
    m_postProcessShader->addPreprocessor( activePostprocessorsOpts );
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

void WGEPostprocessingNode::insert( osg::ref_ptr< osg::Node > node, WGEShader::RefPtr shader )
{
    // we need to inject some code to the shader at this point.
    shader->addPreprocessor( WGEShaderPreprocessor::SPtr(
        new WGEShaderPropertyDefineOptions< WPropBool >( m_active, "WGE_POSTPROCESSING_DISABLED", "WGE_POSTPROCESSING_ENABLED" ) )
    );

    // insert node to group node of all children
    m_childs->insert( node );
}

void WGEPostprocessingNode::remove( osg::ref_ptr< osg::Node > node )
{
    m_childs->remove( node );
}

void WGEPostprocessingNode::clear()
{
    m_childs->clear();
}

