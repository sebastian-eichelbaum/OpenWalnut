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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Welcome to this demo shader. It basically demonstrates the coupling features you already seen in WMTemplateShaders.
// (Properties coupled to uniforms, #defines, property-coupled defines and so on.
//
// NOTE: In most cases, you will also have a fragment shader.
//
// Please note that you need a basic understanding of GLSL to understand this rather simple example.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the version you like. But be aware that older hardware often does not support a version >120. The core shaders
// of OpenWalnut all use 120.
#version 120

// You already know this from C/C++. Technically, GLSL does NOT support inclusion this way. But WGEShaders implements
// a text-replacement based mechanism to allow this though. You are not required to add any paths here. Just ensure the
// shaders are somewhere in THIS shader's search path.
#include "WGETransformationTools.glsl"
// This includes are not really needed here but shows that there are a lot of predefined functions available. Please
// have a look in core/graphicsEngine/shaders/shaders. You will find a lot of useful tools there.
//
//  * Colormapping
//  * Lighting/Shading algorithms
//  * Postprocessing algorithms
//  * Tensor/Matrix utilities
//  * Texture-access with value-descaling
//  * ...
// It is definitely worth a look.
//
// TIP: you have a long list of varying variables? Why not putting them into an separate file and include it here and in
// your fragment shader?

// These are the uniforms we defined in the C++ side. They are accessible in vertex, geometry and fragment shaders.
// This is updated by a callback:
uniform int u_animation;
// These are coupled to the corresponing properties.
uniform vec4 u_spheresColor;
uniform float u_sphereScaler;

// These uniforms where set automatically by wge::bindTexture:
uniform sampler3D u_noiseSampler;
// The size in voxel dimensions
uniform int u_noiseSizeX;
uniform int u_noiseSizeY;
uniform int u_noiseSizeZ;

// The surface normal
varying vec3 v_normal;

// Normalized coordinate in the bounding volume of the sphere
varying vec3 v_normalizedVertex;

// NOTE: We encourage you to use the "u_" prefix and "v_" prefix for your uniforms and varyings. This makes it easier to
// distinguish them in the code.

void main()
{
    // Done. The remaining code is standard GLSL.

    // for easy access to texture coordinates
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // time based dimming of the color
    float dimmer = 0.5 + 0.5 * sin( 3.1416 * mod( u_animation, 100.0 ) / 100.0 );

    // prepare light
    v_normal = gl_NormalMatrix * gl_Normal;
    v_normalizedVertex = gl_Vertex.xyz / 20.0; // trick a bit... looks cooler later

    // push color and position forward through the pipeline
    gl_FrontColor = vec4( u_spheresColor.rgb * dimmer, 1.0 );
    gl_Position = gl_ModelViewProjectionMatrix * vec4( gl_Vertex.xyz * u_sphereScaler, 1.0 );
}
