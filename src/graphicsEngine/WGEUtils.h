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

#include <vector>

#include <osg/Array>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Camera>

#include "../common/WColor.h"
#include "../common/math/WPosition.h"
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
     * Converts a WColor to an OSG compatible color
     *
     * \param color The color in WColor format
     * \return A color which may be used inside of OSG
     */
    osg::Vec4 osgColor( const WColor& color );

    /**
     * Converts a given WPosition into an osg::Vec3.
     *
     * \param pos The WPosition which should be converted
     *
     * \return The osg::Vec3 vector of pos
     */
    osg::Vec3 osgVec3( const wmath::WPosition& pos );

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
     * Conversion of WVector3D to osg::Vec3
     * \param v the vector to convert.
     */
    osg::Vec3 wv3D2ov3( wmath::WVector3D v );

    /**
     * creates the same color as the atlas colormap shader from the index
     *
     * \param index unsigned char that indexes the color
     * \return the color
     */
    WColor WGE_EXPORT createColorFromIndex( int index );
}

inline WColor wge::getRGBAColorFromDirection( const wmath::WPosition &pos1, const wmath::WPosition &pos2 )
{
    wmath::WPosition direction( ( pos2 - pos1 ) );
    direction.normalize();
    return WColor( std::abs( direction[0] ), std::abs( direction[1] ), std::abs( direction[2] ) );
}

inline osg::Vec4 wge::osgColor( const WColor& color )
{
    return osg::Vec4( color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha() );
}

inline osg::Vec3 wge::osgVec3( const wmath::WPosition& pos )
{
    return osg::Vec3( pos[0], pos[1], pos[2] );
}

inline osg::Vec3 wge::wv3D2ov3( wmath::WVector3D v )
{
    return osg::Vec3( v[0], v[1], v[2] );
}

#endif  // WGEUTILS_H
