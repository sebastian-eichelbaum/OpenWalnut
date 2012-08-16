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

#ifndef WGEUTILS_H
#define WGEUTILS_H

#include <string>
#include <vector>


#include <osg/Array>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Camera>

#include "../common/WColor.h"
#include "../common/WAssert.h"
#include "../common/math/linearAlgebra/WLinearAlgebra.h"

namespace wge
{
    /**
     * Enable transparency for the given node. This enabled blending and sets the node to the transparency bin.
     *
     * \param node the node
     */
    void enableTransparency( osg::ref_ptr< osg::Node > node );

    /**
     * Transforms a direction given via two points into a RGB color.
     *
     * \param pos1 First point
     * \param pos2 Second point
     *
     * \return converts a vector to a color
     */
    WColor getRGBAColorFromDirection( const WPosition &pos1, const WPosition &pos2 );

    /**
     * Converts a whole vector of WPositions into an osg::Vec3Array.
     *
     * \param posArray The given positions vector
     *
     * \return Refernce to the same vector but as osg::Vec3Array.
     */
    osg::ref_ptr< osg::Vec3Array > osgVec3Array( const std::vector< WPosition >& posArray );

    /**
     * Converts screen coordinates into Camera coordinates.
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     *
     * \return un-projects a screen coordinate back to world space
     */
    osg::Vec3 unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * Converts screen coordinates into Camera coordinates.
     * \note this method can be useful to work with vectors (w component 0)
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     *
     * \return un-projects a screen coordinate back to world space
     */
    osg::Vec4 unprojectFromScreen( const osg::Vec4 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * creates the same color as the atlas colormap shader from the index
     *
     * \param index unsigned char that indexes the color
     * \return the color
     */
    WColor createColorFromIndex( int index );

    /**
     * creates a rgb WColor from a HSV value
     * \param h hue
     * \param s saturation
     * \param v value
     * \return the color
     */
    WColor createColorFromHSV( int h, float s = 1.0, float v = 1.0 );

    /**
     * creates the nth color of a partition of the hsv color circle
     *
     * \param n number of the color
     * \return the color
     */
    WColor getNthHSVColor( int n );
}

inline WColor wge::getRGBAColorFromDirection( const WPosition &pos1, const WPosition &pos2 )
{
    WPosition direction( normalize( pos2 - pos1 ) );
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ), 1.0f );
}

#endif  // WGEUTILS_H

