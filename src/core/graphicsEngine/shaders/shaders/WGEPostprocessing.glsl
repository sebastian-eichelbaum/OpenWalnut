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

#ifndef WGEPOSTPROCESSING_GLSL
#define WGEPOSTPROCESSING_GLSL

#version 120

// This defines some stuff needed to enable your shader to allow the result to be post-processed.
// You'll need to do the following:
//  * Set a fragment-depth! If you render simple geometry, this is done by your GPU automatically. If you do ray-tracing or similar, where the
//    depth of a fragment is not the same as the proxy geometry, set a proper depth using gl_FragDepth.
//  * Set a fragment-color using wge_FragColor or gl_FragData[0] since GLSL does not allow gl_FragColor for multi-target renderings.
//  * Set a normal per fragment using wge_FragNormal. The normal needs to be scaled to [0,1] and in World-Space!
//    * This can be done by textureNormalize in WGETextureTools.glsl
//    * If you do not set a normal, some post-processings simply won't work
//    * Sometimes, the normal might not be needed in your own code. In this case, use #ifdef WGE_POSTPROCESSING_ENABLED to only calculate it in
//      the case post-processing is enabled
//    * You can use getGradient amd getGradientViewAligned in WGEShadingTools.glsl to calculate the normal in a 3D texture
//  * Set a fragment zoom factor. This represents the zoom of your modelview matrix. It is needed by some post-processors to provide
//    zoom-invariant effects.
//    * Needs to be scaled by 0.1
//    * Can be calculated this way: wge_FragZoom = 0.1 * length( ( gl_ModelViewMatrix * normalize( vec4( 1.0, 1.0, 1.0, 0.0 ) ) ).xyz );
//    * Simpler: call wge_FragZoom = 0.1 * getModelViewScale();

#ifdef WGE_POSTPROCESSING_ENABLED
    #define wge_FragNormal gl_FragData[1].rgb
    #define wge_FragColor  gl_FragData[0]
    #define wge_FragZoom   gl_FragData[2].r
    #define wge_FragTangent gl_FragData[3].rgb
#else
    vec3 WGE_POSTPROCESSING_ENABLED_dummyVec3;
    float WGE_POSTPROCESSING_ENABLED_dummyFloat;
    #define wge_FragNormal WGE_POSTPROCESSING_ENABLED_dummyVec3
    #define wge_FragColor  gl_FragData[0]
    #define wge_FragZoom   WGE_POSTPROCESSING_ENABLED_dummyFloat
    #define wge_FragTangent WGE_POSTPROCESSING_ENABLED_dummyVec3
#endif

/**
 * This initializes the G-Buffer (Geometry Buffer). You need to call this before using one of the output variables wge_FragXYZ.
 * \note: a G-Buffer is basically a term for several textures storing per-pixel geometry information like normals and positions and similar.
 */
void wgeInitGBuffer()
{
    // we need to initialize all output buffers with an alpha value or they will be blended away if blending is on.
    gl_FragData[0] = vec4( vec3( 0.0 ), 1.0 );
    gl_FragData[1] = vec4( vec3( 0.0 ), 1.0 );
    gl_FragData[2] = vec4( vec3( 0.0 ), 1.0 );
    gl_FragData[3] = vec4( vec3( 0.0 ), 1.0 );
}

#endif // WGEPOSTPROCESSING_GLSL

