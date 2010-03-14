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

#include <osg/Texture>
#include <osg/Texture2D>

#include "WGEOffscreen.h"

WGEOffscreen::WGEOffscreen( osg::ref_ptr< osg::Camera > reference, int num ):
    osg::Camera(),
    m_referenceCamera( reference ),
    m_fbo( new osg::FrameBufferObject() )
{
    // initialize members
    setClearColor( reference->getClearColor() );
    setClearMask( reference->getClearMask() );
    setReferenceFrame( osg::Transform::RELATIVE_RF );
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    setRenderOrder( osg::Camera::PRE_RENDER, num );
}

WGEOffscreen::~WGEOffscreen()
{
    // cleanup
}

void WGEOffscreen::attach( BufferComponent buffer, osg::ref_ptr< osg::Texture2D > texture )
{
    m_fbo->setAttachment( buffer, osg::FrameBufferAttachment( texture ) );

    osg::Camera::attach( buffer, texture );
}

osg::ref_ptr< osg::Texture2D > WGEOffscreen::attach( BufferComponent buffer )
{
    osg::ref_ptr< osg::Texture2D > tex = createTexture();
    attach( buffer, tex );
    return tex;
}

void WGEOffscreen::detach( BufferComponent buffer )
{
    m_fbo->setAttachment( buffer, osg::FrameBufferAttachment() );

    osg::Camera::detach( buffer );
}

osg::ref_ptr< osg::Texture2D > WGEOffscreen::createTexture( GLint internalFormat )
{
    osg::ref_ptr< osg::Texture2D > tex = new osg::Texture2D;
    tex->setTextureSize( m_referenceCamera->getViewport()->width(), m_referenceCamera->getViewport()->height() );
    tex->setInternalFormat( internalFormat );

    // setup interpolation
    tex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
    tex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

    // do repeat the texture
    tex->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    tex->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );

    return tex;
}

