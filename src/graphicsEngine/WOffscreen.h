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

#ifndef WOFFSCREEN_H
#define WOFFSCREEN_H

#include <osg/Camera>
#include <osg/FrameBufferObject>

/**
 * This class encapsulates an OSG Camera and a corresponding framebuffer object. It is especially useful for offscreen renderings. It is a camera
 * which, by default, is the same as the camera in the this instance nesting graph. It allows simple attachment of textures to a offscreen
 * rendering as well as easy texture creation.
 */
class WOffscreen: public osg::Camera
{
public:

    /**
     * Creates a new offscreen rendering instance. It uses the specified camera for setup.
     *
     * \param reference the reference camera.
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     */
    WOffscreen( osg::ref_ptr< osg::Camera > reference, int num = 0 );

    /**
     * Destructor.
     */
    virtual ~WOffscreen();

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
     * This method attaches a texture to the given buffer. The texture gets created with the resolution of the FBO.
     *
     * \param buffer the buffer to attach the new texture to
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     *
     * \return the newly created texture.
     */
    osg::ref_ptr< osg::Texture2D > attach( BufferComponent buffer );

    /**
     * Detaches the texture currently bound to the specified buffer.
     *
     * \param buffer the buffer to detach.
     *
     * \note if the node is added to the graph, these functions should only be called from within an update callback.
     */
    void detach( BufferComponent buffer );

    /**
     * Creates a new texture suitable for this offscreen rendering instance. The texture will have the same size as the viewport of this camera.
     *
     * \param internalFormat the format to use for the texture.
     *
     * \return the newly created texture
     */
    osg::ref_ptr< osg::Texture2D > createTexture( GLint internalFormat = GL_RGBA );

protected:

    /**
     * The camera to which is used for setting this camera up.
     */
    osg::ref_ptr< osg::Camera > m_referenceCamera;

    /**
     * The framebuffer object to use for this camera.
     */
    osg::ref_ptr<osg::FrameBufferObject> m_fbo;

private:
};

#endif  // WOFFSCREEN_H

