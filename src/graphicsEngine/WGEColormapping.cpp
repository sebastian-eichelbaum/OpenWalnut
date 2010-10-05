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

#include "WGETextureUtils.h"

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
    if ( !m_instance )
    {
        m_instance = boost::shared_ptr< WGEColormapping >( new WGEColormapping() );
    }

    return m_instance;
}

void WGEColormapping::apply( osg::ref_ptr< osg::Node > node, bool useDefaultShader, size_t startTexUnit )
{
    instance()->applyInst( node, useDefaultShader, startTexUnit );
}

void WGEColormapping::registerTexture( osg::ref_ptr< WGETexture3D > texture )
{
    instance()->registerTextureInst( texture );
}

void WGEColormapping::deregisterTexture( osg::ref_ptr< WGETexture3D > texture )
{
    instance()->deregisterTextureInst( texture );
}

void WGEColormapping::applyInst( osg::ref_ptr< osg::Node > node, bool useDefaultShader, size_t startTexUnit )
{
    // applying to a node simply means adding a callback :-)
    NodeInfo info;
    info.m_initial = true;
    info.m_texUnitStart = startTexUnit;
    m_nodeInfo.insert( std::make_pair( node, info ) );
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
    NodeInfo info = r->get().find( node )->second;
    r.reset();

    //std::cout << "Hallo " << std::endl;

    // need (re-)binding?
    if ( info.m_initial || m_texUpdate )
    {
        TextureContainerType::ReadTicket rt = m_textures.getReadTicket();

        // bind each texture, provide all needed uniforms too
        size_t unit = info.m_texUnitStart;
        for( TextureContainerType::ConstIterator iter = rt->get().begin(); iter != rt->get().end(); ++iter )
        {
            wge::bindTexture( node, *iter, unit, "u_colormap" + boost::lexical_cast< std::string >( unit - info.m_texUnitStart ) );
            unit++;
        }

        rt.reset();
    }
}

