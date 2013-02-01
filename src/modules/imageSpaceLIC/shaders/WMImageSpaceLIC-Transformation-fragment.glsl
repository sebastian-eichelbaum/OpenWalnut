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

#include "WGEColormapping-fragment.glsl"

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGETransformationTools.glsl"
#include "WGEUtils.glsl"

//#include "WGEColorMaps.glsl"

#include "WMImageSpaceLIC-Transformation-varyings.glsl"

/**
 * The texture unit sampler
 */
uniform sampler3D u_texture0Sampler;

/**
 * The texture unit sampler for the 3D noise texture
 */
uniform sampler3D u_texture1Sampler;

/**
 * Scaling factor to unscale the texture
 */
uniform float u_texture0Scale = 1.0;

/**
 * Smallest possible value in the texture
 */
uniform float u_texture0Min = 0.0;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeX = 255;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeY = 255;

/**
 * Size of input texture in pixels
 */
uniform int u_texture0SizeZ = 255;

/**
 * Transforms each vector on each pixel to image space.
 */
void main()
{
    vec3 vecProjected;    // contains the final vector at each fragment

    // if we have a 3D noise texture, use it.
#ifdef NOISE3D_ENABLED
    float noise3D = texture3D( u_texture1Sampler, gl_TexCoord[0].xyz * v_noiseScaleFactor.xyz ).r;
#else
    float noise3D = 1.0;
#endif

#ifdef VECTORDATA
    // get the current vector at this position
    vec3 vec = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz, u_texture0Min, u_texture0Scale ).rgb;
    vec *= sign( vec.x );

    // zero length vectors are uninteresting. discard them
    if( isZero( length( vec ), 0.01 ) )
    {
        discard;
    }

    // project the vectors to image space as the input vector is in the tangent space
    vecProjected = projectVector( vec ).xyz;

#endif
#ifdef SCALARDATA

    // do central differences to get the vector
    // project the vectors to image space

    float sx = 1.0 / u_texture0SizeX;
    float sy = 1.0 / u_texture0SizeY;
    float sz = 1.0 / u_texture0SizeZ;
    float valueXP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( sx, 0.0, 0.0 ), 0.0, 1.0 ).r;
    float valueXM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( sx, 0.0, 0.0 ), 0.0, 1.0 ).r;
    float valueYP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( 0.0, sy, 0.0 ), 0.0, 1.0 ).r;
    float valueYM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( 0.0, sy, 0.0 ), 0.0, 1.0 ).r;
    float valueZP = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz + vec3( 0.0, 0.0, sz ), 0.0, 1.0 ).r;
    float valueZM = texture3DUnscaled( u_texture0Sampler, gl_TexCoord[0].xyz - vec3( 0.0, 0.0, sz ), 0.0, 1.0 ).r;

    vec3 vec = vec3( valueXP - valueXM, valueYP - valueYM, valueZP - valueZM );
    vec *= sign( vec.x );

    // zero length vectors are uninteresting. discard them
    vecProjected = projectVector( vec ).xyz;
    if( isZero( length( vec ), 0.01 ) )
    {
        discard;
    }

#endif

    // calculate lighting for the surface
    float light = blinnPhongIlluminationIntensity( normalize( v_normal ) );

    // MPI PAper Hack: {
    // vec4 cmap = colormapping();
    // gl_FragData[1] = vec4( vec3( cmap.r ), 1.0 );
    //
    // if( cmap.r < 0.15 )
    //     discard;
    // if( isZero( cmap.r - 0.2, 0.1 ) )
    //     gl_FragData[1] = vec4( 0.25, 0.0, 0.0, 1.0 );
    // if( isZero( cmap.r - 0.3, 0.1 ) )
    //     gl_FragData[1] = vec4( 0.5, 0.0, 0.0, 1.0 );
    // if( isZero( cmap.r - 1.0, 0.15 ) )
    //     gl_FragData[1] = vec4( 1.0, 0.0, 0.0, 1.0 );
    // }
    gl_FragData[1] = colormapping();

    // is the vector very orthogonal to the surface? We scale our advection according to this.
    float dotS = abs( dot( v_normalObject, vec.xyz ) );
    // scale down the vector -> the more it "comes out of the surface, the shorter the vector".
    vec2 dotScaled = ( 1.0 - dotS ) * scaleMaxToOne( vecProjected ).xy;

    gl_FragData[0] = vec4( vec2( 0.5 ) + ( 0.5  * dotScaled ), light, noise3D );
}

