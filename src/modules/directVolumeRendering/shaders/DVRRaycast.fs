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

/////////////////////////////////////////////////////////////////////////////
// Varyings
/////////////////////////////////////////////////////////////////////////////

#include "DVRRaycast.varyings"

/////////////////////////////////////////////////////////////////////////////
// Uniforms
/////////////////////////////////////////////////////////////////////////////

// texture containing the data
uniform sampler3D tex0;

// The isovalue to use.
uniform float u_isovalue;

// Should the shader create some kind of isosurface instead of a volume rendering?
uniform bool u_isosurface;

/////////////////////////////////////////////////////////////////////////////
// Attributes
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Variables
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Functions
/////////////////////////////////////////////////////////////////////////////

/**
 * Main entry point of the fragment shader.
 */
void main()
{
    // please do not laugh, it is a very very very simple "isosurface" shader
    gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );

    if ( u_isosurface )
    {
        vec3 curPoint = v_rayStart;
        float value;
        int i = 0;
        while ( i < 250 )
        {
            i++;
            value = texture3D( tex0, curPoint ).r;

            if ( value >= u_isovalue )
            {
                break;
            }
            else
            {
                curPoint += 0.005 * v_ray;
            }
        }

        if ( i >= 250 )
        {
            discard;
        }

        gl_FragColor = vec4( vec3( 1.0 - i / 150.0 ), 1.0 );
    }

    
}

