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

/**
 * Color of the fiber stipples. This will be further combined with tract probability.
 */
uniform vec4 u_color;

/**
 * First focal point, which is one of the endings of the projected diffusion direction.
 */
varying vec3 focalPoint1;

/**
 * Second focal point, which is one of the endings of the projected diffusion direction.
 */
varying vec3 focalPoint2;

/**
 * First focal point, scaled.
 */
varying vec3 scaledFocalPoint1;

/**
 * Second focal point, scaled.
 */
varying vec3 scaledFocalPoint2;

/**
 * Middle point of the quad in texture coordinates, needed for scaling the projection of the principal diffusion direction to fit inside quad.
 */
uniform vec3 middlePoint_tex = vec3( 0.5, 0.5, 0.0 );

/**
 * Probability of the tract, used for further color mapping.
 */
varying float probability;

uniform float u_glyphThickness;

/**
 * Computes the minimal distance from segment vw and point p.
 *
 * \param v start point of the segment
 * \param w end point of the segment
 * \param p point for which the minimal distance should be computed
 *
 * \return minimal distance from segment vw to point p.
 */
float minimum_distance( vec3 v, vec3 w, vec3 p )
{
    // Return minimum distance between line segment vw and point p
    float len = length( v - w );
    if( len == 0.0 )   // v == w case
    {
        return distance( p, v );
    }
    // Consider the line extending the segment, parameterized as v + t (w - v).
    // We find projection of point p onto the line.
    // It falls where t = [(p-v) . (w-v)] / |w-v|^2
    float t = dot( p - v, w - v ) / ( len * len );

    if( t < 0.0 )      // Beyond the 'v' end of the segment
    {
        return distance( p, v );
    }
    else if( t > 1.0 ) // Beyond the 'w' end of the segment
    {
        return distance( p, w );
    }
    vec3 projection = v + t * ( w - v );  // Projection falls on the segment
    return distance( p, projection );
}

void main()
{
    // generally the area of a line stipple is a circle with radius R (each half for the endings of the line stipple) plus
    // a quad with height 2R and width length of the focalPoints v and w. hence we have this equation in R to solve:
    //
    //    R^2*PI + 2R*length(v, w) - A = 0
    //
    // where A is the area to fill.

    float area =  0.1; // this is arbitrarily set
    float l = distance( scaledFocalPoint1, scaledFocalPoint2 );
    float p2 = -l / 3.14159265;
    float q = area / 3.14159265;
    float r1 = p2 + sqrt( p2 * p2 + q );
    float r2 = p2 - sqrt( p2 * p2 + q );
    float radius = max( r1, r2 ) * u_glyphThickness;

    if( minimum_distance( scaledFocalPoint1, scaledFocalPoint2, gl_TexCoord[1].xyz ) < radius )
    {
        gl_FragColor = u_color * probability;
    }
    else
    {
        // if( minimum_distance( scaledFocalPoint1, scaledFocalPoint2, gl_TexCoord[1].xyz ) < ( radius + 0.01 ) )
        // {
        //     gl_FragColor = vec4( 1.0, 1.0, 1.0, gl_Color.w );
        // }

        discard;

        // // Draw quad and inner cricle
        // gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
        // float lp = distance( gl_TexCoord[1].xyz, middlePoint_tex );
        // if( lp >= 0.5 )
        // {
        //     gl_FragColor = vec4( 0.7, 0.7, 1.0, 1.0 ); // discard;
        // }
    }


    // // Color debugging facilities
    // if(  minimum_distance( focalPoint1, focalPoint2, gl_TexCoord[1].xyz ) < 0.01 )
    // {
    //     if( l <= 1.1 )
    //     {
    //         gl_FragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
    //     }
    //     else if( l < 1.225 )
    //     {
    //         gl_FragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
    //     }
    //     else
    //     {
    //         gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 );
    //     }
    // }
    // // display middle point
    // if( distance( gl_TexCoord[1].xyz, middlePoint_tex ) <=  0.01 )
    // {
    //     gl_FragColor = vec4( 0.0, 1.0, 0.0, 1.0 ); // green
    // }

    // // display new focal points
    // if( ( distance( gl_TexCoord[1].xyz, scaledFocalPoint1 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 1.0, 1.0, 1.0 ); // cyan
    // }
    // if( ( distance( gl_TexCoord[1].xyz, scaledFocalPoint2 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 ); // blue
    // }

    // // display evec end points
    // if( ( distance( gl_TexCoord[1].xyz, focalPoint1 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 ); // red
    // }

    // if( ( distance( gl_TexCoord[1].xyz, focalPoint2 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 1.0, 1.0, 0.0, 1.0 ); // yellow
    // }
}
