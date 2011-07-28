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

#include "WGEOffscreenFinalPass.h"

WGEOffscreenFinalPass::WGEOffscreenFinalPass( size_t textureWidth, size_t textureHeight, int num ):
    WGEOffscreenTexturePass( textureWidth, textureHeight, num )
{
    // initialize members
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER );
    setRenderOrder( osg::Camera::NESTED_RENDER, 0 );

    // render before the hud
    if( m_hud )
    {
        getOrCreateStateSet()->setRenderBinDetails( m_hud->getRenderBin() - 1, "RenderBin" );
    }
    else
    {
        getOrCreateStateSet()->setRenderBinDetails( 10000, "RenderBin" );
    }

    osg::StateSet* state = this->getOrCreateStateSet();
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // allow the user to use blending
    getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
}

WGEOffscreenFinalPass::WGEOffscreenFinalPass( size_t textureWidth, size_t textureHeight, osg::ref_ptr< WGETextureHud > hud, std::string name,
                                                  int num ):
    WGEOffscreenTexturePass( textureWidth, textureHeight, hud, name, num )
{
    // initialize members
    setRenderTargetImplementation( osg::Camera::FRAME_BUFFER );
    setRenderOrder( osg::Camera::NESTED_RENDER, 0 );

    // render before the hud
    if( m_hud )
    {
        getOrCreateStateSet()->setRenderBinDetails( m_hud->getRenderBin() - 1, "RenderBin" );
    }
    else
    {
        getOrCreateStateSet()->setRenderBinDetails( 10000, "RenderBin" );
    }

    osg::StateSet* state = this->getOrCreateStateSet();
    state->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

    // allow the user to use blending
    getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
}

WGEOffscreenFinalPass::~WGEOffscreenFinalPass()
{
    // cleanup
}

