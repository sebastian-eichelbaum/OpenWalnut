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

#include <osg/Projection>
#include <osg/MatrixTransform>
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
    class WGETextureHudEntry: public osg::Geode
    {
    public: // NOLINT

        /**
         * Constructor.
         *
         * \param texture the texture to show in the HUD
         */
        explicit WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture );

        /**
         * Destructor.
         */
        ~WGETextureHudEntry();

    protected:
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

protected:

private:
};

#endif  // WGETEXTUREHUD_H

