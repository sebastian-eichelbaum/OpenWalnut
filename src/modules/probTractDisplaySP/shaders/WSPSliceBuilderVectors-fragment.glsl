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

    // line stipple
    if( minimum_distance( gl_TexCoord[1].xyz, gl_TexCoord[2].xyz, gl_TexCoord[0].xyz ) < u_glyphThickness )
    {
        gl_FragColor = gl_Color;
    }
    else
    {
        discard;
    }
}
