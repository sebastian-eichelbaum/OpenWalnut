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

#include "WColor.hpp"

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
    c.tokenize(str, tokens, ";");

    c.setRed( boost::lexical_cast<float>( tokens[0] ) );
    c.setGreen( boost::lexical_cast<float>( tokens[1] ) );
    c.setBlue( boost::lexical_cast<float>( tokens[2] ) );

    return in;
}

void WColor::tokenize(const std::string& str,  std::vector<std::string>& tokens, const std::string& delimiters )
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of( delimiters, lastPos );

    while ( std::string::npos != pos || std::string::npos != lastPos )
    {
        // Found a token, add it to the vector.
        tokens.push_back( str.substr( lastPos, pos - lastPos ) );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );
    }
}

