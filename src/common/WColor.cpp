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
#include <cassert>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include "../common/WStringUtils.h"

#include "WColor.h"

WColor::WColor( float red, float green, float blue, float alpha )
    :   m_red( red ),
    m_green( green ),
    m_blue( blue ),
    m_alpha( alpha )
{
    // check if the given values are correct in range
// TODO(lmath): reenable asserts to WAsserts as soon as LIC module doesn't procude invalid colors
//    assert( m_green <= 1.0 && m_green >= 0.0 && "WColor comopnent out of range" );
//    assert( m_blue <= 1.0 && m_blue >= 0.0 && "WColor comopnent out of range" );
//    assert( m_red <= 1.0 && m_red >= 0.0 && "WColor comopnent out of range" );
//    assert( m_alpha <= 1.0 && m_alpha >= 0.0 && "WColor comopnent out of range" );
}

void WColor::setGreen( float green )
{
// TODO(lmath): reenable asserts to WAsserts as soon as LIC module doesn't procude invalid colors
//    assert( green <= 1.0 && green >= 0.0 );
    m_green = green;
}

void WColor::setBlue( float blue )
{
// TODO(lmath): reenable asserts to WAsserts as soon as LIC module doesn't procude invalid colors
//    assert( blue <= 1.0 && blue >= 0.0 );
    m_blue = blue;
}

void WColor::setRed( float red )
{
// TODO(lmath): reenable asserts to WAsserts as soon as LIC module doesn't procude invalid colors
//    assert( red <= 1.0 && red >= 0.0 );
    m_red = red;
}

void WColor::setAlpha( float alpha )
{
// TODO(lmath): reenable asserts to WAsserts as soon as LIC module doesn't procude invalid colors
//    assert( alpha <= 1.0 && alpha >= 0.0 );
    m_alpha = alpha;
}

float WColor::getRed() const
{
    return m_red;
}

float WColor::getGreen() const
{
    return m_green;
}

float WColor::getBlue() const
{
    return m_blue;
}

float WColor::getAlpha() const
{
    return m_alpha;
}

void WColor::setRGB( double r, double g, double b )
{
    setRed( r );
    setGreen( g );
    setBlue( b );
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
void WColor::setHSV( double h, double s, double v )
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

    setRGB( r, g, b );
}

std::ostream& operator<<( std::ostream& out, const WColor& c )
{
    float r = c.getRed();
    float g = c.getGreen();
    float b = c.getBlue();
    float a = c.getAlpha();

    out << r << ";" << g << ";" << b << ";" << a;
    return out;
}

std::istream& operator>>( std::istream& in, WColor& c )
{
    std::string str;
    in >> str;
    std::vector<std::string> tokens;
    tokens = string_utils::tokenize( str, ";" );
    assert( tokens.size() == 4 && "There weren't 4 color values for a WColor" );

    c.setRed( boost::lexical_cast< float >( tokens[0] ) );
    c.setGreen( boost::lexical_cast< float >( tokens[1] ) );
    c.setBlue( boost::lexical_cast< float >( tokens[2] ) );
    c.setAlpha( boost::lexical_cast< float >( tokens[3] ) );

    return in;
}

bool WColor::operator==( const WColor &rhs ) const
{
    return m_red == rhs.m_red &&
           m_green == rhs.m_green &&
           m_blue == rhs.m_blue &&
           m_alpha == rhs.m_alpha;
}

bool WColor::operator!=( const WColor &rhs ) const
{
    return !( *this == rhs );
}

void WColor::inverse()
{
    m_red = std::abs( 1. - m_red );
    m_green = std::abs( 1. - m_green );
    m_blue = std::abs( 1. - m_blue );
}

void WColor::average( const WColor& other )
{
    m_red = ( m_red + other.getRed() ) / 2.0;
    m_green = ( m_green + other.getGreen() ) / 2.0;
    m_blue = ( m_blue + other.getBlue() ) / 2.0;
}

const WColor WColor::green( 0.0, 1.0, 0.0, 1.0 );
const WColor WColor::red( 1.0, 0.0, 0.0, 1.0 );
const WColor WColor::blue( 0.0, 0.0, 1.0, 1.0 );
