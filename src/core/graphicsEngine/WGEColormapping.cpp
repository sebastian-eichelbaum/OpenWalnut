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

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "../common/WLogger.h"
#include "../common/WStringUtils.h"

#include "WGETextureUtils.h"
#include "exceptions/WGESignalSubscriptionFailed.h"

#include "WGEColormapping.h"

// instance as singleton
boost::shared_ptr< WGEColormapping > WGEColormapping::m_instance = boost::shared_ptr< WGEColormapping >();

/**
 * This functions simply sets some defines to a shader. It sets the texture unit and gl_MultiTexCoord variable names properly.
 *
 * \param shader the shader where to add the defines
 * \param start the start index of the unit for colormap0
 */
void setDefines( osg::ref_ptr< WGEShader > shader, size_t start = 0 )
{
    // simply set some proper defines for each colormap -> the unit and multitex coords
    for( size_t unit = 0; unit < wge::getMaxTexUnits(); ++unit )
    {
        // disable textures with invalid unit numbers
        if( unit < wge::getMaxTexUnits() - start )
        {
            shader->setDefine( "Colormap" + string_utils::toString( unit ) + "Enabled", true );
            shader->setDefine( "Colormap" + string_utils::toString( unit ) + "Unit", start + unit );
        }
    }
}

/**
 * This functions simply sets the specified pre transformation matrix to the shader. It therefore uses a preprocessor define. This allows a
 * hard-coded matrix to be optimized be the shader compiler.
 *
 * \param shader the shader where to add the defines
 * \param preTransform the transformation matrix used to pre-multiply with all texture coordinates
 */
void setPreTransform( osg::ref_ptr< WGEShader > shader, osg::Matrixd preTransform )
{
    std::ostringstream out;
    out << "mat4( ";
    const osg::Matrixd::value_type* m = preTransform.ptr();

    out.precision( 10 );
    out.setf( std::ios::fixed, std::ios::floatfield );

    // print all 16 values
    for( size_t i = 0; i < 15; ++i )
    {
        out << m[ i ] << ", ";
    }
    out << m[ 15 ] << " )";

    // set as define
    shader->setDefine( "ColormapPreTransform", out.str() );
}

WGEColormapping::WGEColormapping():
    m_callback( new WGEFunctorCallback< osg::Node >( boost::bind( &WGEColormapping::callback, this, _1 ) ) )
{
    // initialize members
    m_textures.getChangeCondition()->subscribeSignal( boost::bind( &WGEColormapping::textureUpdate, this ) );
    m_boundingBox.getWriteTicket()->get().set( 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 );
}

WGEColormapping::~WGEColormapping()
{
    // cleanup
}

boost::shared_ptr< WGEColormapping > WGEColormapping::instance()
{
    if( !m_instance )
    {
        m_instance = boost::shared_ptr< WGEColormapping >( new WGEColormapping() );
    }

    return m_instance;
}

void WGEColormapping::apply( osg::ref_ptr< osg::Node > node, osg::ref_ptr< WGEShader > shader, size_t startTexUnit )
{
    instance()->applyInst( NodeList( 1, node ), WMatrix4d( WMatrix4d::identity() ), shader, startTexUnit );
}

void WGEColormapping::apply( osg::ref_ptr< osg::Node > node, WMatrix4d preTransform, osg::ref_ptr< WGEShader > shader,
                             size_t startTexUnit )
{
    instance()->applyInst( NodeList( 1, node ), preTransform, shader, startTexUnit );
}

void WGEColormapping::apply( NodeList nodes, WMatrix4d preTransform, osg::ref_ptr< WGEShader > shader, size_t startTexUnit )
{
    instance()->applyInst( nodes, preTransform, shader, startTexUnit );
}

void WGEColormapping::apply( NodeList nodes, osg::ref_ptr< WGEShader > shader, size_t startTexUnit )
{
    instance()->applyInst( nodes, WMatrix4d( WMatrix4d::identity() ), shader, startTexUnit );
}

