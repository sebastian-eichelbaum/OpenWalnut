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

#ifndef WGEPOSTPROCESSINGNODE_H
#define WGEPOSTPROCESSINGNODE_H

#include <map>
#include <utility>

#include <osg/Switch>

#include "../../common/WPropertyVariable.h"
#include "../../common/WItemSelection.h"
#include "../../common/WSharedAssociativeContainer.h"

#include "../offscreen/WGEOffscreenRenderNode.h"
#include "../offscreen/WGEOffscreenRenderPass.h"
#include "../offscreen/WGEOffscreenFinalPass.h"
#include "../callbacks/WGESwitchCallback.h"
#include "../callbacks/WGENodeMaskCallback.h"
#include "../WGEGroupNode.h"
#include "../WExportWGE.h"

/**
 * This class enables you to add arbitrary nodes that get post-processed in screen space. The only thing you need to take care of is your shader.
 * You need some special parts in it. Please see the all-in-one super-shader-example module WMShaderExample in modules/template.
 *
 * \note Although this is an osg::Switch node, you should avoid using its inherited API unless you know what you do. Using the osg::Switch API
 * might be useful for those who want to modify the post-processing pipeline.
 */
class WGE_EXPORT WGEPostprocessingNode: public osg::Switch // NOLINT
{
public:

    /**
     * Convenience typedef for an osg::ref_ptr
     */
    typedef osg::ref_ptr< WGEPostprocessingNode > RefPtr;

    /**
     * Convenience typedef for an osg::ref_ptr; const
     */
    typedef osg::ref_ptr< const WGEPostprocessingNode > ConstRefPtr;

    /**
     * Create a new post-processing node. It used the WGEOffscreenRenderNode to setup an offscreen, shader-based post-processing for rendered
     * images. This is not limited to geometry but can also be used for ray-traced images.
     *
     * \param reference camera used as reference
     * \param width the width of the textures used in this rendering
     * \param height the height of the textures used in this rendering*
     * \param noHud If true, no hud gets displayed showing the created and used textures.
     */
    WGEPostprocessingNode( osg::ref_ptr< osg::Camera > reference, size_t width = 2048, size_t height = 2048, bool noHud = false );

    /**
     * Destructor.
     */
    virtual ~WGEPostprocessingNode();

    /**
     * Returns the set of properties controlling the post-processing node. You can use them to provide them to the user for example.
     *
     * \return the properties as a group.
     */
    WPropGroup getProperties() const;

    /**
     * Inserts a node to the post-processor and injects the needed code to the specified shader. See class documentation for further details on
     * how the shader gets modified. If you are using an group node, be yourself aware that all nodes in this group need to have the same shader!
     * If not, post-processing will not work properly.
     *
     * \note this is thread-safe and can be done from every thread
     * \note it does NOT apply the shader.
     *
     * \param node the node to post-process
     * \param shader the shader used for the node
     */
    void insert( osg::ref_ptr< osg::Node > node, WGEShader::RefPtr shader = NULL );

    /**
     * Removes the node from the post-processing. If it is not in the post-processing pipeline, nothing happens.
     *
     * \note this is thread-safe and can be done from every thread
     *
     * \param node the node to remove
     */
    void remove( osg::ref_ptr< osg::Node > node );

    /**
     * Removes all associated nodes.
     *
     * \note this is thread-safe and can be done from every thread
     */
    void clear();

    /**
     * Activates/Deactivates the post-processing. This is a shortcut for getProperties()->getProperty( "Enable" )->toPropBool()->set( enable ).
     *
     * \param enable if true, post-processing is active-
     */
    void setEnabled( bool enable = true );

protected:

private:

    /**
     * This type is used to actually store the association between a node and its associated shader and custom preprocessor.
     */
    typedef WSharedAssociativeContainer<
        std::map<
            osg::ref_ptr< osg::Node >,
            std::pair<
                WGEShader::RefPtr,
                WGEShaderPreprocessor::SPtr
            >
        >
    > NodeShaderAssociation;

    /**
     * List of nodes and their corresponding shader and preprocessor.
     */
    NodeShaderAssociation m_nodeShaderAssociation;

    /**
     * The actual offscreen render node.
     */
    osg::ref_ptr< WGEOffscreenRenderNode > m_offscreen;

    /**
     * The group of child nodes to post-process.
     */
    osg::ref_ptr< WGEGroupNode > m_childs;

    /**
     * The first pass, rendering.
     */
    osg::ref_ptr< WGEOffscreenRenderPass > m_render;

    /**
     * The actual post-processing.
     */
    osg::ref_ptr< WGEOffscreenFinalPass > m_postprocess;

    /**
     * This shader actually does post-processing in screen space.
     */
    WGEShader::RefPtr m_postProcessShader;

    /**
     * All the properties of the post-processor.
     */
    WPropGroup m_properties;

    /**
     * If true, post-processing is enabled.
     */
    WPropBool m_active;

    /**
     * If true, a HUD with intermediate textures is shown.
     */
    WPropBool m_showHUD;

    /**
     * The property containing the currently active method or a combination.
     */
    WPropSelection m_activePostprocessors;

    /**
     * Possible post-processors.
     */
    boost::shared_ptr< WItemSelection > m_possiblePostprocessors;

    /**
     * Some text denoting that this is not yet completely done.
     */
    WPropString m_infoText;
};

#endif  // WGEPOSTPROCESSINGNODE_H

