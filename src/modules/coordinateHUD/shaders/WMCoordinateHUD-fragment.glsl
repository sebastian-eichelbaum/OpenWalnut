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

/**
 * Texture sampler for the face-label texture
 */
#ifdef USE_FACETEXTURE
    uniform sampler2D tex;
#endif

void main()
{
#ifdef USE_FACETEXTURE
    vec4 f = texture2D( tex, gl_TexCoord[0].st );

    // simply set the color. All the magic happens in vertex shader.
    gl_FragColor = vec4( f.r * gl_Color.rgb, 1.0 );
#else
    // simply set the color. All the magic happens in vertex shader.
    gl_FragColor = gl_Color;
#endif
    // this line ensures that the coordinate system is always on top.
    gl_FragDepth = 0.001 * gl_FragCoord.z;
}