void WGEColormapping::registerTexture( osg::ref_ptr< WGETexture3D > texture, std::string name )
{
    instance()->registerTextureInst( texture, name );
}

void WGEColormapping::deregisterTexture( osg::ref_ptr< WGETexture3D > texture )
{
    instance()->deregisterTextureInst( texture );
}

void WGEColormapping::replaceTexture( osg::ref_ptr< WGETexture3D > old, osg::ref_ptr< WGETexture3D > newTex, std::string name )
{
    instance()->replaceTextureInst( old, newTex, name );
}

void WGEColormapping::applyInst( NodeList nodes, WMatrix4d preTransform, osg::ref_ptr< WGEShader > shader,
                                 size_t startTexUnit )
{
    // init shader
    osg::ref_ptr< WGEShader > s = shader;
    if( !s )
    {
        // we use a new instance of the default shader here because the preTransform is varying between several nodes.
        s = new WGEShader( "WGEDefaultColormapper" );
    }
    setDefines( s, startTexUnit );
    setPreTransform( s, preTransform );

    // do this for each node
    for( NodeList::const_iterator i = nodes.begin(); i != nodes.end(); ++i )
    {
        // applying to a node simply means adding a callback :-)
        NodeInfo* info = new NodeInfo;
        info->m_rebind = true;
        info->m_texUnitStart = startTexUnit;
        info->m_preTransform = preTransform;
        m_nodeInfo.insert( std::make_pair( *i, info ) );

        ( *i )->addUpdateCallback( m_callback );

        // add the default shader if no other shader has been specified.
        s->apply( *i );
    }
}

void WGEColormapping::registerTextureInst( osg::ref_ptr< WGETexture3D > texture, std::string name )
{
    wlog::debug( "WGEColormapping" ) << "Registering texture.";
    if( !m_textures.count( texture ) )
    {
        if( !name.empty() )
        {
            texture->name()->set( name );
        }
        m_textures.push_front( texture );
        updateBounds();
        m_registerSignal( texture );
    }
}

void WGEColormapping::deregisterTextureInst( osg::ref_ptr< WGETexture3D > texture )
{
    wlog::debug( "WGEColormapping" ) << "De-registering texture.";
    if( m_textures.count( texture ) )
    {
        m_textures.remove( texture );
        updateBounds();
        m_deregisterSignal( texture );
    }
}

void WGEColormapping::replaceTextureInst( osg::ref_ptr< WGETexture3D > old, osg::ref_ptr< WGETexture3D > newTex, std::string name )
{
    wlog::debug( "WGEColormapping" ) << "Replacing texture.";
    if( !name.empty() )
    {
        newTex->name()->set( name );
    }

    // if it exists, replace it
    if( m_textures.count( old ) )
    {
        m_textures.replace( old, newTex );
        updateBounds();
        m_replaceSignal( old, newTex );
    }
    else    // <- if not exists: add
    {
        registerTextureInst( newTex, name );
    }
}

void WGEColormapping::updateBounds()
{
    TextureContainerType::ReadTicket r = m_textures.getReadTicket();
    WSharedObject< WBoundingBox >::WriteTicket bbw = m_boundingBox.getWriteTicket();

    bool first = true;
    for( TextureContainerType::ConstIterator iter = r->get().begin(); iter != r->get().end(); ++iter )
    {
        if( first )
        {
            bbw->get() = ( *iter )->getBoundingBox();
            first = false;
        }
        else
        {
            bbw->get().expandBy( ( *iter )->getBoundingBox() );
        }
    }
}

WBoundingBox WGEColormapping::getBoundingBox() const
{
    return m_boundingBox.getReadTicket()->get();
}

void WGEColormapping::textureUpdate()
{
    NodeInfoContainerType::WriteTicket w = m_nodeInfo.getWriteTicket();
    for( NodeInfoContainerType::Iterator iter = w->get().begin(); iter != w->get().end(); ++iter )
    {
        iter->second->m_rebind = true;
    }
}

