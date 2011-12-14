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

/////////////////////////////////////////////////////////////////////////////
// Varyings
// Used in vertex and fragment shader
/////////////////////////////////////////////////////////////////////////////

// The ray's starting point in texture space
varying vec3 v_rayStart;

// The ray direction in texture space
varying vec3 v_ray;

// the Surface normal at this point
varying vec3 v_normal;

// The isovalue scaled using texture scaling information to [0,1]
varying float v_isovalue;

// The scaling component of the modelview matrix.
varying float v_worldScale;

