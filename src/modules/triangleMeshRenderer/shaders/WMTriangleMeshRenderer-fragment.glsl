//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV-Leipzig and CNCF-CBS
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

varying vec4 VaryingTexCoord0;

uniform int opacity;

#include "WGELighting-fragment.glsl"

void main()
{
    vec4 col = gl_Color;

    vec4 ambient = vec4( 0.0 );
    vec4 diffuse = vec4( 0.0 );
    vec4 specular = vec4( 0.0 );
    calculateLighting( -normal, gl_FrontMaterial.shininess, ambient, diffuse, specular );

    col = ( ambient * col / 2.0 ) + ( diffuse * col ) + ( specular * col / 3.0 );

    col = clamp( col, 0.0, 1.0 );

    col.a = float( opacity ) * 0.01; // MacOS X has old shader spec, no implicit conversion to float takes place

    gl_FragColor = col;
}
