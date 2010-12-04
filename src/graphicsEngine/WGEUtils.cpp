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

#include <algorithm>
#include <vector>

#include <osg/Array>

#include "../common/math/WPosition.h"
#include "WGEUtils.h"

osg::ref_ptr< osg::Vec3Array > wge::osgVec3Array( const std::vector< wmath::WPosition >& posArray )
{
    osg::ref_ptr< osg::Vec3Array > result = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    result->reserve( posArray.size() );
    std::vector< wmath::WPosition >::const_iterator cit;
    for( cit = posArray.begin(); cit != posArray.end(); ++cit )
    {
        result->push_back( *cit );
    }
    return result;
}

osg::Vec3 wge::unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera )
{
    return screen * osg::Matrix::inverse( camera->getViewMatrix() * camera->getProjectionMatrix() * camera->getViewport()->computeWindowMatrix() );
}

WColor wge::createColorFromIndex( int index )
{
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
    float mult = 1.0;

    if ( index == 0 )
    {
        return WColor( 0.0, 0.0, 0.0 );
    }

    if ( ( index & 1 ) == 1 )
    {
        b = 1.0;
    }
    if ( ( index & 2 ) == 2 )
    {
        g = 1.0;
    }
    if ( ( index & 4 ) == 4 )
    {
        r = 1.0;
    }
    if ( ( index & 8 ) == 8 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            g = 1.0;
        }
    }
    if ( ( index & 16 ) == 16 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            b = 1.0;
            g = 1.0;
        }
    }
    if ( ( index & 32 ) == 32 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
            b = 1.0;
        }
    }
    if ( ( index & 64 ) == 64 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            g = 1.0;
        }
    }
    if ( ( index & 128 ) == 128 )
    {
        mult -= 0.15;
        if ( r < 1.0 && g < 1.0 && b < 1.0 )
        {
            r = 1.0;
        }
    }
    r *= mult;
    g *= mult;
    b *= mult;

    return WColor( r, g, b );
}

WColor wge::createColorFromHSV( int h, float s, float v )
{
    h = h % 360;

    int hi = h / 60;
    float f =  ( static_cast<float>( h ) / 60.0 ) - hi;

    float p = v * ( 1.0 - s );
    float q = v * ( 1.0 - s * f );
    float t = v * ( 1.0 - s * ( 1.0 - f ) );

    switch ( hi )
    {
        case 0:
            return WColor( v, t, p, 1.0 );
        case 1:
            return WColor( q, v, p, 1.0 );
        case 2:
            return WColor( p, v, t, 1.0 );
        case 3:
            return WColor( p, q, v, 1.0 );
        case 4:
            return WColor( t, p, v, 1.0 );
        case 5:
            return WColor( v, p, q, 1.0 );
        case 6:
            return WColor( v, t, p, 1.0 );
        default:
            return WColor( v, t, p, 1.0 );
    }
}

WColor wge::getNthHSVColor( int n, int parts )
{
    parts = (std::max)( 1, parts );
    if ( parts > 360 )
    {
        parts = 360;
    }
    int frac = 360 / parts;

    if ( parts > 320 )
    {
        return createColorFromHSV( n * frac );
    }
    else
    {
        int preset[320] = {
          0, 180, 90, 270, 45, 225, 135, 315, 22, 202, 112, 292, 67, 247, 157, 337, 11, 191, 101, 281, 56, 236, 146, 326, 33, 213, 123, 303, 78, 258,
          168, 348, 5, 185, 95, 275, 50, 230, 140, 320, 27, 207, 117, 297, 72, 252, 162, 342, 16, 196, 106, 286, 61, 241, 151, 331, 38, 218, 128,
          308, 83, 263, 173, 353, 2, 182, 92, 272, 47, 227, 137, 317, 24, 204, 114, 294, 69, 249, 159, 339, 13, 193, 103, 283, 58, 238, 148, 328, 35,
          215, 125, 305, 80, 260, 170, 350, 7, 187, 97, 277, 52, 232, 142, 322, 29, 209, 119, 299, 74, 254, 164, 344, 18, 198, 108, 288, 63, 243,
          153, 333, 40, 220, 130, 310, 85, 265, 175, 355, 1, 181, 91, 271, 46, 226, 136, 316, 23, 203, 113, 293, 68, 248, 158, 338, 12, 192, 102,
          282, 57, 237, 147, 327, 34, 214, 124, 304, 79, 259, 169, 349, 6, 186, 96, 276, 51, 231, 141, 321, 28, 208, 118, 298, 73, 253, 163, 343, 17,
          197, 107, 287, 62, 242, 152, 332, 39, 219, 129, 309, 84, 264, 174, 354, 3, 183, 93, 273, 48, 228, 138, 318, 25, 205, 115, 295, 70, 250,
          160, 340, 14, 194, 104, 284, 59, 239, 149, 329, 36, 216, 126, 306, 81, 261, 171, 351, 8, 188, 98, 278, 53, 233, 143, 323, 30, 210, 120,
          300, 75, 255, 165, 345, 19, 199, 109, 289, 64, 244, 154, 334, 41, 221, 131, 311, 86, 266, 176, 356, 4, 184, 94, 274, 49, 229, 139, 319, 26,
          206, 116, 296, 71, 251, 161, 341, 15, 195, 105, 285, 60, 240, 150, 330, 37, 217, 127, 307, 82, 262, 172, 352, 9, 189, 99, 279, 54, 234,
          144, 324, 31, 211, 121, 301, 76, 256, 166, 346, 20, 200, 110, 290, 65, 245, 155, 335, 42, 222, 132, 312, 87, 267, 177, 357
        };
        return createColorFromHSV( preset[n] );
    }
}
