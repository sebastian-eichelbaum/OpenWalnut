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

#include <cassert>
#include <string>
#include <vector>

#include "WColor.h"
#include "WStringUtils.hpp"

WColor::WColor( float red, float green, float blue, float alpha )
    :   m_red( red ),
    m_green( green ),
    m_blue( blue ),
    m_alpha( alpha )
{
}

void WColor::setGreen( float green )
{
    assert( green <= 1.0 && green >= 0.0 );
    m_green = green;
}

void WColor::setBlue( float blue )
{
    assert( blue <= 1.0 && blue >= 0.0 );
    m_blue = blue;
}

void WColor::setRed( float red )
{
    assert( red <= 1.0 && red >= 0.0 );
    m_red = red;
}

void WColor::setAlpha( float alpha )
{
    assert( alpha <= 1.0 && alpha >= 0.0 );
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

osg::Vec4 WColor::getOSGColor() const
{
    return osg::Vec4( m_red, m_green, m_blue, m_alpha );
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
    std::vector<std::string> tokens;
    std::string str;
    in >> str;
    tokens = string_utils::tokenize( str, ";" );
    assert( tokens.size() == 3 && "There weren't 3 color values for a WColor" );

    c.setRed( boost::lexical_cast< float >( tokens[0] ) );
    c.setGreen( boost::lexical_cast< float >( tokens[1] ) );
    c.setBlue( boost::lexical_cast< float >( tokens[2] ) );

    return in;
}
