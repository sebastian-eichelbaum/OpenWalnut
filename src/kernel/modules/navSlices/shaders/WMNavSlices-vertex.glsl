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

varying vec4 VaryingTexCoord0;
varying vec4 VaryingTexCoord1;
varying vec4 VaryingTexCoord2;
varying vec4 VaryingTexCoord3;
varying vec4 VaryingTexCoord4;
varying vec4 VaryingTexCoord5;
varying vec4 VaryingTexCoord6;
varying vec4 VaryingTexCoord7;

void main()
{
    VaryingTexCoord0 = gl_MultiTexCoord0;
    VaryingTexCoord1 = gl_MultiTexCoord1;
    VaryingTexCoord2 = gl_MultiTexCoord2;
    VaryingTexCoord3 = gl_MultiTexCoord3;
    VaryingTexCoord4 = gl_MultiTexCoord4;
    VaryingTexCoord5 = gl_MultiTexCoord5;
    VaryingTexCoord6 = gl_MultiTexCoord6;
    VaryingTexCoord7 = gl_MultiTexCoord7;
    gl_Position = ftransform();
}
