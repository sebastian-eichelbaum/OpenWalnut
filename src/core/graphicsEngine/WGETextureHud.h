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

#ifndef WGETEXTUREHUD_H
#define WGETEXTUREHUD_H

#include <list>
#include <string>

#include <boost/thread.hpp>

#include <osg/Projection>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osg/TexMat>
#include <osgText/Text>

#include "WGEGroupNode.h"

/**
 * This class implements a HUD showing several textures on screen. This is especially useful as debugging tool during offscreen rendering. It is
 * possible to add and remove textures to it. The size of the texture on screen depends on the screen size, as well as the layout of each texture
 * depends on the screen size.
 */
class WGETextureHud: public osg::Projection
{
public:
    /**
     * Default constructor.
     */
    WGETextureHud();

    /**
     * Destructor.
     */
    virtual ~WGETextureHud();

    /**
     * Class implementing one texture HUD entry representing a texture in the HUD.
     */
    class WGETextureHudEntry: public osg::MatrixTransform
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param texture the texture to show in the HUD
         * \param name a telling name to support the illustrative function of the HUD
         * \param transparency true if transparency should be shown
         */
        WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture, std::string name, bool transparency = false );

        /**
         * Destructor.
         */
        ~WGETextureHudEntry();

        /**
         * Returns the real width of the contained texture.
         *
         * \return the real width.
         */
        unsigned int getRealWidth() const;

        /**
         * Returns the real height of the contained texture.
         *
         * \return the real height.
         */
        unsigned int getRealHeight() const;

        /**
         * Get the texture matrix state for this entry.
         *
         * \return the texture matrix state
         */
        osg::ref_ptr< osg::TexMat > getTextureMatrix() const;

        /**
         * Returns the name of the entry.
         *
         * \return name of the entry.
         */
        std::string getName() const;

        /**
         * Gets the texture associated with the entry.
         *
         * \return the texture
         */
        osg::ref_ptr< osg::Texture2D > getTexture() const;

        /**
         * Set maximum text width. This is useful to avoid oversize text. Call only from inside a OSG callback.
         *
         * \param width the max width
         */
        void setMaxTextWidth( float width );
    protected:
        /**
         * The texture.
         */
        osg::ref_ptr< osg::Texture2D > m_texture;

        /**
         * The texture matrix for this entry.
         */
        osg::ref_ptr< osg::TexMat > m_texMat;

        /**
         * The label text.
         */
        osgText::Text* m_label;

        /**
         * The name for this HUD entry.
         */
        std::string m_name;

        /**
         * Mqx text width.
         */
        float m_maxTextWidth;
    private:
    };

    /**
     * Adds the specified HUD element to the HUD.
     *
     * \param texture the texture to show.
     */
    void addTexture( osg::ref_ptr< WGETextureHudEntry > texture );

    /**
     * Remove the texture from the HUD.
     *
     * \param texture the texture to remove.
     */
    void removeTexture( osg::ref_ptr< WGETextureHudEntry > texture );

    /**
     * Remove the texture from the HUD.
     *
     * \param texture the texture to remove.
     */
    void removeTexture( osg::ref_ptr< osg::Texture > texture );

    /**
     * Gets the maximum width of a tex element.
     *
     * \return the maximum width.
     */
    unsigned int getMaxElementWidth() const;

    /**
     * Sets the new maximum width of a texture column.
     *
     * \param width the new width
     */
    void setMaxElementWidth( unsigned int width );

    /**
     * Sets the viewport of the camera housing this HUD. It is needed to have proper scaling of each texture tile. You can use
     * \ref WGEViewportCallback to handle this automatically.
     *
     * \param viewport the viewport
     */
    void setViewport( osg::Viewport* viewport );

    /**
     * Set the viewport to be used for textures too. This is useful if an offscreen rendering renders only into a part of the texture. If
     * coupling is disabled, the whole texture gets rendered.
     *
     * \param couple if true, the viewport set by \ref setViewport gets also used for texture space.
     */
    void coupleViewportWithTextureViewport( bool couple = true );

    /**
     * Returns the render bin used by the HUD.
     *
     * \return the bin number
     */
    size_t getRenderBin() const;

protected:
    /**
     * The group Node where all those texture reside in. Theoretically, it is nonsense to use a separate group inside a osg::Projection since it
     * also is a group node. But WGEGroupNode offers all those nice and thread-safe insert/remove methods.
     */
    osg::ref_ptr< WGEGroupNode > m_group;

    /**
     * The maximum element width.
     */
    unsigned int m_maxElementWidth;

    /**
     * The render bin to use
     */
    size_t m_renderBin;

    /**
     * The current viewport of
     */
    osg::Viewport* m_viewport;

    /**
     * The viewport in texture space to allow viewing parts of the texture.
     */
    bool m_coupleTexViewport;

private:
    /**
     * Callback which aligns and renders the textures.
     */
    class SafeUpdateCallback : public osg::NodeCallback
    {
    public: // NOLINT
        /**
         * Constructor.
         *
         * \param hud just set the creating HUD as pointer for later reference.
         */
        explicit SafeUpdateCallback( WGETextureHud* hud ): m_hud( hud )
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
         * Pointer used to access members of the hud. This is faster than casting the first parameter of operator() to WGETextureHud.
         */
        WGETextureHud* m_hud;
    };
};

#endif  // WGETEXTUREHUD_H

