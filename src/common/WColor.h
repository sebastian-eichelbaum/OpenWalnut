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

#ifndef WCOLOR_H
#define WCOLOR_H

#include <istream>
#include <ostream>
#include <cassert>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <osg/Vec4>

/**
 * Represents a RGBA Color
 */
class WColor
{
public:
    friend std::ostream& operator<<( std::ostream& out, const WColor& c );
    friend std::istream& operator>>( std::istream& in, WColor& c );

    /**
     * Standard way of constructing colors, alpha is optional, and black is default
     */
    WColor( float red = 0.0, float green = 0.0, float blue = 0.0, float alpha = 1.0 );

    /**
     * Sets the green channel for this color
     */
    void setGreen( float green );

    /**
     * Sets the blue channel for this color
     */
    void setBlue( float blue );

    /**
     * Sets the red channel for this color
     */
    void setRed( float red );

    /**
     * Sets the alpha channel for this color
     */
    void setAlpha( float alpha );

    /**
     * \return red channel for this color
     */
    float getRed() const;

    /**
     * \return green channel for this color
     */
    float getGreen() const;

    /**
     * \return blue channel for this color
     */
    float getBlue() const;

    /**
     * \return alpha channel for this color
     */
    float getAlpha() const;

    /**
     * Makes a OSG compatible copy of this.
     */
    osg::Vec4 getOSGColor() const;

    void tokenize( const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " " );


protected:
private:
    float m_red;
    float m_green;
    float m_blue;
    float m_alpha;
};

#endif  // WCOLOR_H
