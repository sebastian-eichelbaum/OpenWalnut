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

#ifndef WGEPOSTPROCESSOR_FRAGMENT_GLSL
#define WGEPOSTPROCESSOR_FRAGMENT_GLSL

#version 120

#include "WGEShadingTools.glsl"
#include "WGETextureTools.glsl"
#include "WGEPostprocessorUtils-fragment.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// These are specific to the postprocessor

/**
 * The number of bins to use for cel shading
 */
uniform int u_celShadingBins = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Varying
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Variables
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Postprocessors
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Apply laplace-filter to depth buffer. An edge is > 0.0.
 *
 * \return the edge
 */
float getEdge()
{
    // TODO(ebaum): provide properties/uniforms for the scaling factors here

    /////////////////////////////////////////////////////////////////////////////////////////////
    // GETTING TEXELS
    //
    // Get surrounding texels; needed for ALL filters
    /////////////////////////////////////////////////////////////////////////////////////////////

    float edgec  = getDepth( pixelCoord );
    float edgebl = getDepth( pixelCoord + vec2( -offsetX, -offsetY ) );
    float edgel  = getDepth( pixelCoord + vec2( -offsetX,     0.0  ) );
    float edgetl = getDepth( pixelCoord + vec2( -offsetX,  offsetY ) );
    float edget  = getDepth( pixelCoord + vec2(     0.0,   offsetY ) );
    float edgetr = getDepth( pixelCoord + vec2(  offsetX,  offsetY ) );
    float edger  = getDepth( pixelCoord + vec2(  offsetX,     0.0  ) );
    float edgebr = getDepth( pixelCoord + vec2(  offsetX,  offsetY ) );
    float edgeb  = getDepth( pixelCoord + vec2(     0.0,  -offsetY ) );

    /////////////////////////////////////////////////////////////////////////////////////////////
    // LAPLACE
    //
    // apply a standart laplace filter kernel
    /////////////////////////////////////////////////////////////////////////////////////////////

    // laplace filter kernel
    float edge = 16.0 * abs(
            0.0 * edgetl +  1.0 * edget + 0.0 * edgetr +
            1.0 * edgel  +  -4.0 * edgec + 1.0 * edger  +
            0.0 * edgebl +  1.0 * edgeb + 0.0 * edgebr
        );

    return edge;
}

/**
 * Calculate the cel-shading of a specified color. The number of colors is defined by the u_celShadingSamples uniform.
 *
 * \param inColor the color to shade
 *
 * \return the cel-shaded color
 */
vec4 getCelShading( vec4 inColor )
{
    float samples = float( u_celShadingBins );
    return vec4(
            vec3( int( ( inColor * samples ).r ),
                  int( ( inColor * samples ).g ),
                  int( ( inColor * samples ).b ) ) / samples, inColor.a );
}

/**
 * Calculate the cel-shading of the input color. The number of colors is defined by the u_celShadingSamples uniform.
 *
 * \return the cel-shaded color
 */
vec4 getCelShading()
{
    return getCelShading( getColor() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Main. Apply the specified post-processors.
 */
void main()
{
    // don't do this stuff for background pixel
    float depth = getDepth();
    gl_FragDepth = depth;
    if( depth > 0.99 )
    {
        discard;
    }

#ifdef WGE_POSTPROCESSOR_EDGE
    // output the depth and final color.
    #ifdef WGE_POSTPROCESSOR_OUTPUT_COMBINE

        // for black borders, multiply edge with color
        #ifdef WGE_POSTPROCESSOR_EDGE_BLACKEDGE
            #define EDGEOP * ( 1.0 - getEdge() )
        #else
            // for white borders, add
            #define EDGEOP + vec3( getEdge() )
        #endif

        // apply operation and output color
        gl_FragColor = vec4( getColor().rgb EDGEOP, 1.0 );
    #else
        // also do this for the effect-only output
        #ifdef WGE_POSTPROCESSOR_EDGE_BLACKEDGE
            #define EDGEOP 1.0 -
        #else
            // for white borders, add
            #define EDGEOP 0.0 +
        #endif
        gl_FragColor = vec4( vec3( EDGEOP getEdge() ), 1.0 );
    #endif
#endif

#ifdef WGE_POSTPROCESSOR_CEL
    // output the depth and final color.
    gl_FragColor = getCelShading();
#endif
}

#endif // WGEPOSTPROCESSOR_FRAGMENT_GLSL

