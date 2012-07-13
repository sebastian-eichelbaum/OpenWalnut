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

#include "WGETextureTools.glsl"

uniform sampler3D u_vectorsSampler;
uniform sampler3D u_probTractSampler;

/**
 * These two uniforms are needed to transform the vectors out of their texture back to their original form
 * as they are stored in RBGA (for example allowing only values between 0..1 for components but no negative
 * ones).
 */
uniform float u_vectorsMin;
uniform float u_vectorsScale;

varying vec4 diffusionDirection;

//uniform float u_glyphThickness;
//uniform float u_glyphSize;

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
    // ellipsoiden scheiss
    // if( ( length( gl_TexCoord[0] - gl_TexCoord[1] ) + length( gl_TexCoord[0] - gl_TexCoord[2] ) ) < 1.2 )

    // determine if this is a long or short line stipple

    //    Old radius driven behaviour:
    //    ############################
    //
    //    float scaleFactor = 1 - length( gl_TexCoord[1].xyz - gl_TexCoord[2].xyz ) / u_glyphSize;
    //    scaleFactor = 1 + scaleFactor * scaleFactor;
    //
    //    // line stipple
    ////    if( minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < scaleFactor * u_glyphThickness )
    //    if( minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < u_glyphThickness )
    //    {
    //        gl_FragColor = gl_Color;
    //    }
    //    else
    //    {
    //        discard;
    //    }

    // New approach: u_glyphSize controls the area each line stipple must use, radius is then derived from that.
    // #########################################################################################################

    // // Tried to calc optimal area...
    // float optRadius = u_glyphSize / 16.0;
    // float optLength = u_glyphSize - 2 * optRadius;
    // float optArea = 2 * optRadius * sqrt( 2 * optLength * optLength ) + 3.14159265 * optRadius * optRadius;

    // // Scalar values for zoom pansen
    // float magicZoom = length( ( gl_ModelViewMatrix * normalize( vec4( 1.0, 1.0, 1.0, 0.0 ) ) ).xyz );

//    float area =  u_glyphSize * u_glyphSize / 10.0; // 10 * u_glyphThickness * 1.0 /  (2 * u_glyphSize) * optArea;
//
//    // generally the area of a line stipple is a circle with radius R (each half for the endings of the line stipple) plus
//    // a quad with height 2R and width length of the focalPoints v and w. hence we have this equation in R to solve:
//    //
//    //    R^2*PI + 2R*length(v, w) - A = 0
//    //
//    // where A is the area to fill.
//
//    float l = distance(  0.8 * gl_TexCoord[1].xyz, 0.8 * gl_TexCoord[2].xyz );
//    float p2 = -l / 3.14159265;
//    float q = area / 3.14159265;
//    float r1 = p2 + sqrt( p2 * p2 + q );
//    float r2 = p2 - sqrt( p2 * p2 + q );
//    float radius = max( r1, r2 );
//
//    vec3 focalPoint1 = 0.8 * gl_TexCoord[1].xyz;
//    vec3 focalPoint2 = 0.8 * gl_TexCoord[2].xyz;
//    vec3 focalPoint1 = gl_TexCoord[1].xyz - normalize( gl_TexCoord[1].xyz ) * radius;
//    vec3 focalPoint2 = gl_TexCoord[2].xyz - normalize( gl_TexCoord[2].xyz ) * radius;

//     float f_l = distance( focalPoint1, focalPoint2 );
//     float f_p2 = -l / 3.14159265;
//     float f_q = area / 3.14159265;
//     float f_r1 = p2 + sqrt( p2 * p2 + q );
//     float f_r2 = p2 - sqrt( p2 * p2 + q );
//     float f_radius = max( r1, r2 );

//    vec4 direction = abs( texture3DUnscaled( u_vectorsSampler, gl_TexCoord[0].xyz, u_vectorsMin, u_vectorsScale ) );
//    vec4 probRGBA = texture3D( u_probTractSampler, gl_TexCoord[0].xyz );
//    probRGBA.a = 1.0; // set alpha explicity to 1

    // vec4 value = texture3D( u_probTractSampler, vec3(gl_TexCoord[0].x, gl_TexCoord[0].y, gl_TexCoord[0].z) );
    // gl_FragColor = vec4( gl_TexCoord[0].xyz, 1.0 ); // vec4( value, 1.0, 0.0, 1.0 );
//    gl_FragColor = vec4( value, 0.0, 0.0, 1.0 );
//    gl_FragColor = vec4( texture3D( u_probTractSampler, pansen.xyz ).rgb, 1.0 );//vec4( 1.0, 0.0, 0.0, 1.0 );
    gl_FragColor = vec4( gl_TexCoord[1].xyz, 1.0 );
    // gl_FragColor = 0.8 * probRGBA + 0.2 * direction;


//    // if( minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < radius )
//    if( minimum_distance( focalPoint1, focalPoint2, gl_TexCoord[0].xyz ) < u_glyphThickness * radius )
//    {
//        gl_FragColor = gl_Color;
//    }
//    else
//    {
//        // if( minimum_distance( focalPoint1, focalPoint2, gl_TexCoord[0].xyz ) < u_glyphThickness * ( radius + 0.01 ) )
//        // {
//        //     gl_FragColor = vec4( 1.0, 1.0, 1.0, gl_Color.w );
//        // }
//
//        discard;
//
//        // // Draw quad and inner cricle
//        //
//        // gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
//        // float lp = length( gl_TexCoord[0].xyz );
//        // if( lp >= ( u_glyphSize / 2.0 ) )
//        // {
//        //     gl_FragColor = vec4( 0.7, 0.7, 1.0, 1.0 ); // discard;
//        // }
//    }

    // // display middle point
    // if( length( gl_TexCoord[0].xyz ) <=  0.01 )
    // {
    //     gl_FragColor = vec4( 1.0, 0.1, 0.1, 1.0 ); // discard;
    // }

    // // display evec end points
    //
    // if( ( distance( gl_TexCoord[0].xyz, gl_TexCoord[1].xyz ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 1.0, 1.0, 0.0, 1.0 ); // yellow
    // }

    // if( ( distance( gl_TexCoord[0].xyz, gl_TexCoord[2].xyz ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 1.0, 0.0, 1.0 ); // green
    // }

    // // display new focal points
    //
    // if( ( distance( gl_TexCoord[0].xyz, focalPoint1 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 1.0, 1.0, 1.0 ); // cyan
    // }
    // if( ( distance( gl_TexCoord[0].xyz, focalPoint2 ) < 0.01 ) )
    // {
    //     gl_FragColor = vec4( 0.0, 0.0, 1.0, 1.0 ); // blue
    // }

    // // Color debugging facilities
    // if(  minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < 0.01 )
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
}
