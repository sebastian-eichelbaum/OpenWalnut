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

#include <boost/signals2.hpp>
#include <boost/function.hpp>

#include <osg/Camera>
#include <osg/Image>
#include <osg/RenderInfo>

#include "../common/WSharedObject.h"
#include "../common/WCondition.h"

#include "animation/WGEAnimationFrameTimer.h"

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
     * Keeps track of several frame-counts.
     */
    typedef struct
    {
        size_t m_frames;        //!< current number of frames.
        size_t m_framesLeft;    //!< the frames to take until stop.
    }
    RecordingInformation;

    /**
     * The shared access type to the FrameCounting struct.
     */
    typedef WSharedObject< RecordingInformation > SharedRecordingInformation;

    /**
     * Convenience typedef
     */
    typedef osg::ref_ptr< WGEScreenCapture > RefPtr;

    /**
     * Convenience typedef
     */
    typedef osg::ref_ptr< const WGEScreenCapture > ConstRefPtr;

    /**
     * This callback signature is needed to subscribe to the handleImage signal.
     */
    typedef boost::function< void( size_t, size_t, osg::ref_ptr< osg::Image > ) > HandleImageCallbackType;

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
     * Resets the frame-counter to 0.
     */
    void resetFrameCounter();

    /**
     * The draw callback operator. Gets called by OSG in draw traversal.
     *
     * \param renderInfo the OSG renderinfo
     */
    virtual void operator()( osg::RenderInfo& renderInfo ) const;   // NOLINT - osg wants this to be a non-const reference

    /**
     * The condition returned here is actually the change condition of the frame counter. This can be used to update GUI or something as it
     * contains frame-counts, recording information and so on (updated per frame).
     *
     * \return the condition
     */
    WCondition::ConstSPtr getRecordCondition() const;

    /**
     * Returns the current recording information. Release the lock after you grabbed the info you need.
     *
     * \return the info struct - read ticket
     */
    SharedRecordingInformation::ReadTicket getRecordingInformation() const;

    /**
     * Returns a timer getting ticked on each recorded frame. This can then be used for animations for example.
     *
     * \return the timer.
     */
    WGEAnimationFrameTimer::ConstSPtr getFrameTimer() const;

    /**
     * Subscribes a specified function to the new-image-signal. This gets emitted every time a new image was grabbed.
     *
     * \param callback the callback
     *
     * \return the connection.
     */
    boost::signals2::connection subscribeSignal( HandleImageCallbackType callback );

protected:

    /**
     * The function handles new images. Implement it.
     *
     * \param framesLeft how much frames to come
     * \param totalFrames the total number of frames until now
     * \param image the image
     */
    virtual void handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const;

    /**
     * Starts recording. If already recording, it continues recording.
     *
     * \param frames the number of frames to record. 0 means stop, 1 is a single screenshot.
     */
    void record( size_t frames = std::numeric_limits< size_t >::max() );

private:

    /**
     * Counts the frames to take.
     */
    SharedRecordingInformation m_recordingInformation;

    /**
     * The frame timer. Ticket on each recorded frame.
     */
    WGEAnimationFrameTimer::SPtr m_timer;

    /**
     * The type of the signal for handleSignal.
     */
    typedef boost::signals2::signal< void( size_t, size_t, osg::ref_ptr< osg::Image > ) > HandleImageSignalType;

    /**
     * The signal emitted on every new grabbed image.
     */
    HandleImageSignalType m_signalHandleImage;
};

#endif  // WGESCREENCAPTURE_H
