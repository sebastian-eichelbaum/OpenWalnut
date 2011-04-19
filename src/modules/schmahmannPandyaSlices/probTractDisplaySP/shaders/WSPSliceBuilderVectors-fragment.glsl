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

uniform float u_glyphThickness;
uniform float u_glyphSize;

float minimum_distance( vec3 v, vec3 w, vec3 p )
{
  // Return minimum distance between line segment vw and point p
  float len = length( v - w );
  if( len == 0.0 )   // v == w case
  {
      return distance(p, v);
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

    float area = u_glyphThickness * u_glyphSize * u_glyphSize;
    float l = length(  gl_TexCoord[1].xyz - gl_TexCoord[2].xyz );
    float p2 = -l / 3.14159265;
    float q = area / 3.14159265;
    float r1 = p2 + sqrt( p2 * p2 + q );
    float r2 = p2 - sqrt( p2 * p2 + q );
    float radius = max( r1, r2 );
    if( minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < radius )
    {
        gl_FragColor = gl_Color;
    }
    else
    {
        discard;
    }

}
