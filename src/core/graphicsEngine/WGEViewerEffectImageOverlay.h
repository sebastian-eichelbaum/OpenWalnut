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

#include "WGEViewerEffect.h"

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

private:

    /**
     * Update the viewport uniforms
     */
    class ViewportUpdater: public osg::Camera::DrawCallback
    {
    public:
        /**
         * Called before draw.
         *
         * \param renderInfo current render state
         */
        virtual void operator() ( osg::RenderInfo& renderInfo ) const
        {
            m_viewportWidth->set( static_cast< float >( renderInfo.getState()->getCurrentViewport()->width() ) );
            m_viewportHeight->set( static_cast< float >( renderInfo.getState()->getCurrentViewport()->height() ) );
        }

        /**
        * Width of the viewport in pixels.
        */
        osg::ref_ptr< osg::Uniform > m_viewportWidth;

        /**
         * Height of the viewport in pixels.
        */
        osg::ref_ptr< osg::Uniform > m_viewportHeight;
    };
};

#endif  // WGEVIEWEREFFECTIMAGEOVERLAY_H

