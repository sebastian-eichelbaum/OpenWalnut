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

#include <string>


#include <osg/Texture>
#include <osg/Texture2D>

#include "core/common/WLogger.h"
#include "../WGETextureHud.h"

#include "WGEOffscreenRenderPass.h"

WGEOffscreenRenderPass::WGEOffscreenRenderPass( size_t textureWidth, size_t textureHeight, int num ):
    osg::Camera(),
    m_width( textureWidth ),
    m_height( textureHeight ),
    m_fbo( new osg::FrameBufferObject() ),
    m_hud( NULL )
{
    // initialize members
    setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 0.0 ) );
    setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    setReferenceFrame( osg::Transform::RELATIVE_RF );
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    setRenderOrder( osg::Camera::PRE_RENDER, num );
}

WGEOffscreenRenderPass::WGEOffscreenRenderPass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name,
                                                int num ):
    osg::Camera(),
    m_width( textureWidth ),
    m_height( textureHeight ),
    m_fbo( new osg::FrameBufferObject() ),
    m_hud( hud ),
    m_name( name )
{
    // initialize members
    setClearColor( osg::Vec4( 0.0, 0.0, 0.0, 0.0 ) );
    setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    setReferenceFrame( osg::Transform::RELATIVE_RF );
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER_OBJECT );
    setRenderOrder( osg::Camera::PRE_RENDER, num );
}

WGEOffscreenRenderPass::~WGEOffscreenRenderPass()
{
    // cleanup
}

void WGEOffscreenRenderPass::attach( BufferComponent buffer, osg::ref_ptr< osg::Texture2D > texture )
{
    m_fbo->setAttachment( buffer, osg::FrameBufferAttachment( texture ) );

    if( m_hud )
    {
        m_hud->addTexture( new WGETextureHud::WGETextureHudEntry( texture, m_name + " - " + getBufferName( buffer ) ) );
    }

    osg::Camera::attach( buffer, texture );
}

void WGEOffscreenRenderPass::attach( BufferComponent buffer, osg::ref_ptr< osg::Image > image )
{
    osg::Camera::attach( buffer, image );
}

osg::ref_ptr< osg::Texture2D > WGEOffscreenRenderPass::attach( BufferComponent buffer, GLint internalFormat )
{
    osg::ref_ptr< osg::Texture2D > tex;
    if( buffer == DEPTH_BUFFER )   // depth buffers need a special texture type (else: FBO status = 0x8cd6 (FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT))
    {
        tex = createTexture( GL_DEPTH_COMPONENT );
    }
    else
    {
#if defined(__APPLE__)
        if( internalFormat != GL_RGBA )
        {
            wlog::warn( "WGEOffscreenRenderPass::attach:" ) <<
                "Changing internal format to GL_RGBA because the original format is not supported on Mac OSX.";
        }
        tex = createTexture( GL_RGBA );           // on MacOS X, only RGBA textures work as attachment for FBO's
#else
        tex = createTexture( internalFormat );
#endif
    }
    attach( buffer, tex );
    return tex;
}

void WGEOffscreenRenderPass::detach( BufferComponent buffer )
{
    // remove the texture from HUD if existing
    if( m_hud && osg::Camera::getBufferAttachmentMap().count( buffer ) )
    {
        m_hud->removeTexture( osg::Camera::getBufferAttachmentMap()[ buffer ]._texture );
    }

    m_fbo->setAttachment( buffer, osg::FrameBufferAttachment() );

    osg::Camera::detach( buffer );
}

osg::ref_ptr< osg::Texture2D > WGEOffscreenRenderPass::createTexture( GLint internalFormat )
{
    osg::ref_ptr< osg::Texture2D > tex = new osg::Texture2D;
    tex->setTextureSize( m_width, m_height );
    tex->setInternalFormat( internalFormat );

    // setup interpolation
    tex->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR );
    tex->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );

    // do repeat the texture
    tex->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
    tex->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );

    return tex;
}

std::string WGEOffscreenRenderPass::getName() const
{
    return m_name;
}

size_t WGEOffscreenRenderPass::getTextureWidth() const
{
    return m_width;
}

size_t WGEOffscreenRenderPass::getTextureHeight() const
{
    return m_height;
}

void WGEOffscreenRenderPass::addUniform( osg::ref_ptr< osg::Uniform > uniform )
{
    this->getOrCreateStateSet()->addUniform( uniform );
}

std::string WGEOffscreenRenderPass::getBufferName( BufferComponent buffer )
{
    switch( buffer )
    {
        case DEPTH_BUFFER:
            return "Depth";
        case STENCIL_BUFFER:
            return "Stencil";
        case PACKED_DEPTH_STENCIL_BUFFER:
            return "Depth+Stencil";
        case COLOR_BUFFER:
            return "Color 0";
        case COLOR_BUFFER0:
            return "Color 0";
        case COLOR_BUFFER1:
            return "Color 1";
        case COLOR_BUFFER2:
            return "Color 2";
        case COLOR_BUFFER3:
            return "Color 3";
        case COLOR_BUFFER4:
            return "Color 4";
        case COLOR_BUFFER5:
            return "Color 5";
        case COLOR_BUFFER6:
            return "Color 6";
        case COLOR_BUFFER7:
            return "Color 7";
        case COLOR_BUFFER8:
            return "Color 8";
        case COLOR_BUFFER9:
            return "Color 9";
        case COLOR_BUFFER10:
            return "Color 10";
        case COLOR_BUFFER11:
            return "Color 11";
        case COLOR_BUFFER12:
            return "Color 12";
        case COLOR_BUFFER13:
            return "Color 13";
        case COLOR_BUFFER14:
            return "Color 14";
        case COLOR_BUFFER15:
            return "Color 15";
        default:
            return "Unknown";
    }
}

