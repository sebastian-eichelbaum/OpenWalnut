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

#ifndef WGEOFFSCREENTEXTUREPASS_H
#define WGEOFFSCREENTEXTUREPASS_H

#include <string>

#include <osg/Geode>
#include <osg/TexMat>

#include "WGEOffscreenRenderPass.h"

class WGETextureHud;

/**
 * This class encapsulates an OSG Camera and a corresponding framebuffer object. It is a specialized variant of \ref WGEOffscreenRenderPass,
 * optimized for processing textures. Therefore, it creates an correctly sized quad and can process each pixel in the fragment shader.
 */
class WGEOffscreenTexturePass: public WGEOffscreenRenderPass
{
public:
    /**
     * Creates a new offscreen rendering instance.
     *
     * \param textureWidth the width of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.
     * \param textureHeight the height of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.*
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     */
    WGEOffscreenTexturePass( size_t textureWidth, size_t textureHeight, int num = 0 );

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
    WGEOffscreenTexturePass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name, int num = 0 );

    /**
     * Destructor.
     */
    virtual ~WGEOffscreenTexturePass();

protected:
private:

    /**
     * Sets the whole node up. Used to get some code duplication out of the constructors.
     */
    void setup();

    /**
     * The texture matrix for this pass. Used to scale the texture coordinates according to viewport/texture size relation.
     */
    osg::ref_ptr< osg::TexMat > m_texMat;

    /**
     * Callback which aligns and renders the textures.
     */
    class TextureMatrixUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param pass the pass to which this callback is applied. Needed for accessing some mebers.
         */
        explicit TextureMatrixUpdateCallback( WGEOffscreenTexturePass* pass ): m_pass( pass )
        {
        };

        /**
         * operator () - called during the update traversal.
         *
         * \param node the osg node
         * \param nv the node visitor
         */
        virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

        /**
         * The pass used in conjunction with this callback.
         */
        WGEOffscreenTexturePass* m_pass;
    };
};

#endif  // WGEOFFSCREENTEXTUREPASS_H

