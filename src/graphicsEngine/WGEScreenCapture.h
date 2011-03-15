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

#ifndef WGESCREENCAPTURE_H
#define WGESCREENCAPTURE_H

#include <limits>

#include <osg/Camera>
#include <osg/Image>
#include <osg/RenderInfo>

#include "../common/WSharedObject.h"
#include "../common/WCondition.h"

#include "WExportWGE.h"

/**
 * This class is a screen recorder. It records the frame buffer to files on a per-frame-basis. This class is NOT thread-safe due to performance
 * reasons. You should not distribute the instance among multiple threads. It can be applied to <b>ONE</b> camera only by setting it as
 * finalDrawCallback (osg::Camera::setFinalDrawCallback). Each camera can only use ONE final draw callback.
 *
 * This class is abstract. Derive your own class and handle image writing.
 *
 * \note This class does NOT write the images to disk. Set a callback for this.
 *
 * \ingroup GE
 */
class WGE_EXPORT WGEScreenCapture: public osg::Camera::DrawCallback
{
public:

    /**
     * Creates a screen capture callback.
     */
    WGEScreenCapture();

    /**
     * Destructor. Cleans up.
     */
    virtual ~WGEScreenCapture();

    /**
     * Starts recording. If it already is running, nothing happens.
     */
    void recordStart();

    /**
     * Stops recording. If not recording, nothing happens.
     */
    void recordStop();

    /**
     * Checks if there are frames left for recording.
     *
     * \return true if yes.
     */
    bool isRecording();

    /**
     * Makes a screenshot with the <b>next</b> frame. This is a shortcut for record( 1 ).
     */
    void screenshot();

    /**
     * The draw callback operator. Gets called by OSG in draw traversal.
     *
     * \param renderInfo the OSG renderinfo
     */
    virtual void operator()( osg::RenderInfo& renderInfo ) const;

    /**
     * Returns the condition which fires if the recording is started.
     *
     * \return the condition.
     */
    WCondition::ConstSPtr getRecordStartCondition() const;

    /**
     * Returns the condition which fires if the recording is finished.
     *
     * \return the condition.
     */
    WCondition::ConstSPtr getRecordStopCondition() const;

protected:

    /**
     * The function handles new images. Implement it.
     *
     * \param framesLeft how much frames to come
     * \param totalFrames the total number of frames until now
     * \param image the image
     */
    virtual void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const = 0;

    /**
     * Starts recording. If already recording, it continues recording.
     *
     * \param frames the number of frames to record. 0 means stop, 1 is a single screenshot.
     */
    void record( size_t frames = std::numeric_limits< size_t >::max() );

private:

    /**
     * Frame counter. Incremented each frame. Must be mutable to allow the const operator() to change it.
     */
    mutable size_t m_frame;

    /**
     * Counts the frames to take.
     */
    WSharedObject< size_t > m_framesLeft;

    /**
     * The condition is fired if recording gets started.
     */
    WCondition::SPtr m_recordStartCondition;

    /**
     * The condition is fired if recording is stopped or done.
     */
    WCondition::SPtr m_recordStopCondition;
};

#endif  // WGESCREENCAPTURE_H
