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

#include <osg/BufferObject>

#include "animation/WGEAnimationFrameTimer.h"

#include "WGEScreenCapture.h"

WGEScreenCapture::WGEScreenCapture():
    m_timer( new WGEAnimationFrameTimer() )
{
    // initialize
    SharedRecordingInformation::WriteTicket w = m_recordingInformation.getWriteTicket();
    w->get().m_frames = 0;
    w->get().m_framesLeft = 0;
}

WGEScreenCapture::~WGEScreenCapture()
{
    // cleanup
}

WGEAnimationFrameTimer::ConstSPtr WGEScreenCapture::getFrameTimer() const
{
    return m_timer;
}

void WGEScreenCapture::recordStart()
{
    record();
}

void WGEScreenCapture::recordStop()
{
    record( 0 );
}

void WGEScreenCapture::screenshot()
{
    record( 1 );
}

void WGEScreenCapture::record( size_t frames )
{
    SharedRecordingInformation::WriteTicket w = m_recordingInformation.getWriteTicket();
    w->get().m_framesLeft = frames;
}

bool WGEScreenCapture::isRecording()
{
    SharedRecordingInformation::ReadTicket r = m_recordingInformation.getReadTicket();
    return r->get().m_framesLeft;
}

void WGEScreenCapture::operator()( osg::RenderInfo& renderInfo ) const  // NOLINT - osg wants this to be a non-const reference
{
    SharedRecordingInformation::WriteTicket w = m_recordingInformation.getWriteTicket();
    RecordingInformation& frameCounter = w->get();

    // is there something to record?
    if( !frameCounter.m_framesLeft )
    {
        return;
    }

    // we need the graphics context here.
    osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();

    // get size and color mode
    size_t width = 0;
    size_t height = 0;
    GLenum pixelFormat = GL_RGB;
    if( gc->getTraits())
    {
        width = gc->getTraits()->width;
        height = gc->getTraits()->height;
        pixelFormat = gc->getTraits()->alpha ? GL_RGBA : GL_RGB;
    }

    // count frames
    frameCounter.m_frames++;
    frameCounter.m_framesLeft--;

    // read back buffer
#ifndef GL_ES_VERSION_2_0
    glReadBuffer( GL_BACK );
#endif

    // The following code uses PBO to grab the framebuffer. This sometimes causes errors. I am not sure why. glReadPixels
    // osg::ref_ptr< osg::Image > imagePBO = new osg::Image();
    // osg::BufferObject::Extensions* ext = osg::BufferObject::getExtensions( gc->getState()->getContextID(), true );
    // imagePBO->allocateImage( width, height, 1, pixelFormat, GL_UNSIGNED_BYTE );
    // if (m_pbo==0)
    // {
    //     std::cout << "CREATE" << std::endl;
    //     ext->glGenBuffers( 1, &m_pbo );
    //     ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, m_pbo );
    //     ext->glBufferData( GL_PIXEL_PACK_BUFFER_ARB, imagePBO->getTotalSizeInBytes(), 0, GL_STREAM_READ );
    // }
    // else
    // {
    //     std::cout << "BIND" << std::endl;
    //     ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, m_pbo );
    // }
    // std::cout << "READ" << std::endl;
    // glReadPixels( 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE, 0 );
    // std::cout << "MAP" << std::endl;
    // GLubyte* src = static_cast< GLubyte* >( ext->glMapBuffer( GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB ) );
    // if(src)
    // {
    //     std::cout << "COPY" << std::endl;
    //     memcpy( imagePBO->data(), src, imagePBO->getTotalSizeInBytes() );
    //     ext->glUnmapBuffer( GL_PIXEL_PACK_BUFFER_ARB );
    //}
    //
    // std::cout << "RESTORE" << std::endl;
    // ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, 0 );

    // create image and read pixels into it
    osg::ref_ptr< osg::Image > image = new osg::Image();
    image->readPixels( 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE );

    // another frame.
    m_timer->tick();

    // someone wants this image
    handleImage( frameCounter.m_framesLeft, frameCounter.m_frames, image );
}

WCondition::ConstSPtr WGEScreenCapture::getRecordCondition() const
{
    return m_recordingInformation.getChangeCondition();
}

WGEScreenCapture::SharedRecordingInformation::ReadTicket WGEScreenCapture::getRecordingInformation() const
{
    return m_recordingInformation.getReadTicket();
}

void WGEScreenCapture::resetFrameCounter()
{
    SharedRecordingInformation::WriteTicket w = m_recordingInformation.getWriteTicket();
    w->get().m_frames = 0;
    m_timer->reset();
}

void WGEScreenCapture::handleImage( size_t framesLeft, size_t totalFrames, osg::ref_ptr< osg::Image > image ) const
{
    m_signalHandleImage( framesLeft, totalFrames, image );
}

boost::signals2::connection WGEScreenCapture::subscribeSignal( HandleImageCallbackType callback )
{
    return m_signalHandleImage.connect( callback );
}

