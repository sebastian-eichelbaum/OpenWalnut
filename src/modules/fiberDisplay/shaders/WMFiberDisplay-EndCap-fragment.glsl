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

#extension GL_EXT_geometry_shader4 : enable

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGEPostprocessing.glsl"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * The size of the tube
 */
uniform float u_tubeSize;

/**
 * Ratio between colormap and fiber color.
 */
uniform float u_colormapRatio = 1.0;

/**
 * Ratio between original dataset color and ROI colors.
 */
uniform float u_roiFilterColorOverride = 0.0;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

/**
 * The normal in world-space
 */
varying vec3 v_normalWorld;

/**
 * The vertex coordinate in world-space
 */
varying vec4 v_vertex;

/**
 * The scaling component of the modelview matrix.
 */
varying float v_worldScale;

/**
 * This varying carries the current cluster color.
 */
varying vec4 v_clusterColor;

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // it should be round
    float c = ( gl_TexCoord[0].x * gl_TexCoord[0].x ) + ( gl_TexCoord[0].y * gl_TexCoord[0].y );
    // clip everything outside the sphere
    float outside = 1.0 - step( 1.0, c );

    // the normal
    vec3 normal = 0.5 * u_tubeSize * v_normalWorld;

    // light
    float light = 1.0;
#ifdef ILLUMINATION_ENABLED
    light = blinnPhongIlluminationIntensity( wge_DefaultLightIntensityLessDiffuse, normalize( normal ) );
#endif

    // calculate some kind of "round" depth value
    // NOTE: gl_TexCoord[0].w is 0.0 if the front of the cap is seen
    vec3 v = v_vertex.xyz + ( 1.0 - gl_TexCoord[0].w ) * ( normal ) * ( 1.0 - c );
    // apply standard projection:
    vec4 vProj = gl_ProjectionMatrix * vec4( v.xyz, 1.0 );
    float depth = ( 0.5 * vProj.z / vProj.w ) + 0.5;

    float w = gl_TexCoord[0].w * ( 1.0 - abs( gl_TexCoord[0].x ) ) + ( 1.0 - gl_TexCoord[0].w );

    // apply colormapping to the input color
    vec4 finalColor = gl_Color;

    // use secondary color only if bitfield filtering is active
#ifdef BITFIELD_ENABLED
#ifdef SECONDARY_COLORING_ENABLED
    // finalColor = mix( v_clusterColor, gl_Color, u_roiFilterColorOverride );
    // FIXME_ somehow, the secondary color array does not arrive at the vertex shader although it is bound. We make endcaps transparent in the
    // case
    finalColor = mix( vec4( 0.0 ), gl_Color, u_roiFilterColorOverride );
#endif
#endif

#ifdef CLUSTER_FILTER_ENABLED
    finalColor = vec4( v_clusterColor, 1.0 );
#endif
#ifdef COLORMAPPING_ENABLED
    finalColor = mix( finalColor, colormapping(), u_colormapRatio );
#endif

    // done
    wge_FragColor = vec4( light * w * finalColor.rgb, outside * finalColor.a );
    wge_FragNormal = textureNormalize( normalize( normal ) );
    wge_FragZoom = v_worldScale;
    gl_FragDepth = depth;
}

