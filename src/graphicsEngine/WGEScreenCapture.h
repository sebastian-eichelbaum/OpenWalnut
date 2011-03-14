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

#include <osg/Camera>
#include <osg/RenderInfo>
#include <osgDB/WriteFile>
#include <osg/GLObjects>
#include <osg/BufferObject>

#include "WExportWGE.h"

/**
 * \ingroup GE
 */
class WGE_EXPORT WGEScreenCapture: public osg::Camera::DrawCallback
{
public:
    WGEScreenCapture():
        m_record( false ),
        m_second( 0 ),
        m_frame( 0 )
    {
    }

    bool toggleRecord()
    {
        m_record = !m_record;
        return m_record;
    }

    virtual void operator () (osg::RenderInfo& renderInfo) const
    {
        if ( !m_record )
            return;

        m_frame++;
        if ( m_frame >= 24 )
        {
            m_frame = 0;
            m_second++;
        }

        std::stringstream filename;
        filename << "/home/ebaum/test_" << m_second << "-" << m_frame << ".jpg";
        std::cout << filename.str() << std::endl;


        osg::GraphicsContext* gc = renderInfo.getState()->getGraphicsContext();
        osg::BufferObject::Extensions* ext = osg::BufferObject::getExtensions(gc->getState()->getContextID(),true);

        glReadBuffer( GL_BACK );

    /*if (m_pbo==0)
    {
        ext->glGenBuffers(1, &pbo);
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
        ext->glBufferData(GL_PIXEL_PACK_BUFFER_ARB, image->getTotalSizeInBytes(), 0, GL_STREAM_READ);

        osg::notify(osg::NOTICE)<<"Generating pbo "<<pbo<<std::endl;
    }
    else
    {
        ext->glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, pbo);
    }
*/

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

        osg::Image* image = new osg::Image();
        image->readPixels( 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE );

        osgDB::writeImageFile( *image, filename.str() );
    }

    bool m_record;
    mutable size_t m_second;
    mutable size_t m_frame;
};

#endif  // WGESCREENCAPTURE_H
