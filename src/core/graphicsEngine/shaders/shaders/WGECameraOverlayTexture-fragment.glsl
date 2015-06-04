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

#version 120

/**
 * Overlay texture
 */
uniform sampler2D u_overlay;

/**
 * Overlay Texture Width in pixels.
 */
uniform float u_overlayWidth = 1024;

/**
 * Overlay Texture height in pixels.
 */
uniform float u_overlayHeight = 256;

/**
 * Viewport width in pixel
 */
uniform float u_viewportWidth = 1280;

/**
 * Viewport height in pixel
 */
uniform float u_viewportHeight = 720;

/**
 * Desired overlay scale in percent
 */
uniform float u_overlayScalePerc = 50;

/**
 * Move overlay to right
 */
uniform bool u_toRight = true;

/**
 * Move overlay to the top
 */
uniform bool u_toTop = false;

/**
 * Change transparency of the overlay.
 */
uniform float u_overlayOpacity;

/**
 * Automatically blend out?
 */
uniform bool u_overlayAutoBlendOut;

/**
 * Animation timer. 100 Ticks per sec
 */
uniform int u_blendOutTimer;

/**
 * Howe long will it take to blend out? In seconds =
 */
uniform float u_overlayBlendOutDuration;

/**
 * Pixel position in [0,1]
 */
varying vec2 v_pos;

void main()
{
    // pixel coordinate of screen
    vec2 pixel = vec2( u_viewportWidth * v_pos.x, u_viewportHeight * v_pos.y );

    // scale according to uniform
    float scaler =( ( u_overlayScalePerc / 100.0 ) );

    float scaledOverlayWidth = u_overlayWidth * scaler;
    float scaledOverlayHeight = u_overlayHeight * scaler;

    float limitScaleX = min( 1.0, u_viewportWidth / scaledOverlayWidth );
    float limitScaleY = min( 1.0, u_viewportHeight / scaledOverlayHeight );

    scaledOverlayWidth *= min( limitScaleX, limitScaleY );
    scaledOverlayHeight *= min( limitScaleX, limitScaleY );

    // position right:
    pixel += vec2(
                   float( u_toRight ) * -( u_viewportWidth - scaledOverlayWidth ),
                   float( u_toTop ) * -( u_viewportHeight - scaledOverlayHeight )
                 );

    // absolute size in texture coordinate system
    vec2 coord = vec2( pixel.x / scaledOverlayWidth, pixel.y / scaledOverlayHeight );

    // avoid repeat/clamp effects
    float valid = float( ( coord.x >= 0.0 ) && ( coord.x <= 1.0 ) && ( coord.y >= 0.0 ) && ( coord.y <= 1.0 ) );

    // overlay texture
    vec4 tex = texture2D( u_overlay, coord );

    // auto blend out?
    float aSec = float( u_blendOutTimer ) / 100.0;
    float autoBlendOut = 1.0 - ( aSec / u_overlayBlendOutDuration );
    autoBlendOut = ( float( u_overlayAutoBlendOut ) * autoBlendOut ) + ( 1 - float( u_overlayAutoBlendOut ) );

    // use texture coordinate to mix along the cylinder
    gl_FragColor = vec4( tex.rgb, autoBlendOut* u_overlayOpacity * tex.a * valid );
}

