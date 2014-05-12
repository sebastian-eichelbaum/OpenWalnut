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

#include <osg/Geode>
#include <osg/ShapeDrawable>

#include "core/graphicsEngine/WGEGeodeUtils.h"

#include "WDemoGeometry.h"

osg::ref_ptr< osg::Node > WDemoGeometry::createSphereGeometry()
{
    // Create some spheres using the OSG functionality.  To understand the following code, we would like to refer you to the OpenSceneGraph
    // documentation.

    // add a bunch of spheres to a group and return it. Normals are set but no color.
    osg::ref_ptr< osg::Group > group( new osg::Group );

    // add 5 spheres with increasing size and add them along the X axis
    float x = 2.0;
    for( size_t i = 0; i < 5; ++i )
    {
        // Create a sphere.
        osg::Geode* sphereGeode = new osg::Geode;
        sphereGeode->addDrawable(
            new osg::ShapeDrawable(
                new osg::Sphere(
                    osg::Vec3d( x, 50.0, 2 + i * 3 ),   // moving center
                    2 + i * 3                           // increasing radius
                )
            )
        );

        // move the spheres along the X axis a bit each time
        x += 2 + 2 * i * 3 + 10.0;

        // Add to group.
        group->addChild( sphereGeode );
    }
    return group;
}

osg::ref_ptr< osg::Node > WDemoGeometry::createPlaneGeometry()
{
    // Create a nice plane. We use a pre-defined function in OpenWalnut's graphics engine:
    return wge::genFinitePlane( osg::Vec3( 0.0, 0.0, 0.0 ),   // base
                                osg::Vec3( 100.0, 0.0, 0.0 ), // spanning vector a
                                osg::Vec3( 0.0, 100.0, 0.0 ), // spanning vector b
                                WColor( 0.5, 0.5, 0.5, 1.0 )  // a color.
           );
}

