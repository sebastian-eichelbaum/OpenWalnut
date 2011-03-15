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

#include "WGEScreenCapture.h"

WGEScreenCapture::WGEScreenCapture():
    m_framesToRecord( 0 ),
    m_frame( 0 )
{
    // initialize
}

WGEScreenCapture::~WGEScreenCapture()
{
    // cleanup
}

void WGEScreenCapture::record( size_t frames )
{
    m_framesToRecord = frames;
}

void WGEScreenCapture::recordStop()
{
    m_framesToRecord = 0;
}

bool WGEScreenCapture::recordToggle()
{
    m_framesToRecord = m_framesToRecord ? 0 : std::numeric_limits< size_t >::max();
    return m_framesToRecord;
}

void WGEScreenCapture::screenshot()
{
    record( 1 );
}

bool WGEScreenCapture::isRecording()
{
    return m_framesToRecord;
}

void WGEScreenCapture::operator()( osg::RenderInfo& renderInfo ) const
{
    // is there something to record?
    if ( !m_framesToRecord )
    {
        return;
    }

    // we need the graphics context here.
    osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();

    // get size and color mode
    size_t width = 0;
    size_t height = 0;
    GLenum pixelFormat = GL_RGB;
    if (gc->getTraits())
    {
        width = gc->getTraits()->width;
        height = gc->getTraits()->height;
        pixelFormat = gc->getTraits()->alpha ? GL_RGBA : GL_RGB;
    }

    // count frames
    m_frame++;
    m_framesToRecord--;

    // read back buffer
    glReadBuffer( GL_BACK );

    /* The following code uses PBO to grab the framebuffer. This sometimes causes errors. I am not sure why. glReadPixels
    osg::ref_ptr< osg::Image > image = new osg::Image();
    osg::BufferObject::Extensions* ext = osg::BufferObject::getExtensions( gc->getState()->getContextID(), true );
    image->allocateImage( width, height, 1, pixelFormat, GL_UNSIGNED_BYTE );
    if (m_pbo==0)
    {
        std::cout << "CREATE" << std::endl;
        ext->glGenBuffers( 1, &m_pbo );
        ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, m_pbo );
        ext->glBufferData( GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ );
    }
    else
    {
        std::cout << "BIND" << std::endl;
        ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, m_pbo );
    }
    std::cout << "READ" << std::endl;
    glReadPixels( 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE, 0 );
    std::cout << "MAP" << std::endl;
    GLubyte* src = static_cast< GLubyte* >( ext->glMapBuffer( GL_PIXEL_PACK_BUFFER_ARB, GL_READ_ONLY_ARB ) );
    if(src)
    {
        std::cout << "COPY" << std::endl;
        memcpy( image->data(), src, image->getTotalSizeInBytes() );
        ext->glUnmapBuffer( GL_PIXEL_PACK_BUFFER_ARB );
    }

    std::cout << "RESTORE" << std::endl;
    ext->glBindBuffer( GL_PIXEL_PACK_BUFFER_ARB, 0 );
    */

    // create image and read pixels into it
    osg::ref_ptr< osg::Image > image = new osg::Image();
    image->readPixels( 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE );

    // someone wants this image
    handleImage( m_framesToRecord, m_frame, image );
}

