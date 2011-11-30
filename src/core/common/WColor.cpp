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

#include <cmath>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "../common/exceptions/WOutOfBounds.h"
#include "../common/WStringUtils.h"
#include "WColor.h"

// initialize static palette
namespace defaultColor
{
    /** the default palette colors */
    const WColor DefaultPalette[ 22 ]= {
            // The standard colors
            RED, GREEN, BLUE, YELLOW, ORANGE, PINK, CYAN,
            // the slightly lighter standard colors
            LIGHTRED, LIGHTGREEN, LIGHTBLUE, LIGHTYELLOW,
            // the slighly darker standard colors
            DARKRED, DARKGREEN, DARKBLUE, DARKYELLOW, VIOLET, TEAL,
            // black-white
            BLACK, GRAY25, GRAY50, GRAY75, WHITE
    };
}

// This function is taken from VTK 5.4.2. Since its BSD licensed the license
// notice follows below. It is not taken from FAnToM since it seems more self
// documenting.
//
// /*=========================================================================
//
//   Program:   Visualization Toolkit
//   Module:    $RCSfile: vtkMath.cxx,v $
//
//   Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
//   All rights reserved.
//   See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
//
//      This software is distributed WITHOUT ANY WARRANTY; without even
//      the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//      PURPOSE.  See the above copyright notice for more information.
//
// =========================================================================
//   Copyright 2005 Sandia Corporation.
//   Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
//   license for use of this work by or on behalf of the
//   U.S. Government. Redistribution and use in source and binary forms, with
//   or without modification, are permitted provided that this Notice and any
//   statement of authorship are reproduced on all copies.
//
//   Contact: pppebay@sandia.gov,dcthomp@sandia.gov,
//
// =========================================================================*/
WColor convertHSVtoRGBA( double h, double s, double v )
{
    const double onethird = 1.0 / 3.0;
    const double onesixth = 1.0 / 6.0;
    const double twothird = 2.0 / 3.0;
    const double fivesixth = 5.0 / 6.0;
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;

    // compute RGB from HSV
    if( h > onesixth && h <= onethird ) // green/red
    {
        g = 1.0;
        r = ( onethird - h ) / onesixth;
        b = 0.0;
    }
    else if( h > onethird && h <= 0.5 ) // green/blue
    {
        g = 1.0;
        b = ( h - onethird ) / onesixth;
        r = 0.0;
    }
    else if( h > 0.5 && h <= twothird ) // blue/green
    {
        b = 1.0;
        g = ( twothird - h ) / onesixth;
        r = 0.0;
    }
    else if( h > twothird && h <= fivesixth ) // blue/red
    {
        b = 1.0;
        r = ( h - twothird ) / onesixth;
        g = 0.0;
    }
    else if( h > fivesixth && h <= 1.0) // red/blue
    {
        r = 1.0;
        b = ( 1.0 - h ) / onesixth;
        g = 0.0;
    }
    else // red/green
    {
        r = 1.0;
        g = h / onesixth;
        b = 0.0;
    }

    // add Saturation to the equation.
    r = ( s * r + ( 1.0 - s ) ) * v;
    g = ( s * g + ( 1.0 - s ) ) * v;
    b = ( s * b + ( 1.0 - s ) ) * v;

    return WColor( r, g, b, 1.0f );
}

WColor inverseColor( const WColor& other )
{
    return WColor( std::abs( 1.0f - other[0] ), std::abs( 1.0f - other[1] ), std::abs( 1.0f - other[2] ), other[3] );
}
