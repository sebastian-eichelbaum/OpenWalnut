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

#include <boost/lexical_cast.hpp>

#include <osg/Array>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Camera>

#include "../common/WColor.h"
#include "../common/WAssert.h"
#include "../common/math/WPosition.h"
#include "../common/math/WMatrix.h"
#include "WExportWGE.h"

namespace wge
{
    /**
     * Transforms a direction given via two points into a RGB color.
     *
     * \param pos1 First point
     * \param pos2 Second point
     */
    WColor getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 );

    /**
     * Converts a whole vector of WPositions into an osg::Vec3Array.
     *
     * \param posArray The given positions vector
     *
     * \return Refernce to the same vector but as osg::Vec3Array.
     */
    osg::ref_ptr< osg::Vec3Array > WGE_EXPORT osgVec3Array( const std::vector< wmath::WPosition >& posArray );

    /**
     * Converts screen coordinates into Camera coordinates.
     *
     * \param screen the screen coordinates
     * \param camera The matrices of this camera will used for unprojecting.
     */
    osg::Vec3 WGE_EXPORT unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera  );

    /**
     * creates the same color as the atlas colormap shader from the index
     *
     * \param index unsigned char that indexes the color
     * \return the color
     */
    WColor WGE_EXPORT createColorFromIndex( int index );

    /**
     * creates a rgb WColor from a HSV value
     * \param h hue
     * \param s saturation
     * \param v value
     * \return the color
     */
    WColor WGE_EXPORT createColorFromHSV( int h, float s = 1.0, float v = 1.0 );

    /**
     * creates the nth color of a partition of the hsv color circle
     *
     * \param n number of the color
     * \return the color
     */
    WColor WGE_EXPORT getNthHSVColor( int n );

    /**
     * This method converts an WMatrix to the corresponding osg::Matrixd.
     *
     * \param matrix the matrix to convert
     *
     * \return the osg matrix.
     */
    osg::Matrixd WGE_EXPORT toOSGMatrix( const wmath::WMatrix<double>& matrix );
}

inline WColor wge::getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 )
{
    wmath::WPosition direction( ( pos2 - pos1 ) );
    direction.normalize();
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ), 1.0f );
}

inline osg::Matrixd wge::toOSGMatrix( const wmath::WMatrix<double>& matrix )
{
    WAssert( ( matrix.getNbRows() == 3 || matrix.getNbRows() == 4 ) && ( matrix.getNbCols() == 3 || matrix.getNbCols() == 4 ),
             "Only 3x3 or 4x4 matrices allowed." );

    // handle 4x4 and 3x3 separately
    if ( matrix.getNbRows() == 4 )
    {
        return osg::Matrixd( matrix[ 0 ], matrix[ 4 ], matrix[ 8 ], matrix[ 12 ],
                             matrix[ 1 ], matrix[ 5 ], matrix[ 9 ], matrix[ 13 ],
                             matrix[ 2 ], matrix[ 6 ], matrix[ 10 ], matrix[ 14 ],
                             matrix[ 3 ], matrix[ 7 ], matrix[ 11 ], matrix[ 15 ]
                           );
    }
    else
    {
        return osg::Matrixd( matrix[ 0 ], matrix[ 1 ], matrix[ 2 ], 0.0,
                             matrix[ 3 ], matrix[ 4 ], matrix[ 5 ], 0.0,
                             matrix[ 6 ], matrix[ 7 ], matrix[ 8 ], 0.0,
                             matrix[ 9 ], matrix[ 10 ], matrix[ 11 ], 1.0
                           );
    }
}

#endif  // WGEUTILS_H

