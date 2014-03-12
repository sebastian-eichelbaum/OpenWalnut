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
#include "../../common/WItemSelection.h"

#include "../shaders/WGEShaderPropertyDefineOptions.h"
#include "../callbacks/WGENodeMaskCallback.h"
#include "../WGEUtils.h"

#include "WGEPostprocessor.h"

#include "WGEPostprocessingNode.h"

WGEPostprocessingNode::WGEPostprocessingNode( osg::ref_ptr< WGECamera > reference, size_t width, size_t height, bool noHud ):
    osg::Switch(),
    m_childs( new WGEGroupNode() ),
    m_properties( boost::shared_ptr< WProperties >( new WProperties( "Post-processing", "Post-processing properties" ) ) )
{
    // the geometry is always the first in the switch node
    addChild( m_childs );

    // this node has some properties:
    boost::shared_ptr< WItemSelection > m_possibleSelections( new WItemSelection() );
    m_possibleSelections->addItem( "None", "No postprocessing." );

    m_showHud = m_properties->addProperty( "Texture Debug", "If set, all intermediate texture are shown on screen for debugging.", false );
    m_active = m_properties->addProperty( "Enable", "If set, post-processing is enabled.", false, true );
    m_activePostprocessor = m_properties->addProperty( "Postprocessor", "Selection one of the postprocessors.",
                                                       m_possibleSelections->getSelectorFirst(),
                                                       boost::bind( &WGEPostprocessingNode::postprocessorSelected, this ) );
    WPropertyHelper::PC_SELECTONLYONE::addTo( m_activePostprocessor );
    WPropertyHelper::PC_NOTEMPTY::addTo( m_activePostprocessor );

    // control texture HUD
    osg::ref_ptr< WGENodeMaskCallback > textureHudCallback = new WGENodeMaskCallback( m_showHud );

    // get available postprocessors and setup the node
    WGEPostprocessor::ProcessorList processors = WGEPostprocessor::getPostprocessors();
    for( WGEPostprocessor::ProcessorList::const_iterator iter = processors.begin(); iter != processors.end(); ++iter )
    {
        // offscreen node
        osg::ref_ptr< WGEOffscreenRenderNode > offscreen( new WGEOffscreenRenderNode( reference, width, height, noHud ) );
        offscreen->getTextureHUD()->addUpdateCallback( textureHudCallback );

        // if the specific postprocessor requires us to fix the viewport size to the result-texture size:
        offscreen->setLinkViewportToTextureSize( ( *iter )->getFixedViewportSize() );

        // the geometry render step
        osg::ref_ptr< WGEOffscreenRenderPass > render = offscreen->addGeometryRenderPass(
            m_childs,
            "Rendered"
        );

        // create G-Buffer
        WGEPostprocessor::PostprocessorInput buf( WGEPostprocessor::PostprocessorInput::attach( render ) );

        // let the specific post processor build its pipeline
        WGEPostprocessor::SPtr processor = ( *iter )->create( offscreen, buf );
        m_postprocs.push_back( processor );

        // add the postprocessor's properties
        m_properties->addProperty( processor->getProperties() );
        processor->getProperties()->setHidden( true );

        // add it to the selection prop
        m_possibleSelections->addItem( processor->getName(), processor->getDescription() );

        // the final step
        osg::ref_ptr< WGEOffscreenFinalPass > output = offscreen->addFinalOnScreenPass( new WGEShader( "WGEPostprocessorCombiner" ),
                                                                                        "Output" );
        output->bind( processor->getOutput(), 0 );

        // does this processor provide a depth?
        osg::ref_ptr< osg::Texture2D > depthTex = processor->getDepth();
        if( !depthTex )
        {
            depthTex = buf.m_depthTexture;
        }
        output->bind( depthTex, 1 );

        // add the offscreen renderer and the original node to the switch
        addChild( offscreen );
    }

    // let the props control some stuff
    addUpdateCallback( new WGESwitchCallback< WPropSelection >( m_activePostprocessor ) );
}

WGEPostprocessingNode::~WGEPostprocessingNode()
{
    // cleanup
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

WGEPostprocessor::SPtr WGEPostprocessingNode::getCurrentPostprocessor() const
{
    return m_postprocs[ m_activePostprocessor->get() - 1 ];
}

void WGEPostprocessingNode::postprocessorSelected()
{
    if( m_postprocs.size() == 0 )
    {
        m_active->set( false );
        return;
    }

    size_t active = m_activePostprocessor->get();

    // this triggers several shader preprocessors of all child nodes
    m_active->set( active != 0 );

    // hide all, but not the active one
    for( size_t i = 0; i < m_postprocs.size(); ++i )
    {
        m_postprocs[ i ]->getProperties()->setHidden( i != ( active - 1 ) );
    }
}