void WGEColormapping::callback( osg::Node* node )
{
    // get node info
    NodeInfoContainerType::ReadTicket r = m_nodeInfo.getReadTicket();
    NodeInfoContainerType::ConstIterator infoItem = r->get().find( node );
    if( infoItem == r->get().end() )
    {
        return;
    }
    r.reset();

    NodeInfo* info = infoItem->second;

    // need (re-)binding?
    if( info->m_rebind )
    {
        info->m_rebind = false;

        size_t maxTexUnits = wge::getMaxTexUnits();
        wge::unbindTexture( node, info->m_texUnitStart, maxTexUnits - info->m_texUnitStart );

        TextureContainerType::ReadTicket rt = m_textures.getReadTicket();

        // bind each texture, provide all needed uniforms too
        size_t unit = info->m_texUnitStart;
        for( TextureContainerType::ConstIterator iter = rt->get().begin();
             ( unit < maxTexUnits ) && ( iter != rt->get().end() );
             ++iter )
        {
            wge::bindTexture( node, *iter, unit, "u_colormap" + string_utils::toString( unit - info->m_texUnitStart ) );
            unit++;
        }

        rt.reset();
    }
}

bool WGEColormapping::moveDown( osg::ref_ptr< WGETexture3D > texture )
{
    TextureContainerType::WriteTicket w = m_textures.getWriteTicket();

    // does the texture exist?
    TextureContainerType::Iterator iter = std::find( w->get().begin(), w->get().end(), texture );
    if( iter == w->get().end() )
    {
        return false;
    }

    // is it already the last item?
    if( iter + 1 == w->get().end() )
    {
        return false;
    }

    // swap items
    std::iter_swap( iter, iter + 1 );

    // unlock and call callbacks
    w.reset();
    m_sortSignal();

    return true;
}

bool WGEColormapping::moveUp( osg::ref_ptr< WGETexture3D > texture )
{
    TextureContainerType::WriteTicket w = m_textures.getWriteTicket();

    // does the texture exist?
    TextureContainerType::Iterator iter = std::find( w->get().begin(), w->get().end(), texture );
    if( iter == w->get().end() )
    {
        return false;
    }

    // is it already the first item?
    if( iter == w->get().begin() )
    {
        return false;
    }

    // swap items
    std::iter_swap( iter, iter - 1 );

    // unlock and call callbacks
    w.reset();
    m_sortSignal();

    return true;
}

size_t WGEColormapping::size() const
{
    return m_textures.size();
}

boost::signals2::connection WGEColormapping::subscribeSignal( TextureListSignal signal, TextureRegisterHandler notifier )
{
    switch( signal )
    {
        case Registered:
            return m_registerSignal.connect( notifier );
        case Deregistered:
            return m_deregisterSignal.connect( notifier );
        default:
            throw new WGESignalSubscriptionFailed( std::string( "Could not register TextureRegisterHandler to sort signal." ) );
    }
}

boost::signals2::connection WGEColormapping::subscribeSignal( TextureListSignal signal, TextureReplaceHandler notifier )
{
    switch( signal )
    {
        case Replaced:
            return m_replaceSignal.connect( notifier );
        default:
            throw new WGESignalSubscriptionFailed( std::string( "Could not register TextureReplaceHandler to signal." ) );
    }
}

boost::signals2::connection WGEColormapping::subscribeSignal( TextureListSignal signal, TextureSortHandler notifier )
{
    switch( signal )
    {
        case Sorted:
            return m_sortSignal.connect( notifier );
        default:
            throw new WGESignalSubscriptionFailed( std::string( "Could not register TextureSortHandler to register/deregister signal." ) );
    }
}

WGEColormapping::TextureContainerType::ReadTicket WGEColormapping::getReadTicket()
{
    return m_textures.getReadTicket();
}

WCondition::SPtr WGEColormapping::getChangeCondition() const
{
    return m_textures.getChangeCondition();
}

