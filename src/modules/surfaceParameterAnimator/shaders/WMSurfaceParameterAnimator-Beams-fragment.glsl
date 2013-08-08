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

#version 130
#extension GL_EXT_gpu_shader4 : enable

#include "WGETextureTools.glsl"
#include "WGETransformationTools.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Varyings
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "WMSurfaceParameterAnimator-Beams-varyings.glsl"

///////////////////////////////////////////////////////////////////////////////////////////////////
// Uniforms
///////////////////////////////////////////////////////////////////////////////////////////////////

// **************************************************************************
// Uniforms defining the input textures
// **************************************************************************

// texture containing the data
uniform sampler3D u_texture0Sampler;

// texture containing the tracing data
uniform sampler3D u_texture1Sampler;

// texture containing the tracing data -> the scaling factor of the values in the texture
uniform float u_texture1Scale;

// texture containing the tracing data -> the min value of the values in the texture
uniform float u_texture1Min;

// **************************************************************************
// Uniforms for the isosurface mode
// **************************************************************************

// The isovalue to use.
uniform float u_isovalue;

// The number of steps to use.
uniform int u_steps;

// The alpha value to set
uniform float u_alpha;

// **************************************************************************
// Animation
// **************************************************************************

// Animation reference in 100th of a second
uniform int u_animationTime;

// **************************************************************************
// Setup appearance of surface beams
// **************************************************************************

// the saturation of the final image. Useful to gray out stuff.
uniform float u_saturation;

// The size of beam 1
uniform int u_size1;

// The size of beam 2
uniform int u_size2;

// The speed of beam 1 relative to the clock in u_animationTime
uniform int u_speed1;

// The speed of beam 2 relative to the clock in u_animationTime
uniform int u_speed2;

// Scaling factor to resize the parameter space to match others.
uniform float u_parameterScale;

///////////////////////////////////////////////////////////////////////////////////////////////////
// Attributes
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Calculates the endpoint and distance using the ray and ray-start-point up to the end of the
 * bounding volume.
 *
 * \param d the distance along the ray until the ray leaves the bounding volume.
 *
 * \return the end point -> v_rayStart + v_ray*d
 */
vec3 findRayEnd( out float d )
{
    vec3 r = v_ray + vec3( 0.0000001 );
    vec3 p = v_rayStart;

    // we need to ensure the vector components are not exactly 0.0

    // v_ray in cube coordinates is used to check against the unit cube borders
    // when will v_ray reach the front face?
    float tFront     = - p.z / r.z;                  // (x,x,0) = v_rayStart + t * v_ray
    float tBack      = ( 1.0 - p.z ) / r.z;          // (x,x,1) = v_rayStart + t * v_ray

    float tLeft      = - p.x / r.x;                  // (0,x,x) = v_rayStart + t * v_ray
    float tRight     = ( 1.0 - p.x ) / r.x;          // (1,x,x) = v_rayStart + t * v_ray

    float tBottom    = - p.y / r.y;                  // (x,0,x) = v_rayStart + t * v_ray
    float tTop       = ( 1.0 - p.y ) / r.y;          // (x,1,x) = v_rayStart + t * v_ray

    // get the nearest hit
    d = min( min( max( tFront, tBack ), max( tLeft, tRight ) ), max( tBottom, tTop ) );
    return p + ( r * d );
}

/**
 * Distance between two points.
 *
 * \param p1 point 1
 * \param p2 point 2
 *
 * \return || p1-p2 ||
 */
float pointDistance( vec3 p1, vec3 p2 )
{
    return length( p1 - p2 );
}

/**
 * @brief Function to calculate lighting intensitybased on "Real-Time Volume Graphics, p 119, chapter 5.4, Listing 5.1".
 * It is basically the same as blinnPhongIllumination function above. But it is faster if you just need
 * the intensity.
 *
 * @param ambient   materials ambient intensity
 * @param diffuse   materials diffuse intensity
 * @param specular  materials specular intensity
 * @param shininess material shininess
 * @param lightIntensity  the light intensity
 * @param ambientIntensity the ambient light intensity
 * @param normalDir the normal
 * @param viewDir   viewing direction
 * @param lightDir  light direction
 *
 * @return the light intensity.
 */
float blinnPhongIlluminationIntensity( float ambient, float diffuse, float specular,
                                       float shininess,
                                       float lightIntensity, float ambientIntensity,
                                       vec3 normalDir, vec3 viewDir, vec3 lightDir )
{
    vec3 H =  normalize( lightDir + viewDir );

    // compute ambient term
    float ambientV = ambient * ambientIntensity;

    // compute diffuse term
    float diffuseLight = max( dot( lightDir, normalDir ), 0. );
    float diffuseV = diffuse * diffuseLight;

    // compute specular term
    float specularLight = pow( max( dot( H, normalDir ), 0. ), shininess );
    if( diffuseLight <= 0.) specularLight = 0.;
    float specularV = specular * specularLight;

    return ambientV + (diffuseV + specularV)*lightIntensity;
}

bool epsilonEquals( float value, float equals, float epsilon = 0.01 )
{
    return abs( value - equals ) <= epsilon;
}

