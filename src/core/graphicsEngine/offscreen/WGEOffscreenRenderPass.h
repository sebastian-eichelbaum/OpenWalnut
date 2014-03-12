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

#ifndef WGEOFFSCREENRENDERPASS_H
#define WGEOFFSCREENRENDERPASS_H

#include <string>

#include <osg/Camera>
#include <osg/FrameBufferObject>

#include "../WGEUtils.h"
#include "../WGETexture.h"
#include "../WGECamera.h"

class WGETextureHud;

/**
 * This class encapsulates an OSG Camera and a corresponding framebuffer object. It is especially useful for offscreen renderings. It is a camera
 * which, by default, is the same as the camera in the this instance nesting graph. It allows simple attachment of textures to a offscreen
 * rendering as well as easy texture creation.
 */
class WGEOffscreenRenderPass: public WGECamera // NOLINT
{
public:
    /**
     * Convenience typedef for an osg::ref_ptr
     */
    typedef osg::ref_ptr< WGEOffscreenRenderPass > RefPtr;

    /**
     * Convenience typedef for an osg::ref_ptr; const
     */
    typedef osg::ref_ptr< const WGEOffscreenRenderPass > ConstRefPtr;

    /**
     * Creates a new offscreen rendering instance.
     *
     * \param textureWidth the width of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.
     * \param textureHeight the height of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.*
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     */
    WGEOffscreenRenderPass( size_t textureWidth, size_t textureHeight, int num = 0 );

    /**
     * Creates a new offscreen rendering instance.
     *
     * \param textureWidth the width of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.
     * \param textureHeight the height of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.*
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     * \param hud the hud that gets notified about attached and detached textures. Useful for debugging.
     * \param name the name of this render pass. This is a nice debugging feature in conjunction with WGETextureHud as it gets displayed there.
     */
    WGEOffscreenRenderPass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name, int num = 0 );

    /**
     * Destructor.
     */
    virtual ~WGEOffscreenRenderPass();

    /**
     * Attach a given texture to a buffer.
     *
     * \param buffer the buffer to attach the texture to
     * \param texture the texture to attach
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     */
    void attach( BufferComponent buffer, osg::ref_ptr< osg::Texture2D > texture );

    /**
     * Attach a given image to a buffer. Use this if you want to read back your rendered textures. This does not replace the attach call for
     * textures. This only utilizes the multiple render target extension and allows you to read back your buffers.
     *
     * \param buffer the buffer to attach the texture to
     * \param image the image to attach
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     */
    void attach( BufferComponent buffer, osg::ref_ptr< osg::Image > image );

    /**
     * This method attaches a texture to the given buffer. The texture gets created with the resolution of the FBO.
     *
     * On Mac OSX, only GL_RGBA works as internal format, so all input to internalFormat is ignored and overwritten by GL_RGBA internally.
     *
     * \param buffer the buffer to attach the new texture to
     * \param internalFormat the format to use. By default, RGBA
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     *
     * \return the newly created texture.
     */
    osg::ref_ptr< osg::Texture2D > attach( BufferComponent buffer, GLint internalFormat = GL_RGBA );

    /**
     * Detaches the texture currently bound to the specified buffer.
     *
     * \param buffer the buffer to detach.
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     */
    void detach( BufferComponent buffer );

    /**
     * This is a shortcut for wge::bindTexture. See \ref wge::bindTexture for details.
     *
     * \param unit the unit to use
     * \param texture the texture to use.
     * \tparam T the type of texture. Usually osg::Texture3D or osg::Texture2D.
     */
    template < typename T >
    void bind( osg::ref_ptr< T > texture, size_t unit = 0 );

    /**
     * Creates a new texture suitable for this offscreen rendering instance. The texture will have the same size as the viewport of this camera.
     *
     * \param internalFormat the format to use for the texture.
     *
     * \return the newly created texture
     */
    osg::ref_ptr< osg::Texture2D > createTexture( GLint internalFormat = GL_RGBA );

    /**
     * Returns the name of this render pass.
     *
     * \return the name
     */
    std::string getName() const;

    /**
     * Returns the buffer name. This is useful for debugging messages and so on as it maps a buffer constant to its name.
     *
     * \param buffer the buffer to get the name for
     *
     * \return the name
     */
    static std::string getBufferName( BufferComponent buffer );

    /**
     * Get the size of the underlying texture.
     *
     * \return the width
     */
    size_t getTextureWidth() const;

    /**
     * Get the size of the underlying texture.
     *
     * \return the height
     */
    size_t getTextureHeight() const;

    /**
     * The uniform to add. This is a shortcut for this->getOrCreateStateSet()->addUniform( uniform ).
     *
     * \param uniform the uniform to add
     */
    virtual void addUniform( osg::ref_ptr< osg::Uniform > uniform );
protected:
    /**
     * The width of the textures used for this pass. This should be as large as needed for each "common" viewport."
     */
    size_t m_width;

    /**
     * The height of the textures used for this pass. This should be as large as needed for each "common" viewport."
     */
    size_t m_height;

    /**
     * The framebuffer object to use for this camera.
     */
    osg::ref_ptr<osg::FrameBufferObject> m_fbo;

    /**
     * Gets notified about any added and removed attachment
     */
    osg::ref_ptr< WGETextureHud > m_hud;

    /**
     * The name if the rendering pass. Especially useful in conjunction with m_hud.
     */
    std::string m_name;
private:
};

template < typename T >
void WGEOffscreenRenderPass::bind( osg::ref_ptr< T > texture, size_t unit )
{
    wge::bindTexture( this, texture, unit );
}

#endif  // WGEOFFSCREENRENDERPASS_H

