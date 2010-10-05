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

#include "WGEColormapping.h"

// instance as singleton
boost::shared_ptr< WGEColormapping > WGEColormapping::m_instance = boost::shared_ptr< WGEColormapping >();

WGEColormapping::WGEColormapping()
{
    // initialize members
}

WGEColormapping::~WGEColormapping()
{
    // cleanup
}

boost::shared_ptr< WGEColormapping > WGEColormapping::instance()
{
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WGEColormapping >( new WGEColormapping() );
    }

    return m_instance;
}

void WGEColormapping::apply( osg::ref_ptr< osg::Node > node, bool useDefaultShader )
{
    instance()->applyInst( node, useDefaultShader );
}

void WGEColormapping::registerTexture( osg::ref_ptr< WGETexture3D > texture )
{
    instance()->registerTextureInst( texture );
}

void WGEColormapping::deregisterTexture( osg::ref_ptr< WGETexture3D > texture )
{
    instance()->deregisterTextureInst( texture );
}

void WGEColormapping::applyInst( osg::ref_ptr< osg::Node > node, bool useDefaultShader )
{
}

void WGEColormapping::registerTextureInst( osg::ref_ptr< WGETexture3D > texture )
{
    wlog::debug( "WGEColormapping" ) << "Registering texture.";
    if ( !m_textures.count( texture ) )
    {
        m_textures.push_back( texture );
    }
}

void WGEColormapping::deregisterTextureInst( osg::ref_ptr< WGETexture3D > texture )
{
    wlog::debug( "WGEColormapping" ) << "De-registering texture.";
    m_textures.remove( texture );
}

