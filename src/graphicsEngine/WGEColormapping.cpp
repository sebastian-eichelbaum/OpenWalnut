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

WGEColormapping::WGEColormapping():
    m_callback( new WGEFunctorCallback< osg::Node >( boost::bind( &WGEColormapping::callback, this, _1 ) ) ),
    m_texUpdate( true )
{
    // initialize members
    m_textures.getChangeCondition()->subscribeSignal( boost::bind( &WGEColormapping::textureUpdate, this ) );
}

WGEColormapping::~WGEColormapping()
{
    // cleanup
}

boost::shared_ptr< WGEColormapping > WGEColormapping::instance()
{
    if ( !m_instance.get() )
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
    // applying to a node simply means adding a callback :-)
    node->addUpdateCallback( m_callback );
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

void WGEColormapping::textureUpdate()
{
    m_texUpdate = true;
}

void WGEColormapping::callback( osg::Node* node )
{
    // get node info
    NodeInfoContainerType::ReadTicket r = m_nodeInfo.getReadTicket();
    // what if find is the last element in the map? then there wont be a second
    std::map< osg::Node*, NodeInfo >::const_iterator found = r->get().find( node );

    NodeInfo info;

    if ( found != r->get().end() )
    {
        info = found->second;

        // need (re-)binding?
        if ( info.m_initial || m_texUpdate )
        {
            TextureContainerType::ReadTicket rt = m_textures.getReadTicket();
            // implement
            rt.reset();
        }
    }
    r.reset();
}

