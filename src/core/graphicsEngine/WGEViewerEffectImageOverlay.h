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

#ifndef WGEVIEWEREFFECTIMAGEOVERLAY_H
#define WGEVIEWEREFFECTIMAGEOVERLAY_H

#include <osg/Texture2D>

#include "core/common/WLogger.h"

#include "callbacks/WGEShaderAnimationCallback.h"

#include "WGEViewerEffect.h"

class WGEViewer;

/**
 * Image Overlay Effect.
 */
class WGEViewerEffectImageOverlay: public WGEViewerEffect
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WGEViewerEffectImageOverlay >.
     */
    typedef osg::ref_ptr< WGEViewerEffectImageOverlay > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WGEViewerEffectImageOverlay >.
     */
    typedef osg::ref_ptr< const WGEViewerEffectImageOverlay > ConstSPtr;

    /**
     * Default constructor.
     */
    WGEViewerEffectImageOverlay();

    /**
     * Destructor.
     */
    virtual ~WGEViewerEffectImageOverlay();

    /**
     * Set the reference viewer. Needed as the effect itself is not able to query important camera states. This can be the viewer on which the
     * effect is applied but does not have to. Important is a valid WGECamera, including a properly setup viewport.
     *
     * \param viewer the viewer in which the effect is used.
     */
    void setReferenceViewer( boost::shared_ptr< WGEViewer > viewer );

    /**
     * Query current reference viewer.
     *
     * \return the reference viewer or NULL if none has been set.
     */
    const boost::shared_ptr< WGEViewer > getReferenceViewer() const;

protected:
    /**
     * Width of the image in pixels.
     */
    osg::ref_ptr< osg::Uniform > m_overlayWidth;

    /**
     * Height of the image in pixels.
     */
    osg::ref_ptr< osg::Uniform > m_overlayHeight;

    /**
     * Width of the viewport in pixels.
     */
    osg::ref_ptr< osg::Uniform > m_viewportWidth;

    /**
     * Height of the viewport in pixels.
     */
    osg::ref_ptr< osg::Uniform > m_viewportHeight;

    /**
     * The viewer used to query the current reference cam.
     */
    boost::shared_ptr< WGEViewer > m_viewer;

    /**
     * The filename to load.
     */
    WPropFilename m_image;

    /**
     * Enable to blend out the overlay.
     */
    WPropBool m_blendOutAuto;

    /**
     * Time to blend out in seconds.
     */
    WPropDouble m_blendOutDuration;

    /**
     * Animate blend out sequence
     */
    osg::ref_ptr< WGEShaderAnimationCallback > m_animationCallback;

    /**
     * The texture.
     */
    osg::ref_ptr< osg::Texture2D > m_logoTexture;

    /**
     * If true, the update callback is forced to reload the image.
     */
    bool m_forceReload;

private:
    /**
     * Update the uniforms and textures if needed
     */
    class Updater: public osg::NodeCallback
    {
    public:
        /**
         * Called before draw on update
         *
         * \param node the node
         * \param nv the visitor calling this.
         */
        virtual void operator() ( osg::Node* node, osg::NodeVisitor* nv );
    };
    /**
     * Update callback for the viewport.
     */
    osg::ref_ptr< Updater > m_updater;
};

#endif  // WGEVIEWEREFFECTIMAGEOVERLAY_H

