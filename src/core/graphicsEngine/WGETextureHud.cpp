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
#include <string>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <osg/Camera>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/TexEnv>
#include <osgText/Text>

// Compatibility between OSG 3.2 and earlier versions
#include "core/graphicsEngine/WOSG.h"

#include "../common/WPathHelper.h"

#include "WGETextureHud.h"

WGETextureHud::WGETextureHud():
    osg::Projection(),
    m_group( new WGEGroupNode() ),
    m_maxElementWidth( 256 ),
    m_renderBin( 10000 ),
    m_viewport( new osg::Viewport() ),
    m_coupleTexViewport( false )
{
    getOrCreateStateSet()->setRenderBinDetails( m_renderBin, "RenderBin" );
    m_group->addUpdateCallback( new SafeUpdateCallback( this ) );
    addChild( m_group );
}

WGETextureHud::~WGETextureHud()
{
    // cleanup
}

void WGETextureHud::SafeUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    // set the new size of the widget (how can we get this data?)
    unsigned int screenWidth = m_hud->m_viewport->width();
    unsigned int screenHeight = m_hud->m_viewport->height();
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
        float height = static_cast< float >( m_hud->getMaxElementWidth() * tex->getRealHeight() ) / static_cast< float >( tex->getRealWidth() );

        // scale texture if needed
        if( m_hud->m_coupleTexViewport )
        {
            osg::ref_ptr< osg::TexMat > texMat = tex->getTextureMatrix();
            texMat->setMatrix( osg::Matrixd::scale( static_cast< float >( screenWidth ) / static_cast< float >( tex->getRealWidth() ),
                                                    static_cast< float >( screenHeight )/ static_cast< float >( tex->getRealHeight() ), 1.0 ) );

            // this also changes the aspect ratio in the texture:
            height = static_cast< float >( m_hud->getMaxElementWidth() * screenHeight ) / static_cast< float >( screenWidth );
        }

        // scale them to their final size
        osg::Matrixd scale = osg::Matrixd::scale( m_hud->getMaxElementWidth(), height, 1.0 );

        // need to add a "line-break"?
        if( nextY + height + border > screenHeight )
        {
            nextX += m_hud->getMaxElementWidth() + border;
            nextY = border;
        }

        // transform them to the right place
        osg::Matrixd translate = osg::Matrixd::translate( static_cast< double >( nextX ), static_cast< double >( nextY ), 0.0 );
        tex->setMatrix( scale * translate );

        // calculate the y position of the next texture
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

/**
 * This method compares a specified texture with the specified node. If the node is an WGETextureHudEntry instance, the containied textures
 * get compared.
 *
 * \param tex the texture to compare to
 * \param node the node
 *
 * \return true if node->m_texture == tex.
 */
bool hudEntryPredicate( osg::ref_ptr< osg::Texture > tex, osg::ref_ptr< osg::Node > const& node )
{
    // is the node an WGETextureHudEntry?
    WGETextureHud::WGETextureHudEntry const* e = dynamic_cast< WGETextureHud::WGETextureHudEntry const* >( node.get() );
    if( !e )
    {
        return false;
    }

    // check if textures are equal
    return e->getTexture() == tex;
}

void WGETextureHud::removeTexture( osg::ref_ptr< osg::Texture > texture )
{
    typedef WPredicateHelper::ArbitraryPredicate< osg::ref_ptr< osg::Node > const,
                                                  boost::function< bool ( osg::ref_ptr< osg::Node > const& ) > > TexCheck;  // NOLINT - if the
    // space after bool is removed (as the stylechecker want) it interprets it as old-style cast and complains about it. This is valid syntax for
    // boost::function.

    m_group->remove_if(
        boost::shared_ptr< WGEGroupNode::NodePredicate >(
            new TexCheck( boost::bind( &hudEntryPredicate, texture, _1 ) )
        )
    );
}

void WGETextureHud::setViewport( osg::Viewport* viewport )
{
    m_viewport = viewport;
}

