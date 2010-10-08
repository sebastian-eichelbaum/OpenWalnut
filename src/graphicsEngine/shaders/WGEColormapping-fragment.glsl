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

#include "WGEColormapping-uniforms.glsl"
#include "WGEColormapping-varyings.glsl"

/**
 * Calculates the final colormapping. Call this from your fragment shader. A call to colormapping() from within the vertex shader is also needed.
 * Be aware that this only works with the WGEColormapping class.
 * 
 * \return the final color determined by the user defined colormapping
 */
vec4 colormapping()
{
    return vec4( u_colormap0Alpha, 0.0, 0.0, 1.0 );
}

