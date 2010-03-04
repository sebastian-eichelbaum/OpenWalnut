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

#include <GL/glu.h>

#include "../common/math/WPosition.h"
#include "WGEUtils.h"

osg::ref_ptr< osg::Vec3Array > wge::osgVec3Array( const std::vector< wmath::WPosition >& posArray )
{
    osg::ref_ptr< osg::Vec3Array > result = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
    result->reserve( posArray.size() );
    std::vector< wmath::WPosition >::const_iterator cit;
    for( cit = posArray.begin(); cit != posArray.end(); ++cit )
    {
        result->push_back( wge::osgVec3( *cit ) );
    }
    return result;
}

osg::Vec3 wge::unprojectFromScreen( const osg::Vec3 screen, osg::ref_ptr< osg::Camera > camera )
{
    double* modelView;
    double* projection;
    double dviewport[4];

    modelView = camera->getViewMatrix().ptr();
    projection = camera->getProjectionMatrix().ptr();
    dviewport[0] = camera->getViewport()->x();
    dviewport[1] = camera->getViewport()->y();
    dviewport[2] = camera->getViewport()->width();
    dviewport[3] = camera->getViewport()->height();

    double x, y, z;
    GLint viewport[4];
    viewport[0] = static_cast< GLint >( dviewport[0] );
    viewport[1] = static_cast< GLint >( dviewport[1] );
    viewport[2] = static_cast< GLint >( dviewport[2] );
    viewport[3] = static_cast< GLint >( dviewport[3] );

    gluUnProject( screen[0], screen[1], screen[2], modelView, projection, viewport, &x, &y, &z );

    osg::Vec3 world;
    world[0] = x;
    world[1] = y;
    world[2] = z;

    return world;
}
