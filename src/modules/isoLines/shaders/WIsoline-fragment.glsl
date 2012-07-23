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

#include "WIsoline-varyings.glsl"

/**
 * Line width.
 */
uniform float u_lineWidth;

/**
 * Line Color.
 */
uniform vec4 u_color = vec4( 1.0, 0.0, 0.0, 1.0 );

// TODO(math): this function was copyied over from fiberStipple fragment shader. So please either replace it with
// a library function: look in GLSL as well as WGE! Or make it a WGE shader helper function or similar. Last but
// not least ask Seb about it!
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

/**
 * Draws a line from point p1 to point p2 with the given width and color.
 *
 * \param p1 Start point
 * \param p2 End point
 * \param width Line width
 * \param color Line color
 */
void drawLine( vec3 p1, vec3 p2, float width, vec4 color )
{
    // debug: enable to make quad background color red
    // gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );

    // draw actually the line
    if( minimum_distance( p1, p2, gl_TexCoord[1].xyz ) < width )
    {
        gl_FragColor = u_color;
    }

    // debug: enable to draw hit (where the isovalue hits the edge, exactly) point in blue
    // if( ( length( gl_TexCoord[1].xyz - p1 ) < 0.1 ) || ( length( gl_TexCoord[1].xyz - p2 ) < 0.1 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 );
    // }
}

void main()
{
    bool edge0Hit = bool( edge0Hit_f ); // OpenGL does not allow bool varyings
    bool edge1Hit = bool( edge1Hit_f ); // OpenGL does not allow bool varyings
    bool edge2Hit = bool( edge2Hit_f ); // OpenGL does not allow bool varyings
    bool edge3Hit = bool( edge3Hit_f ); // OpenGL does not allow bool varyings
    if( sumHits < 15.0 )
    {
        if( edge0Hit && edge1Hit )
        {
            drawLine( hit0Pos, hit1Pos, u_lineWidth, u_color );
        }
        else if( edge1Hit && edge2Hit )
        {
            drawLine( hit1Pos, hit2Pos, u_lineWidth, u_color );
        }
        else if( edge2Hit && edge3Hit )
        {
            drawLine( hit2Pos, hit3Pos, u_lineWidth, u_color );
        }
        else if( edge3Hit && edge0Hit )
        {
            drawLine( hit3Pos, hit0Pos, u_lineWidth, u_color );
        }
        else if( edge3Hit && edge1Hit )
        {
            drawLine( hit3Pos, hit1Pos, u_lineWidth, u_color );
        }
        else if( edge0Hit && edge2Hit )
        {
            drawLine( hit0Pos, hit2Pos, u_lineWidth, u_color );
        }
    }
    else // if( sumHits == 15 ) // Yes, this is topological INCORRECT but fast as hell and good looking too
    {
        drawLine( hit0Pos, hit1Pos, u_lineWidth, u_color );
        drawLine( hit2Pos, hit3Pos, u_lineWidth, u_color );
    }
}
