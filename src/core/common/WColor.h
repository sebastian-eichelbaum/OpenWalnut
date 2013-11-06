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

#include <osg/Vec4>
#include <osg/io_utils> // for the operator<< and operator>> for Vec4



/**
 * Represents a RGBA Color.
 */
typedef osg::Vec4 WColor;

/**
 * Creates a color from a hue, saturation and value (HSV).
 *
 * \ingroup Color utils
 *
 * \param h hue
 * \param s saturation
 * \param v value
 *
 * \return The same color but in rgba format.
 */
WColor convertHSVtoRGBA( double h, double s, double v );

/**
 * Computes the inverse of this color in means of RGB space. The alpha value is untouched.
 *
 * \ingroup Color utils
 *
 * \param other The color (RGBA) from which the inverse should be calculated.
 */
WColor inverseColor( const WColor& other );

/**
 * Some default colors.
 */
namespace defaultColor
{
    // \cond Suppress_Doxygen
    static const WColor DARKRED( 0.5, 0.0, 0.0, 1.0 ); //!< Default for dark red
    static const WColor RED( 1.0, 0.0, 0.0, 1.0 ); //!< Default for red
    static const WColor LIGHTRED( 1.0, 0.5, 0.5, 1.0 ); //!< Default for lighter red

    static const WColor DARKGREEN( 0.0, 0.5, 0.0, 1.0 ); //!< Default for a dark green
    static const WColor GREEN( 0.0, 1.0, 0.0, 1.0 ); //!< Default for green
    static const WColor LIGHTGREEN( 0.5, 1.0, 0.5, 1.0 ); //!< Default for lighter green

    static const WColor DARKBLUE( 0.0, 0.0, 0.5, 1.0 ); //!< Default for blue
    static const WColor BLUE( 0.0, 0.0, 1.0, 1.0 ); //!< Default for blue
    static const WColor LIGHTBLUE( 0.5, 0.5, 1.0, 1.0 ); //!< Default for blue

    static const WColor DARKYELLOW( 0.5, 0.5, 0.0, 1.0 ); //!< Default for yellow
    static const WColor YELLOW( 1.0, 1.0, 0.0, 1.0 ); //!< Default for yellow
    static const WColor LIGHTYELLOW( 1.0, 1.0, 0.5, 1.0 ); //!< Default for yellow

    static const WColor ORANGE( 1.0, 0.5, 0.0, 1.0 ); //!< Default for orange

    static const WColor VIOLET( 0.5, 0.0, 0.5, 1.0 ); //!< Default for violet (darker pink)
    static const WColor PINK( 1.0, 0.0, 1.0, 1.0 ); //!< Default for pink

    static const WColor TEAL( 0.0, 0.5, 0.5, 1.0 ); //!< Default for darker cyan (teal)
    static const WColor CYAN( 0.0, 1.0, 1.0, 1.0 ); //!< Default for cyan

    static const WColor BLACK( 0.0, 0.0, 0.0, 1.0 ); //!< Default for black
    static const WColor GRAY25( 0.25, 0.25, 0.25, 1.0 ); //!< Default for gray
    static const WColor GRAY50( 0.5, 0.5, 0.5, 1.0 ); //!< Default for gray
    static const WColor GRAY75( 0.75, 0.75, 0.75, 1.0 ); //!< Default for gray
    static const WColor WHITE( 1.0, 1.0, 1.0, 1.0 ); //!< Default for white
    static const WColor TRANSPARENT( 1.0, 1.0, 1.0, 0.0 ); //!< Completely transparent  -- NOTE: this does not belong to the default palette
    // \endcond

    /**
     * The size of the default palette
     */
    static const size_t DefaultPaletteSize = 22;

    /**
     * This array is a simple palette definition containing the above colors. This palette is probably not very usable for all your special
     * needs. DO NOT add thousand other palettes here. Define them for yourself, in your module.
     */
    extern const WColor DefaultPalette[22];
}

#endif  // WCOLOR_H
