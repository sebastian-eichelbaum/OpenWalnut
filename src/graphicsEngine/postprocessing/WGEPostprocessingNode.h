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

#include <osg/Switch>

#include "../../common/WPropertyVariable.h"

#include "../offscreen/WGEOffscreenRenderNode.h"
#include "../offscreen/WGEOffscreenRenderPass.h"
#include "../offscreen/WGEOffscreenFinalPass.h"
#include "../callbacks/WGESwitchCallback.h"
#include "../callbacks/WGENodeMaskCallback.h"
#include "../WGEGroupNode.h"
#include "../WExportWGE.h"

/**
 *
 */
class WGE_EXPORT WGEPostprocessingNode: public osg::Switch // NOLINT
{
public:
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

protected:

private:

    osg::ref_ptr< WGEOffscreenRenderNode > m_offscreen;
    osg::ref_ptr< WGEGroupNode > m_childs;
    osg::ref_ptr< WGEOffscreenRenderPass > m_render;
    osg::ref_ptr< WGEOffscreenFinalPass > m_postprocess;

    osg::ref_ptr< WGESwitchCallback< WPropBool > > m_switchCallback;
    osg::ref_ptr< WGENodeMaskCallback > m_hudCallback;

    WPropGroup m_properties;
    WPropBool m_active;
    WPropBool m_showHUD;
};

#endif  // WGEPOSTPROCESSINGNODE_H

