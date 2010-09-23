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
#include "WGETransformationTools.glsl"
#include "WGEUtils.glsl"

#include "WMImageSpaceLIC-Transformation-varyings.glsl"

/**
 * The texture unit sampler
 */
uniform sampler3D u_texture0Sampler;

/**
 * Scaling factor to unscale the texture
 */
uniform float u_texture0Scale;

/**
 * Smallest possible value in the texture
 */
uniform float u_texture0Min;

/**
 * Transforms each vector on each pixel to image space.
 */
void main()
{
    // get the current vector at this position
    vec3 vec = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz, u_texture0Min, u_texture0Scale ).rgb;

    // zero length vectors are uninteresting. discard them
    if ( isZero( length( vec ), 0.01 ) )
    {
        discard;
    }

    // project the vectors to image space
    vec2 vecProjected = projectVector( vec ).xy;

    // scale it 
    vec2 vecProjectedScaled = textureNormalize( vecProjected );

    // calculate lighting for the surface
    // TODO(ebaum): material properties should be used instead
    float light = blinnPhongIlluminationIntensity( 0.5, 0.3, 0.3, 10.0, 1.0, 0.5, v_normal, v_viewDir, v_lightSource );
    
    gl_FragColor = vec4( abs(vecProjectedScaled), light, 1.0 );
}

