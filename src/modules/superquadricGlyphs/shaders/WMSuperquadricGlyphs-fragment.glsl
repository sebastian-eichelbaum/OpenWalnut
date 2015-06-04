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

// commonly used variables
#include "WMSuperquadricGlyphs-varyings.glsl"

// tollerance value for float comparisons
float zeroTollerance = 0.01;

#define RenderMode_Superquadric
//#define RenderMode_Ellipsoid

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- sPow
//
// This function extends the "pow" function with features to handle negative base values.
/////////////////////////////////////////////////////////////////////////////////////////////
float sPow( float x, float y )
{
    if( y == 0.0 )
    {
        return 1.0;
    }
    else
    {
        return pow( abs( x ), y );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function calculates allmost every value at a given point (the value of the
// superquadric function, gradient and derivate)
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
// ray:         returns position on ray
// gradient:    the gradient vector at current ray point
// sqd:         value of derived function at current ray point
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric( vec3 viewDir, vec3 planePoint, float t, out vec3 ray, out vec3 gradient, out float sqd )
{
    ray = planePoint.xyz + t*viewDir.xyz;

    // those values will be needed multiple times ...
    float rayXYPow = sPow( ray.x, v_alphaBeta.x ) + sPow( ray.y, v_alphaBeta.x );
    float rayZPow = sPow( ray.z, v_alphaBeta.y );

    // the value at given position
    float sq = sPow( rayXYPow, v_alphaBeta.z ) + rayZPow  - 1.0;

    // calculate squadric value for current t

    // if we get a hit we need those values for calculating the gradient at the hit point
    // if we do not get a hit we use those values for calculating the derivative at the current point
    // for doing the next newton step

    /////////////////// HINT ///////////////////
    // If we derive sign(x)*pow(abs(x),y) we use product rule to get:
    //                    sign' * pow + sign * pow'
    // Well the derived sign function is nothing else than the unit impulse delta.
    // It is              delta<>0 iff x=0
    // And also           x=0 -> pow(x, y)=0
    // so delta(x)*pow(x) will allways be 0 ... we can drop it
    //
    // ==> y * sign(x)* sPow(x, y-1.0);


    float a  = sign( rayXYPow ) * sPow( rayXYPow, v_alphaBeta.z - 1.0 );
    float b1 = sign( ray.x ) * sPow( ray.x, v_alphaBeta.x - 1.0 );

    float b2 = sign( ray.y ) * sPow( ray.y, v_alphaBeta.x - 1.0 );
    float c = v_alphaBeta.y  * sign( ray.z ) * sPow( ray.z, v_alphaBeta.y  - 1.0 );

    // now we can reuse some values to calculate the gradient vector at the hit point
    gradient = vec3( v_alphaBeta.y * a * b1, v_alphaBeta.y * a * b2, c );

    // calculate the derived function, reuse as much previously calculated values as possible
    sqd = dot( gradient, viewDir );

    // thats it, return value at current point
    return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- superquadric
//
// This function is the "light" version of the above one. It just calculates the result of
// the superquadrics function.
//
// Parameters:
// viewDir:     viewing direction vector
// planePoint:  point on projection plane
// t:           current point on ray
//
// Returns:
//              The function value of the superquadrics function at current point
/////////////////////////////////////////////////////////////////////////////////////////////
float superquadric( vec3 viewDir, vec3 planePoint, float t )
{
    vec3 ray = planePoint.xyz + t*viewDir.xyz;

    // those values will be needed multiple times ...
    float rayXYPow = sPow( ray.x, v_alphaBeta.x ) + sPow( ray.y, v_alphaBeta.x );
    float rayZPow = sPow( ray.z, v_alphaBeta.y );

    // the value at given position
    float sq = sPow( rayXYPow, v_alphaBeta.z ) + rayZPow  - 1.0;

    // thats it, return value at current point
    return sq;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// GPU Super Quadrics -- fragment shader -- main
/////////////////////////////////////////////////////////////////////////////////////////////
void main( void )
{
    // filter out glyphs whose anisotropy is smaller than the threshold or where the eigenvalues
    // are below the threshold (alphaBeta.w is != if this is the case)
    if( v_alphaBeta.w > 0.0 )
    {
        discard;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 1: try to find a goot start value for newton iteration
    /////////////////////////////////////////////////////////////////////////////////////////////

    // well the following value is quite empiric but it works well with squadrics whose beta<=alpha<=1.0
    // HINT: if the ray hits the surface of the glyph lastT will allways be negative!
    float lastT = 0.0;
    int numNewtonLoops = 10;

    // this vector stores the gradient if there has been a hit
    vec3 grad = vec3( 0.0, 0.0, 1.0 );

    // some vars that will be needed later
    // got a hit?
    bool hit = false;
    vec3 hitPoint = vec3( 0.0, 0.0, 0.0 );

#ifdef RenderMode_Superquadric // Superquadric based rendermode

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 2: newton iteration to determine roots of the superquadric-ray intersection
    /////////////////////////////////////////////////////////////////////////////////////////////

    // now try to calculate the intersection of the given ray with the superquadric.
    // here we are using newtons iterative method for finding roots

    // the iteration loop (NOTE: due to the fact that the shaders do not support an at compile time unknown loop count we set it to
    // this quite empiric value (which works well (for at least the squadrics with beta<=alpha<=1.0))
    for( int i = 0; i < numNewtonLoops; i++ )
    {
        // calculate all needed values
        float sqd = 0.0;
        float sq = superquadric( v_viewDir.xyz, v_planePoint.xyz, lastT, hitPoint, grad, sqd );

        // new ray parameter
        float newT = lastT - ( sq / sqd );

        // near enough?
        // or has t not changed much since last iteration?
        if( !hit && ( ( abs( sq ) <= zeroTollerance ) || ( abs( newT - lastT ) < zeroTollerance ) ) )
        {
            hit = true;
            break;
        }

        // if the ray parameter starts to jump around (it should get smaller step by step (because lastT is negative))
        // NOTE: this speeds up rendering at rays that will miss the glyph (round about 50% faster!)
        if( newT > lastT )
            break;

        // not near enough -> another newton step
        lastT = newT;
    }

#endif

#ifdef RenderMode_Ellipsoid // Render ellipsoids

/////////////////////////////////////////////////////////////////////////////////////////////
// 2: solve ellipsoid equation to determine roots of the intersection
/////////////////////////////////////////////////////////////////////////////////////////////

    float A = dot( v_viewDir.xyz, v_viewDir.xyz );
    float B = 2.0 * dot( v_viewDir.xyz, v_planePoint.xyz );
    float C = dot( v_planePoint.xyz, v_planePoint.xyz ) - 1.0;

    // try to solve At^2 + Bt + C = 0
    float discriminant = ( B * B ) - ( 4.0 * A * C );

    // no solution
    if( discriminant <= 0.0 )
    {
        discard;
    }

    // there will be a solution
    hit = true;

    // use solution formula
    float twoAinv = 1.0 / ( 2.0 * A );
    float root = sqrt( discriminant );
    float t1 = ( -B + root ) * twoAinv;
    float t2 = ( -B - root ) * twoAinv;

    lastT = min( t1, t2 );
    if( lastT < 0.0 )
    {
        discard;
    }

    // on a sphere surface the normal is allways the vector from the middle point (in our case (0,0,0))
    // to the surface point
    grad = -( v_planePoint.xyz + v_viewDir.xyz * lastT );

#endif

    /////////////////////////////////////////////////////////////////////////////////////////////
    // 3: draw or discard the pixel
    /////////////////////////////////////////////////////////////////////////////////////////////

    if( hit )
    {
        // get a normal in world space again: use v_glyphToWorld
        vec3 normal = normalize( ( v_glyphToWorld * vec4( grad, 0.0 ) ).xyz );

        // finally set the color and depth
        wgeInitGBuffer();
        wge_FragNormal = textureNormalize( normal );
        wge_FragZoom = v_worldScale;
        wge_FragTangent = textureNormalize( vec3( 0.0, 1.0, 0.0 ) );

#ifdef DIRECTIONALCOLORING_ENABLED
        vec3 col = gl_Color.rgb;
#else
        vec3 col = vec3( 1.0, 1.0, 1.0 );
#endif

        // draw shaded pixel
        wge_FragColor = blinnPhongIllumination(
            // material properties
            col.rgb * 0.2,                    // ambient color
            col.rgb * 2.0,                    // diffuse color
            col.rgb,                          // specular color
            30.0,                                  // shininess

            // light color properties
            gl_LightSource[0].diffuse.rgb,         // light color
            gl_LightSource[0].ambient.rgb,         // ambient light

            // directions
            normalize( grad ),                     // normal
            v_viewDir.xyz,                         // viewdir
            v_lightDir.xyz );                      // light direction
    }
    else // no hit: discard
    {
        // want to see the bounding box? uncomment this line
        // gl_FragColor=vec4(0.5, 0.5, 1., 1.0);
        discard;
    }
}

