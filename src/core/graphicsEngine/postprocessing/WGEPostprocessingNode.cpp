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
#include "../WGEUtils.h"

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
    osg::ref_ptr< osg::Texture2D > renderNormalTexture = m_render->attach( osg::Camera::COLOR_BUFFER1, GL_RGB );
    osg::ref_ptr< osg::Texture2D > renderParameterTexture = m_render->attach( osg::Camera::COLOR_BUFFER2, GL_LUMINANCE );
    osg::ref_ptr< osg::Texture2D > renderTangentTexture = m_render->attach( osg::Camera::COLOR_BUFFER3, GL_RGB );
    osg::ref_ptr< osg::Texture2D > renderDepthTexture = m_render->attach( osg::Camera::DEPTH_BUFFER );
    m_postprocess->bind( renderColorTexture, 0 );
    m_postprocess->bind( renderNormalTexture, 1 );
    m_postprocess->bind( renderParameterTexture, 2 );
    m_postprocess->bind( renderDepthTexture, 3 );

    // add the offscreen renderer and the original node to the switch
    addChild( m_childs );
    addChild( m_offscreen );

    // some info text:
    m_infoText = m_properties->addProperty( "Hint", "This is for advanced users.",
        std::string( "The post-processing has to be seen as facility to create appealing images. The here offered options are not all "
                     "possibilities. The most powerful effects can be achieved by using custom combinations (using custom GLSL code) of "
                     "post-processors and is recommended for <b>advanced users</b> only." )
    );
    m_infoText->setPurpose( PV_PURPOSE_INFORMATION );

    // add some properties here:
    m_active = m_properties->addProperty( "Enable", "If set, post-processing is enabled.", true );
    m_showHUD = m_properties->addProperty( "Show HUD", "If set, the intermediate textures are shown.", false );

    // Post-processings:
    // First: Create a list with name, description and shader define which is used to enable it
    typedef WGEShaderPropertyDefineOptionsTools::NameDescriptionDefineTuple Tuple;
    std::vector< Tuple > namesAndDefs;
    namesAndDefs.push_back( Tuple( "Color Only",   "No Post-Processing.",                               "WGE_POSTPROCESSOR_COLOR" ) );
    namesAndDefs.push_back( Tuple( "Smoothed Color", "Smoothed Color Image using Gauss Filter.",        "WGE_POSTPROCESSOR_GAUSSEDCOLOR" ) );
    namesAndDefs.push_back( Tuple( "PPL - Phong",   "Per-Pixel-Lighting using Phong.",                  "WGE_POSTPROCESSOR_PPLPHONG" ) );
    namesAndDefs.push_back( Tuple( "SSAO", "Screen-Space Ambient Occlusion.",                           "WGE_POSTPROCESSOR_SSAO" ) );
    namesAndDefs.push_back( Tuple( "SSAO with Phong", "Screen-Space Ambient Occlusion in combination with Phong ( Phong + SSAO * Color ).",
                                                                                                        "WGE_POSTPROCESSOR_SSAOWITHPHONG" ) );
    namesAndDefs.push_back( Tuple( "Cel-Shading",  "Under-sampling of the color for cartoon-like shading.", "WGE_POSTPROCESSOR_CELSHADING" ) );
    namesAndDefs.push_back( Tuple( "Depth-Cueing", "Use the Depth to fade out the pixel's brightness.", "WGE_POSTPROCESSOR_DEPTHFADING" ) );
    namesAndDefs.push_back( Tuple( "Depth-of-Field", "Depth of field effect.",                          "WGE_POSTPROCESSOR_DOF" ) );
    namesAndDefs.push_back( Tuple( "Edge",         "Edge of Rendered Geometry.",                        "WGE_POSTPROCESSOR_EDGE" ) );
    namesAndDefs.push_back( Tuple( "Depth",        "Depth Value only.",                                 "WGE_POSTPROCESSOR_DEPTH" ) );
    namesAndDefs.push_back( Tuple( "Smoothed Depth", "Gauss-Smoothed Depth Value only.",                "WGE_POSTPROCESSOR_GAUSSEDDEPTH" ) );
    namesAndDefs.push_back( Tuple( "Normal",       "Geometry Normal.",                                  "WGE_POSTPROCESSOR_NORMAL" ) );
    namesAndDefs.push_back( Tuple( "Custom", "Provide Your Own Post-processing-Code.",                  "WGE_POSTPROCESSOR_CUSTOM" ) );

    // Second: create the Shader option object and the corresponding property automatically:
    WGEShaderPropertyDefineOptions< WPropSelection >::SPtr activePostprocessorsOpts(
        WGEShaderPropertyDefineOptionsTools::createSelection(
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

    // some of the post-processors need some white noise, like the ssao
    const size_t size = 64;
    osg::ref_ptr< WGETexture2D > randTex = wge::genWhiteNoiseTexture( size, size, 3 );
    m_postprocess->bind( randTex, 4 );
    m_postprocess->bind( renderTangentTexture, 5 );
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
    // the shader needs an own preprocessor.
    WGEShaderPreprocessor::SPtr preproc( new WGEShaderPropertyDefineOptions< WPropBool >(
        m_active, "WGE_POSTPROCESSING_DISABLED", "WGE_POSTPROCESSING_ENABLED" )
    );

    // we need to inject some code to the shader at this point.
    shader->addPreprocessor( preproc );

    // do it thread-safe as we promise to be thread-safe
    NodeShaderAssociation::WriteTicket w = m_nodeShaderAssociation.getWriteTicket();
    // to keep track of which node is associated with which shader and preprocessor:
    w->get()[ node ] = std::make_pair( shader, preproc );

    // insert node to group node of all children
    m_childs->insert( node );
}

void WGEPostprocessingNode::remove( osg::ref_ptr< osg::Node > node )
{
    // do it thread-safe as we promise to be thread-safe
    NodeShaderAssociation::WriteTicket w = m_nodeShaderAssociation.getWriteTicket();

    // remove the item from our map
    NodeShaderAssociation::Iterator item = w->get().find( node );

    if( item != w->get().end() )
    {
        // we need to remove the preprocessor from the shader.
        ( *item ).second.first->removePreprocessor( ( *item ).second.second );
        w->get().erase( item );
    }

    // although we may not find the node in our association list, try to remove it
    m_childs->remove( node );
}

void WGEPostprocessingNode::clear()
{
    // do it thread-safe as we promise to be thread-safe
    NodeShaderAssociation::WriteTicket w = m_nodeShaderAssociation.getWriteTicket();

    // remove from node-shader association list
    for( NodeShaderAssociation::Iterator iter = w->get().begin(); iter != w->get().end(); ++iter )
    {
        ( *iter ).second.first->removePreprocessor( ( *iter ).second.second );
    }
    w->get().clear();

    // remove the node from the render group
    m_childs->clear();
}

void WGEPostprocessingNode::setEnabled( bool enable )
{
    m_active->set( enable );
}

