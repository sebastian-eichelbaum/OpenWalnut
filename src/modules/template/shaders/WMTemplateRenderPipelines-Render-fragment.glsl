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

// For lighting functionality.
#include "WGEShadingTools.glsl"

// The surface normal
varying vec3 v_normal;

void main()
{
    float light = blinnPhongIlluminationIntensity( normalize( viewAlign( v_normal ) ) );
    // finally set the color and depth
    // NOTE: you should remember that, when outputting to more than one texture (if you attached multiple color outputs), you need to use
    // gl_FragData instead of gl_FragColor!
    gl_FragData[0] = vec4( light, 0.0, 0.0, 1.0 );
}

