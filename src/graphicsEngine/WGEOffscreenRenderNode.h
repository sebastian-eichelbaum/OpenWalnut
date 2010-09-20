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

#ifndef WGEOFFSCREENRENDERNODE_H
#define WGEOFFSCREENRENDERNODE_H

#include <osg/Camera>

#include "WGEGroupNode.h"
#include "WGEOffscreenRenderPass.h"
#include "WGETextureHud.h"

/**
 * This type of node basically is a convenience class for managing and creating offscreen renderings. The children of this node should be of type
 * \ref WGEOffscreenRenderPass. This class provides factories to create offscreen-render-pass instances with proper sizes with a coupling to a
 * reference camera. This is useful to provide automatic viewport scaling etc. to each render-pass. You do not explicitly need this class to
 * create offscreen-renderings at all. You can manually manage multiple WGEOffscreenRenderPass instances.
 *
 * It is important to understand, that the graph (your scene) must not be a children of this node. This node can be placed somewhere in your
 * scene. The OSG collects all the cameras (and offscreen-cameras) and render then independently from their position in the graph (except for
 * transformations inherited from others).
 *
 * \note Please not that you should not modify the whole wiring and offscreen configuration if the this node has been added as it is not
 * thread-safe.
 */
class WGEOffscreenRenderNode: public WGEGroupNode
{
public:
    /**
     * Create a new managing instance. It uses the specified camera as reference to all created offscreen-render-pass instances. Especially
     * viewport, clear-mask and clear-color get used.
     *
     * \param reference camera used as reference
     * \param width the width of the textures used in this rendering
     * \param height the height of the textures used in this rendering*
     * \param noHud If true, no hud gets displayed showing the created and used textures.
     */
    WGEOffscreenRenderNode( osg::ref_ptr< osg::Camera > reference, size_t width, size_t height, bool noHud = false );

    /**
     * Destructor.
     */
    virtual ~WGEOffscreenRenderNode();

    /**
     * Creates a new offscreen-render-pass coupled with the reference camera which renders a specified OSG graph to a texture.
     *
     * \param node the node which represents the subgraph.
     *
     * \note never forget to remove the returned node if not used anymore or use WGEGroup::clean.
     *
     * \return the geometry render pass.
     */
    virtual osg::ref_ptr< WGEOffscreenRenderPass > addGeometryRenderPass( osg::ref_ptr< osg::Node > node );

    /**
     * Creates a new offscreen-render-pass coupled with the reference camera which simply processes textures. All the in- and output textures
     * have to be specified manually.
     *
     * \note never forget to remove the returned node if not used anymore or use WGEGroup::clean.
     *
     * \return the texture processing pass created.
     */
    virtual osg::ref_ptr< WGEOffscreenRenderPass >  addTextureProcessingPass();

protected:

private:

    /**
     * The camera to which is used for setting this camera up.
     */
    osg::ref_ptr< osg::Camera > m_referenceCamera;

    /**
     * The pointer to the hud used to render all used texture buffers. This can be NULL. It gets distributed to all created render-pass
     * instances.
     */
    osg::ref_ptr< WGETextureHud > m_hud;

    /**
     * The width of each texture in this offscreen rendering.
     */
    size_t m_textureWidth;

    /**
     * The height of each texture in this offscreen rendering.
     */
    size_t m_textureHeight;

    /**
     * The number of the next pass getting added.
     */
    size_t m_nextPassNum;
};

#endif  // WGEOFFSCREENRENDERNODE_H

