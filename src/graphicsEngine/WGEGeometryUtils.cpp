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

#include <vector>

#include <osg/Array>

#include "../math/WPosition.h"
#include "WGEGeometryUtils.h"
#include "WGEUtils.h"

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuads( const std::vector< wmath::WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    // Surfaces
    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );
    vertices->push_back( wge::osgVec3( corners[3] ) );

    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );

    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[4] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );

    vertices->push_back( wge::osgVec3( corners[4] ) );
    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[3] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );

    vertices->push_back( wge::osgVec3( corners[3] ) );
    vertices->push_back( wge::osgVec3( corners[2] ) );
    vertices->push_back( wge::osgVec3( corners[6] ) );
    vertices->push_back( wge::osgVec3( corners[7] ) );

    vertices->push_back( wge::osgVec3( corners[0] ) );
    vertices->push_back( wge::osgVec3( corners[1] ) );
    vertices->push_back( wge::osgVec3( corners[5] ) );
    vertices->push_back( wge::osgVec3( corners[4] ) );
    return vertices;
}

osg::Vec3 wge::getQuadNormal( const wmath::WPosition& a,
                              const wmath::WPosition& b,
                              const wmath::WPosition& c )
{
    wmath::WPosition vec1 = a - b;
    wmath::WPosition vec2 = c - b;
    wmath::WPosition normal = vec2.crossProduct( vec1 );
    normal.normalize();
    return wge::osgVec3( normal );
}

osg::ref_ptr< osg::Vec3Array > wge::generateCuboidQuadNormals( const std::vector< wmath::WPosition >& corners )
{
    osg::ref_ptr< osg::Vec3Array > vertices = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );

    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[2] ) );
    vertices->push_back( getQuadNormal( corners[1], corners[5], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[5], corners[4], corners[7] ) );
    vertices->push_back( getQuadNormal( corners[4], corners[0], corners[3] ) );
    vertices->push_back( getQuadNormal( corners[3], corners[2], corners[6] ) );
    vertices->push_back( getQuadNormal( corners[0], corners[1], corners[5] ) );
    return vertices;
}
