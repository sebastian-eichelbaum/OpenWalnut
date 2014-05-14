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

// For the colormapper
#include "WGEColormapping-fragment.glsl"

// The surface normal
varying vec3 v_normal;

void main()
{
    // we can now simply query the color of this fragment!
    vec4 col = colormapping();

    // Please also have a look into WGEColorMapsImproved.glsl. Use the colormap() function directly on your custom values to apply a colormap to
    // some custom data.

    // make it more spatial ...
    float light = blinnPhongIlluminationIntensity( normalize( viewAlign( v_normal ) ) );
    col.rgb *= light;

    // in this tutorial, we ensure there always is something to see.
    col.a = 1.0;
    gl_FragColor = col;
}

