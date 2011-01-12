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

#ifndef WGEOFFSCREENFINALPASS_H
#define WGEOFFSCREENFINALPASS_H

#include <string>

#include <osg/Geode>
#include <osg/TexMat>

#include "WGEOffscreenTexturePass.h"

class WGETextureHud;

/**
 * This class is basically a WGEOffscreenTexturePass with the difference that it finally renders to the on-screen frame buffer.
 */
class WGEOffscreenFinalPass: public WGEOffscreenTexturePass
{
public:
    /**
     * Convenience typedef for an osg::ref_ptr
     */
    typedef osg::ref_ptr< WGEOffscreenFinalPass > RefPtr;

    /**
     * Convenience typedef for an osg::ref_ptr; const
     */
    typedef osg::ref_ptr< const WGEOffscreenFinalPass > ConstRefPtr;

    /**
     * Creates a new rendering pass instance which actually renders on a full-screen quad.
     *
     * \param textureWidth the width of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.
     * \param textureHeight the height of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.*
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     */
    WGEOffscreenFinalPass( size_t textureWidth, size_t textureHeight, int num = 0 );

    /**
     * Creates a new rendering pass instance which actually renders on a full-screen quad.
     *
     * \param textureWidth the width of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.
     * \param textureHeight the height of all the textures created and used by this render pass. This should be large enough for every reasonable
     *                     viewport size.*
     * \param num the order number. This camera gets rendered at the num'th place in the pre render queue of the subgraph it is attached to.
     * \param hud the hud that gets notified about attached and detached textures. Useful for debugging.
     * \param name the name of this render pass. This is a nice debugging feature in conjunction with WGETextureHud as it gets displayed there.
     */
    WGEOffscreenFinalPass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name, int num = 0 );

    /**
     * Destructor.
     */
    virtual ~WGEOffscreenFinalPass();

protected:
private:
};

#endif  // WGEOFFSCREENFINALPASS_H

