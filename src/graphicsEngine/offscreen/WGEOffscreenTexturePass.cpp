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

#include "../WGETextureHud.h"
#include "../WGEGeodeUtils.h"

#include "WGEOffscreenTexturePass.h"

WGEOffscreenTexturePass::WGEOffscreenTexturePass( size_t textureWidth, size_t textureHeight, int num ):
    WGEOffscreenRenderPass( textureWidth, textureHeight, num )
{
    // initialize members
    setup();
}

WGEOffscreenTexturePass::WGEOffscreenTexturePass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name,
                                                  int num ):
    WGEOffscreenRenderPass( textureWidth, textureHeight, hud, name, num )
{
    // initialize members
    setup();
}

WGEOffscreenTexturePass::~WGEOffscreenTexturePass()
{
    // cleanup
}

void WGEOffscreenTexturePass::setup()
{
    // we need to create a nice quad for texture processing spanning the whole texture space
    osg::ref_ptr< osg::Geode > geode = wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),
                                                            osg::Vec3( 1.0, 0.0, 0.0 ),
                                                            osg::Vec3( 0.0, 1.0, 0.0 ) );
    // setup the texture matrix scaler to the geode
    geode->addUpdateCallback( new TextureMatrixUpdateCallback( this ) );

    // add the slice to the geode
    this->addChild( geode );

    // disable all annoying states as blending, lighting and so on is done via shaders
    osg::StateSet* state = this->getOrCreateStateSet();
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
    state->setMode( GL_LIGHTING, osg::StateAttribute::PROTECTED );
    state->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    state->setMode( GL_BLEND, osg::StateAttribute::PROTECTED );
    state->setMode( GL_BLEND, osg::StateAttribute::OFF );

    // setup 2D projection
    this->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
    this->setProjectionMatrixAsOrtho2D( 0.0, 1.0, 0.0, 1.0 );
    this->setViewMatrix( osg::Matrixd::identity() );

    // enable texture coordinate manipulation via texture matrices
    m_texMat = new osg::TexMat;
    m_texMat->setMatrix( osg::Matrixd::identity() );
    state->setTextureAttributeAndModes( 0, m_texMat, osg::StateAttribute::ON );
}

void WGEOffscreenTexturePass::TextureMatrixUpdateCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::ref_ptr< osg::TexMat > texMat = m_pass->m_texMat;

    // we need all this to scale the texture coordinates to match the viewport of the camera as the whole texture might be larger than the
    // viewport.
    unsigned int screenWidth = m_pass->getViewport()->width();
    unsigned int screenHeight = m_pass->getViewport()->height();

    texMat->setMatrix( osg::Matrixd::scale( static_cast< float >( screenWidth ) / static_cast< float >( m_pass->getTextureWidth() ),
                                            static_cast< float >( screenHeight )/ static_cast< float >( m_pass->getTextureHeight() ), 1.0 ) );

    // call nested callbacks
    traverse( node, nv );
}