bool inInterval( float value, float valStart, float valEnd )
{
    return ( value >= valStart ) && ( value <= valEnd );
}

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // please do not laugh, it is a very very very simple "isosurface" shader
    vec4 color = vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragDepth = gl_FragCoord.z;

    // First, find the rayEnd point. We need to do it in the fragment shader as the ray end point may be interpolated wrong
    // when done for each vertex.
    float totalDistance = 0.0;
    vec3 rayEnd = findRayEnd( totalDistance );

    // the point along the ray in cube coordinates
    vec3 curPoint = v_rayStart;

    // the current value inside the data
    float value;

    // the step counter
    int i = 0;
    float stepDistance = totalDistance / float( u_steps );
    while( i < u_steps ) // we do not need to ch
    {
        // get current value
        value = texture3D( u_texture0Sampler, curPoint ).r;

        // is it the isovalue?
        if( abs( value - u_isovalue ) < 0.1 )
        {
            // we need to know the depth value of the current point inside the cube
            // Therefore, the complete standard pipeline is reproduced here:

            // 1: transfer to world space and right after it, to eye space
            vec3 curPointProjected = project( curPoint );

            // 2: set depth value
            gl_FragDepth = curPointProjected.z;

            // 3: find a proper normal for a headlight
            float s = 0.005;
            float valueXP = texture3D( u_texture0Sampler, curPoint + vec3( s, 0.0, 0.0 ) ).r;
            float valueXM = texture3D( u_texture0Sampler, curPoint - vec3( s, 0.0, 0.0 ) ).r;
            float valueYP = texture3D( u_texture0Sampler, curPoint + vec3( 0.0, s, 0.0 ) ).r;
            float valueYM = texture3D( u_texture0Sampler, curPoint - vec3( 0.0, s, 0.0 ) ).r;
            float valueZP = texture3D( u_texture0Sampler, curPoint + vec3( 0.0, 0.0, s ) ).r;
            float valueZM = texture3D( u_texture0Sampler, curPoint - vec3( 0.0, 0.0, s ) ).r;

            vec3 dir = v_ray;
            dir += vec3( valueXP, 0.0, 0.0 );
            dir -= vec3( valueXM, 0.0, 0.0 );
            dir += vec3( 0.0, valueYP, 0.0 );
            dir -= vec3( 0.0, valueYM, 0.0 );
            dir += vec3( 0.0, 0.0, valueZP );
            dir -= vec3( 0.0, 0.0, valueZP );

            // Phong:
            float light = blinnPhongIlluminationIntensity(
                    0.3,                                // material ambient
                    1.0,                                // material diffuse
                    1.3,                                // material specular
                    10.0,                               // shinines
                    1.0,                                // light diffuse
                    0.75,                               // light ambient
                    normalize( -dir ),                  // normal
                    normalize( v_ray ),                 // view direction
                    normalize( v_lightSource )          // light source position
            );

            // 3: get the current trace value from tex1, which in most cases is a increasing number along the rasterized line direction
            float paramSpaceSize = 1000.0;
            float minSize = ( paramSpaceSize * 0.05 );  // the minimum size of 1% of the parameter space size
            float maxSize = ( paramSpaceSize * 0.30 );  // the maximum size -> 30% of parameter space size

            float trace    = texture3D( u_texture1Sampler, curPoint ).r  * ( paramSpaceSize );
            float traceInv = ( 1.0 - texture3D( u_texture1Sampler, curPoint ).r ) * ( paramSpaceSize );

            // 4: prepare animation
            // the current time step:
            float timeStep = u_animationTime / 4.0; // scale 100th of a second to 25 times per second

            //timeStep = 25;
            // create a triangle function increasing time in 1/100 steps
            float relativeSpeed1 = float( u_speed1 ) / 2.0;
            float relativeSpeed2 = float( u_speed2 ) / 2.0;
            float anim1 = ( int( timeStep * relativeSpeed1 ) % int( paramSpaceSize + maxSize + 30 ) ) - maxSize / 2.0;
            float anim2 = ( int( timeStep * relativeSpeed2 ) % int( paramSpaceSize + maxSize + 30 ) ) - maxSize / 2.0;

            // To have more than one halo at a time:
            // anim1 = anim1 % 50;
            // anim2 = anim2 % 50;
            // trace = trace % 50;
            // traceInv = traceInv % 50;

            // original surface color
            vec4 ocol = light * gl_Color;
            ocol.a = u_alpha;

            // apply animation color
            // ensure the beam is at leas 1% and at most 30% of the space long
            float halfSize1 = ( minSize + maxSize * ( u_size1 / 100.0 ) ) / 2.0;
            float halfSize2 = ( minSize + maxSize * ( u_size2 / 100.0 ) ) / 2.0;

            // does this fragment belong to the beam?
            bool belongsToBeam1 = abs( anim1 - trace ) <= halfSize1;
            bool belongsToBeam2 = abs( anim2 - traceInv ) <= halfSize2;

            // check four cases:
            // 1. fragment belongs only to beam 1
            if( belongsToBeam1 )
            {
                ocol.r = light + 0.4;
                ocol.g = 0.0;
                ocol.b = 0.0;
            }

            // 2. fragment belongs only to beam 2
            if( ( belongsToBeam2 && !belongsToBeam1 ) || ( belongsToBeam2 && ( u_size1 > u_size2 ) ) )
            {
                ocol.r = 0.0;
                ocol.g = light + 0.4;
                ocol.b = 0.0;
            }

            // 4. fragment belongs to a border
            bool doWhite   = ( epsilonEquals( abs( anim2 - traceInv ), halfSize2 + 0.5, 5.0 ) ||
                               epsilonEquals( abs( anim1 - trace ),    halfSize1 + 0.5, 5.0 ) );
            if( doWhite )
            {
                ocol.r = 1.0;
                ocol.g = 1.0;
                ocol.b = 1.0;
            }

            ocol *= u_saturation;
            ocol.a = u_alpha;
            gl_FragData[0] = ocol;

            break;
        }
        else
        {
            // no it is not the iso value
            // -> continue along the ray
            curPoint += stepDistance * v_ray;
        }

        // do not miss to count the steps already done
        i++;
    }

    // the ray did never hit the surface --> discard the pixel
    if( i == u_steps )
    {
        discard;
    }
}

