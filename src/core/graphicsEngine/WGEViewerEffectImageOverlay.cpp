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

#include "WGEViewer.h"

#include "callbacks/WGEFunctorCallback.h"
#include "shaders/WGEShader.h"
#include "shaders/WGEPropertyUniform.h"

#include "WGEViewerEffectImageOverlay.h"

WGEViewerEffectImageOverlay::WGEViewerEffectImageOverlay():
    WGEViewerEffect( "Image Overlay", "Blend in some arbitrary image." )
{
    m_blendOutAuto = m_properties->addProperty( "Auto Blend-Out",
        "Make the overlay blend out automatically after a given amount of time.", true );

    m_blendOutDuration = m_properties->addProperty( "Blend-Out Duration",
        "Make the overlay blend out after this amount of time in seconds.", 4.0 );
    m_blendOutDuration->setMin( 0.0 );
    m_blendOutDuration->setMax( 60.0 );

    m_image = m_properties->addProperty( "Image", "The Image to use.", WPathHelper::getSharePath() / "GE" / "overlay.png" );
    WPropDouble scale = m_properties->addProperty( "Scale", "Scale the image in percent.", 50.0 );
    scale->setMin( 0.0 );
    scale->setMax( 200.0 );

    WPropBool moveToTop = m_properties->addProperty( "Move to Top", "Move the image to the top.", false );
    WPropBool moveToRight = m_properties->addProperty( "Move to Right", "Move the image to the right.", true );

    WPropDouble opacity = m_properties->addProperty( "Opacity",
        "Make the overlay transparent. Please be aware that the image itself might be transparent already.", 1.0 );
    opacity->setMin( 0.0 );
    opacity->setMax( 3.0 );

    osg::ref_ptr< WGEShader > overlayShader = new WGEShader( "WGECameraOverlayTexture" );
    overlayShader->apply( m_geode );

    m_forceReload = true;

    // texture setup, Loading is done later in the update callback.
    m_logoTexture = new osg::Texture2D;
    m_logoTexture->setDataVariance( osg::Object::DYNAMIC );
    // no wrapping
    m_logoTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER );
    m_logoTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER );

    // texture width and height (gets set by the update callback)
    m_overlayWidth = new osg::Uniform( "u_overlayWidth", static_cast< float >( 1 ) );
    m_overlayHeight = new osg::Uniform( "u_overlayHeight", static_cast< float >( 1 ) );
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

    m_state->addUniform( new WGEPropertyUniform< WPropDouble >( "u_overlayOpacity", opacity ) );
    m_state->addUniform( new WGEPropertyUniform< WPropBool >( "u_overlayAutoBlendOut", m_blendOutAuto ) );
    m_state->addUniform( new WGEPropertyUniform< WPropDouble >( "u_overlayBlendOutDuration", m_blendOutDuration ) );


    // add a callback which handles changes in viewport size
    m_updater = new Updater();
    addUpdateCallback( m_updater );

    // also add an animation timer
    osg::Uniform* animationTime(
        new osg::Uniform(
            "u_blendOutTimer",     // a name.
            0                      // initial value, type of this parameter defines the uniform type in GLSL!
        )
    );
    // use the predefined timer callback:
    osg::ref_ptr< WGEShaderAnimationCallback > m_animationCallback = new WGEShaderAnimationCallback();
    animationTime->setUpdateCallback( m_animationCallback );

    // bind to geode
    m_geode->getOrCreateStateSet()->addUniform( animationTime );

    // bind
    m_state->setTextureAttributeAndModes( 0, m_logoTexture, osg::StateAttribute::ON );
}

WGEViewerEffectImageOverlay::~WGEViewerEffectImageOverlay()
{
    // cleanup
}

void WGEViewerEffectImageOverlay::setReferenceViewer( WGEViewer::SPtr viewer )
{
    m_viewer = viewer;
}

const boost::shared_ptr< WGEViewer > WGEViewerEffectImageOverlay::getReferenceViewer() const
{
    return m_viewer;
}

void WGEViewerEffectImageOverlay::Updater::operator() ( osg::Node* node, osg::NodeVisitor* nv )
{
    WGEViewerEffectImageOverlay* effect = dynamic_cast< WGEViewerEffectImageOverlay* >( node );
    if( effect )
    {
        // viewer set?
        if( effect->m_viewer )
        {
            // valid camera? -> update viewport
            WGECamera* cam = effect->m_viewer->getCamera();
            if( cam )
            {
                // valid viewport?
                if( cam->getViewport() )
                {
                    effect->m_viewportWidth->set( static_cast< float >( cam->getViewport()->width() ) );
                    effect->m_viewportHeight->set( static_cast< float >( cam->getViewport()->height() ) );
                }
            }

            // update image if needed
            if( effect->m_forceReload || effect->m_image->changed() )
            {
                effect->m_forceReload = false;
                osg::Image* logoImage = osgDB::readImageFile( effect->m_image->get( true ).string() );
                if( logoImage )
                {
                    effect->m_logoTexture->setImage( logoImage );
                    effect->m_overlayWidth->set( static_cast< float >( logoImage->s() ) );
                    effect->m_overlayHeight->set( static_cast< float >( logoImage->t() ) );
                }
            }
        }
    }

    // call nested callbacks
    traverse( node, nv );
    return;
}

