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

#ifndef WQTGLSCREENCAPTURE_H
#define WQTGLSCREENCAPTURE_H

#include <limits>

#include <osg/Camera>
#include <osg/Image>
#include <osg/RenderInfo>

#include "../../graphicsEngine/WGEScreenCapture.h"

/**
 * This class is a screen recorder adapter in QT. It uses WGEScreenCapture and provides a nice widget around it.
 */
class WQtGLScreenCapture: public WGEScreenCapture
{
public:

    /**
     * Creates a screen capture callback.
     */
    WQtGLScreenCapture();

    /**
     * Destructor. Cleans up.
     */
    virtual ~WQtGLScreenCapture();

protected:

    /**
     * The function handles new images. Implement it.
     *
     * \param framesLeft how much frames to come
     * \param totalFrames the total number of frames until now
     * \param image the image
     */
    virtual void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const;

private:

};

#endif  // WQTGLSCREENCAPTURE_H
