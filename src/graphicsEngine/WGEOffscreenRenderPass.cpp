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

#include <boost/lexical_cast.hpp>

#include <osg/Texture>
#include <osg/Texture2D>

#include "WGETextureHud.h"

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

    if ( m_hud )
    {
        m_hud->addTexture( new WGETextureHud::WGETextureHudEntry( texture, m_name + " " + boost::lexical_cast< std::string >( buffer ) ) );
    }

    osg::Camera::attach( buffer, texture );
}

osg::ref_ptr< osg::Texture2D > WGEOffscreenRenderPass::attach( BufferComponent buffer )
{
    osg::ref_ptr< osg::Texture2D > tex;
    if ( buffer == DEPTH_BUFFER )   // depth buffers need a special texture type (else: FBO status = 0x8cd6 (FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT))
    {
        tex = createTexture( GL_DEPTH_COMPONENT );
    }
    else
    {
        tex = createTexture();
    }
    attach( buffer, tex );
    return tex;
}

void WGEOffscreenRenderPass::detach( BufferComponent buffer )
{
    // remove the texture from hud if existing
    if ( m_hud && osg::Camera::getBufferAttachmentMap().count( buffer ) )
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