void WGETextureHud::coupleViewportWithTextureViewport( bool couple )
{
    m_coupleTexViewport = couple;
}

WGETextureHud::WGETextureHudEntry::WGETextureHudEntry( osg::ref_ptr< osg::Texture2D > texture, std::string name, bool transparency ):
    osg::MatrixTransform(),
    m_texture( texture ),
    m_name( name )
{
    setMatrix( osg::Matrixd::identity() );
    setReferenceFrame( osg::Transform::ABSOLUTE_RF );

    //////////////////////////////////////////////////
    // Texture Quad

    osg::Geode* geode = new osg::Geode();

    // Set up geometry for the HUD and add it to the HUD
    osg::ref_ptr< wosg::Geometry > HUDBackgroundGeometry = new wosg::Geometry();

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
    HUDBackgroundGeometry->setNormalBinding( wosg::Geometry::BIND_OVERALL );
    HUDBackgroundGeometry->addPrimitiveSet( HUDBackgroundIndices );
    HUDBackgroundGeometry->setVertexArray( HUDBackgroundVertices );
    HUDBackgroundGeometry->setColorArray( HUDcolors );
    HUDBackgroundGeometry->setTexCoordArray( 0, HUDBackgroundTex );
    HUDBackgroundGeometry->setColorBinding( wosg::Geometry::BIND_OVERALL );

    geode->addDrawable( HUDBackgroundGeometry );

    // Create and set up a state set using the texture from above
    osg::StateSet* state = geode->getOrCreateStateSet();
    state->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    state->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    // enable texture coordinate manipulation via texture matrices
    m_texMat = new osg::TexMat;
    m_texMat->setMatrix( osg::Matrixd::identity() );
    state->setTextureAttributeAndModes( 0, m_texMat, osg::StateAttribute::ON );

    // This disables colorblending of the texture with the underlying quad
    // osg::TexEnv* decalState = new osg::TexEnv();
    // decalState->setMode( osg::TexEnv::DECAL );
    // state->setTextureAttribute( 0, decalState, osg::StateAttribute::ON );

    // en/disable blending
    if( !transparency )
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

    //////////////////////////////////////////////////
    // Text
    osg::ref_ptr< osg::Geode > textGeode = new osg::Geode();
    state = textGeode->getOrCreateStateSet();
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    addChild( textGeode );
    osgText::Text* label = new osgText::Text();
    label->setFont( WPathHelper::getAllFonts().Default.string() );
    label->setBackdropType( osgText::Text::OUTLINE );
    label->setCharacterSize( 15 );
    label->setText( m_name );
    label->setAxisAlignment( osgText::Text::SCREEN );
    label->setPosition( osg::Vec3( 0.01, 0.01, -1.0 ) );
    label->setColor( osg::Vec4( 1.0, 1.0, 1.0, 1.0 ) );
    textGeode->addDrawable( label );
}

WGETextureHud::WGETextureHudEntry::~WGETextureHudEntry()
{
    // cleanup
}

unsigned int WGETextureHud::WGETextureHudEntry::getRealWidth() const
{
    return m_texture->getTextureWidth();
}

unsigned int WGETextureHud::WGETextureHudEntry::getRealHeight() const
{
    return m_texture->getTextureHeight();
}

osg::ref_ptr< osg::TexMat > WGETextureHud::WGETextureHudEntry::getTextureMatrix() const
{
    return m_texMat;
}

std::string WGETextureHud::WGETextureHudEntry::getName() const
{
    return m_name;
}

unsigned int WGETextureHud::getMaxElementWidth() const
{
    return m_maxElementWidth;
}

void WGETextureHud::setMaxElementWidth( unsigned int width )
{
    m_maxElementWidth = width;
}

osg::ref_ptr< osg::Texture2D > WGETextureHud::WGETextureHudEntry::getTexture() const
{
    return m_texture;
}

size_t WGETextureHud::getRenderBin() const
{
    return m_renderBin;
}

