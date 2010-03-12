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

#include <boost/thread.hpp>

#include <osg/Projection>
#include <osg/Geode>
#include <osg/Texture2D>

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
         * \param transparency true if transparency should be shown
         */
        WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture, bool transparency = false );

        /**
         * Destructor.
         */
        ~WGETextureHudEntry();

        /**
         * Returns the real width of the contained texture.
         *
         * \return the real width.
         */
        unsigned int getRealWidth();

        /**
         * Returns the real height of the contained texture.
         *
         * \return the real height.
         */
        unsigned int getRealHeight();

    protected:

        /**
         * The texture.
         */
        osg::ref_ptr< osg::Texture2D > m_texture;
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
     * Gets the maximum width of a tex element.
     *
     * \return the maximum width.
     */
    unsigned int getMaxElementWidth();

    /**
     * Sets the new maximum width of a texture column.
     *
     * \param width the new width
     */
    void setMaxElementWidth( unsigned int width );

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
        explicit SafeUpdateCallback( osg::ref_ptr< WGETextureHud > hud ): m_hud( hud )
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
        osg::ref_ptr< WGETextureHud > m_hud;
    };
};

#endif  // WGETEXTUREHUD_H

