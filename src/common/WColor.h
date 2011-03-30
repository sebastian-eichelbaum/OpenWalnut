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

#include <iostream>

#include <osg/Vec4>
#include <osg/io_utils> // for the operator<< and operator>> for Vec4

#include "WExportCommon.h"

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
WColor OWCOMMON_EXPORT convertHSVtoRGBA( double h, double s, double v );

/**
 * Computes the inverse of this color in means of RGB space. The alpha value is untouched.
 *
 * \ingroup Color utils
 *
 * \param other The color (RGBA) from which the inverse should be calculated.
 */
WColor OWCOMMON_EXPORT inverseColor( const WColor& other );

/**
 * Some default colors.
 */
namespace defaultColor
{
    // \cond
    static const WColor GREEN( 0.0, 1.0, 0.0, 1.0 ); //!< Default for green
    static const WColor RED( 1.0, 0.0, 0.0, 1.0 ); //!< Default for red
    static const WColor BLUE( 0.0, 0.0, 1.0, 1.0 ); //!< Default for blue
    static const WColor BLACK( 0.0, 0.0, 0.0, 1.0 ); //!< Default for black
    static const WColor WHITE( 1.0, 1.0, 1.0, 1.0 ); //!< Default for black
    // \endcond
}

#endif  // WCOLOR_H
