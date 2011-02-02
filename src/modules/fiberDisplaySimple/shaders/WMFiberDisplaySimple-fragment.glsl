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

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGEPostprocessing.glsl"

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#ifdef CLIPPLANE_ENABLED
/**
 * The distance to the plane
 */
varying float dist;
#endif

/**
 * The surface normal. Needed for nice lighting.
 */
varying vec3 v_normal;

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

/**
 * The max distance allowed
 */
uniform float u_distance = 1.0;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

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
#ifdef CLIPPLANE_ENABLED
    // discard fragment if too far from plane
    if ( abs( dist ) >= u_distance )
    {
        discard;
    }
#endif

#ifdef ILLUMINATION_ENABLED
    float light = blinnPhongIlluminationIntensity( wge_DefaultLightIntensityLessDiffuse, normalize( v_normal ) );
#else
    float light = 1.0;
#endif

    wge_FragColor = vec4( vec3( gl_Color.xyz * light ), gl_Color.a );
    wge_FragNormal = textureNormalize( v_normal );
    gl_FragDepth = gl_FragCoord.z;
}

