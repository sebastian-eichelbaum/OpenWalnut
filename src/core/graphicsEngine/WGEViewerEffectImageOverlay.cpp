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

#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "../common/WPathHelper.h"
#include "../common/WProperties.h"
#include "../common/WLogger.h"

#include "callbacks/WGEFunctorCallback.h"
#include "shaders/WGEShader.h"
#include "shaders/WGEPropertyUniform.h"

#include "WGEViewerEffectImageOverlay.h"

WGEViewerEffectImageOverlay::WGEViewerEffectImageOverlay():
    WGEViewerEffect( "Image Overlay", "Blend in some arbitrary image." )
{
    WPropFilename imageFn = m_properties->addProperty( "Image", "The Image to use.", WPathHelper::getSharePath() / "GE" / "overlay.png" );
    WPropDouble scale = m_properties->addProperty( "Scale", "Scale the image in percent.", 50.0 );
    scale->setMin( 0.0 );
    scale->setMax( 100.0 );

    WPropBool moveToTop = m_properties->addProperty( "Move to Top", "Move the image to the top.", false );
    WPropBool moveToRight = m_properties->addProperty( "Move to Right", "Move the image to the right.", true );

    osg::ref_ptr< WGEShader > overlayShader = new WGEShader( "WGECameraOverlayTexture" );
    overlayShader->apply( m_geode );

    // some logo
    osg::ref_ptr< osg::Texture2D > logoTexture = new osg::Texture2D;
    osg::Image* logoImage = osgDB::readImageFile( ( WPathHelper::getSharePath() / "GE" / "overlay.png" ).string() );
    if( logoImage )
    {
        // Assign the texture to the image we read from file:
        logoTexture->setImage( logoImage );

        // no wrapping
        logoTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
        logoTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );
        logoTexture->setDataVariance( osg::Object::DYNAMIC );

        // texture width and height
        m_overlayWidth = new osg::Uniform( "u_overlayWidth", static_cast< float >( logoImage->s() ) );
        m_overlayHeight = new osg::Uniform( "u_overlayHeight", static_cast< float >( logoImage->t() ) );
        m_state->addUniform( m_overlayWidth );
        m_state->addUniform( m_overlayHeight );

        // NOTE: the values of these uniforms get updated by the updateViewport callback, so these values are only placeholders
        m_viewportWidth = new osg::Uniform( "u_viewportWidth", static_cast< float >( 1024 ) );
        m_viewportHeight = new osg::Uniform( "u_viewportHeight", static_cast< float >( 768 ) );
        m_state->addUniform( m_viewportWidth );
        m_state->addUniform( m_viewportHeight );

        m_state->addUniform( new WGEPropertyUniform< WPropDouble >( "u_overlayScalePerc", scale ) );
        m_state->addUniform( new WGEPropertyUniform< WPropBool >( "u_toTop", moveToTop ) );
        m_state->addUniform( new WGEPropertyUniform< WPropBool >( "u_toRight", moveToRight ) );

        // add a callback which handles changes in viewport size
        osg::ref_ptr< ViewportUpdater > viewportUpd( new ViewportUpdater() );
        viewportUpd->m_viewportWidth = m_viewportWidth;
        viewportUpd->m_viewportHeight = m_viewportHeight;
        setPreDrawCallback( viewportUpd );

        // bind
        m_state->setTextureAttributeAndModes( 0, logoTexture, osg::StateAttribute::ON );
    }
    else
    {
        wlog::error( "WGECameraEffect" ) << "Could not load overlay image \"" <<
                                            ( WPathHelper::getSharePath() / "GE" / "overlay.png" ).string()  << "\".";
    }
}

WGEViewerEffectImageOverlay::~WGEViewerEffectImageOverlay()
{
    // cleanup
}

