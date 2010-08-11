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

#include <iostream>

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/TexEnv>

#include "WGETextureHud.h"

WGETextureHud::WGETextureHud():
    osg::Projection(),
    m_group( new WGEGroupNode ),
    m_maxElementWidth( 256 )
{
    getOrCreateStateSet()->setRenderBinDetails( 1000, "RenderBin" );
    m_group->addUpdateCallback( new SafeUpdateCallback( this ) );
    addChild( m_group );
}

WGETextureHud::~WGETextureHud()
{
    // cleanup
}

void WGETextureHud::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // TODO(ebaum): all those values (viewport, size of textures) are hardcoded. This is ugly but works for now.
    // TODO(ebaum): use shader to selectively render channels ( if one only wants to see the alpha channel for example).

    // set the new size of the widget (how can we get this data?)
    unsigned int screenWidth = 1024;
    unsigned int screenHeight = 768;
    m_hud->setMatrix( osg::Matrix::ortho2D( 0, screenWidth, 0, screenHeight ) );

    // border around each element
    unsigned int border = 5;

    // update all those
    osg::Group* group = static_cast< osg::Group* >( node );

    unsigned int nextX = border;
    unsigned int nextY = border;

    // iterate all children
    for( size_t i = 0; i < group->getNumChildren(); ++i )
    {
        // all children are WGETextureHudEntries.
        WGETextureHudEntry* tex = static_cast< WGETextureHudEntry* >( group->getChild( i ) );

        // scale the height of the quad (texture) to have proper aspect ratio
        double height = m_hud->getMaxElementWidth() * tex->getRealHeight() / tex->getRealWidth();

        // scale them to their final size
        osg::Matrixd scale = osg::Matrixd::scale( m_hud->getMaxElementWidth(), height, 1.0 );

        // need to add a "linebreak"?
        if ( nextY + height + border > screenHeight )
        {
            nextX += m_hud->getMaxElementWidth() + border;
            nextY = border;
        }

        // transform them to the right place
        osg::Matrixd translate = osg::Matrixd::translate( static_cast< double >( nextX ), static_cast< double >( nextY ), 0.0 );
        tex->setMatrix( scale * translate );

        // calc the y position of the next texture
        nextY += height + border;
    }

    // update all the others
    traverse( node, nv );
}

void WGETextureHud::addTexture( osg::ref_ptr< WGETextureHudEntry > texture )
{
    m_group->insert( texture );
}

void WGETextureHud::removeTexture( osg::ref_ptr< WGETextureHudEntry > texture )
{
    m_group->remove( texture );
}

WGETextureHud::WGETextureHudEntry::WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture, bool transparency ):
    osg::MatrixTransform(),
    m_texture( texture )
{
    setMatrix( osg::Matrixd::identity() );
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    osg::Geode* geode = new osg::Geode();

    // Set up geometry for the HUD and add it to the HUD
    osg::ref_ptr< osg::Geometry > HUDBackgroundGeometry = new osg::Geometry();

    osg::ref_ptr< osg::Vec3Array > HUDBackgroundVertices = new osg::Vec3Array;
    HUDBackgroundVertices->push_back( osg::Vec3( 0, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1, 0, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 1, 1, -1 ) );
    HUDBackgroundVertices->push_back( osg::Vec3( 0, 1, -1 ) );

    osg::ref_ptr< osg::Vec3Array > HUDBackgroundTex = new osg::Vec3Array;
    HUDBackgroundTex->push_back( osg::Vec3( 0, 0, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 1, 0, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 1, 1, 0 ) );
    HUDBackgroundTex->push_back( osg::Vec3( 0, 1, 0 ) );

    osg::ref_ptr< osg::DrawElementsUInt > HUDBackgroundIndices = new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
    HUDBackgroundIndices->push_back( 0 );
    HUDBackgroundIndices->push_back( 1 );
    HUDBackgroundIndices->push_back( 2 );
    HUDBackgroundIndices->push_back( 3 );

    osg::ref_ptr< osg::Vec4Array > HUDcolors = new osg::Vec4Array;
    HUDcolors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    osg::ref_ptr< osg::Vec3Array > HUDnormals = new osg::Vec3Array;
    HUDnormals->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );
    HUDBackgroundGeometry->setNormalArray( HUDnormals );
    HUDBackgroundGeometry->setNormalBinding( osg::Geometry::BIND_OVERALL );
    HUDBackgroundGeometry->addPrimitiveSet( HUDBackgroundIndices );
    HUDBackgroundGeometry->setVertexArray( HUDBackgroundVertices );
    HUDBackgroundGeometry->setColorArray( HUDcolors );
    HUDBackgroundGeometry->setTexCoordArray( 0, HUDBackgroundTex );
    HUDBackgroundGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

    geode->addDrawable( HUDBackgroundGeometry );

    // Create and set up a state set using the texture from above
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    state->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // This disables colorblending of the texture with the underlying quad
    // osg::TexEnv* decalState = new osg::TexEnv();
    // decalState->setMode( osg::TexEnv::DECAL );
    // state->setTextureAttribute( 0, decalState, osg::StateAttribute::ON );

    // en/disable blending
    if ( !transparency )
    {
        state->setMode( GL_BLEND, osg::StateAttribute::PROTECTED );
        state->setMode( GL_BLEND, osg::StateAttribute::OFF );
    }
    else
    {
        state->setMode( GL_BLEND, osg::StateAttribute::PROTECTED );
        state->setMode( GL_BLEND, osg::StateAttribute::ON );
        state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    }

    // add the geode
    addChild( geode );
}

WGETextureHud::WGETextureHudEntry::~WGETextureHudEntry()
{
    // cleanup
}

unsigned int WGETextureHud::WGETextureHudEntry::getRealWidth()
{
    return m_texture->getTextureWidth();
}

unsigned int WGETextureHud::WGETextureHudEntry::getRealHeight()
{
    return m_texture->getTextureHeight();
}

unsigned int WGETextureHud::getMaxElementWidth()
{
    return m_maxElementWidth;
}

void WGETextureHud::setMaxElementWidth( unsigned int width )
{
    m_maxElementWidth = width;
}

