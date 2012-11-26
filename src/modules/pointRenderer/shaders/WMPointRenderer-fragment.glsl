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
varying vec3 v_vertex;

/**
 * The texture coordinates in [-1,1]
 */
varying vec3 v_texCoord;

/**
 * The scaling component of the modelview matrix.
 */
varying float v_worldScale;

/**
 * The radius of a sphere in world-space
 */
varying float v_worldSpaceRadius;

/**
 * The center point of the sphere
 */
varying vec3 v_centerPoint;

/**
 * Depth of the center point
 */
varying float v_centerVertexDepth;

/**
 * Deptj of the neareast point on the sphere towards the camera
 */
varying float v_nearestVertexDepth;

/**
 * Difference between nearest depth and center point depth
 */
varying float v_vertexDepthDiff;

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
    // create a sphere
    float r = 1.0 - ( ( v_texCoord.x * v_texCoord.x ) + ( v_texCoord.y * v_texCoord.y ) );
    if( r < 0.0 )
        discard;

    vec3 sphereSurf = v_vertex - v_centerPoint;
    sphereSurf.z += r * v_worldSpaceRadius;
    sphereSurf = normalize( sphereSurf );

    // lighting
    // NOTE: this is disabled as usually, we use a headlight, causing a specular highlight in the middle of each splat.
    // float light = blinnPhongIlluminationIntensity( wge_DefaultLightIntensity, sphereSurf );

    // finally set the color and depth
    wgeInitGBuffer();
    wge_FragColor = vec4( gl_Color.rgb * r, 1.0 );
    wge_FragNormal = textureNormalize( sphereSurf );
    wge_FragZoom = 0.1 * v_worldScale;
    wge_FragTangent = textureNormalize( vec3( 0.0, 1.0, 0.0 ) );
#ifdef DEPTHWRITE_ENABLED
    // we allow to disable depth write. This allows the GPU to disacard pixels before applying the fragment shader -> speedup
    gl_FragDepth = ( r * v_vertexDepthDiff ) + v_centerVertexDepth;
#endif
}

