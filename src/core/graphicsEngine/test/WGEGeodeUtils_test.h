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

#ifndef WGEGEODEUTILS_TEST_H
#define WGEGEODEUTILS_TEST_H

#include <cxxtest/ValueTraits.h>
#include <cxxtest/TestSuite.h>

#include <osg/io_utils>

#include "../../common/WStringUtils.h"
#include "WVec3Traits.h"
#include "../WGEGeodeUtils.h"

/**
 * Testsuite for the WGEGeode utils.
 */
class WGEGeodeUtilsTest : public CxxTest::TestSuite
{
public:
    /**
     * A unit subdivided plane with resolution width_x_height has width*height many quads.
     */
    void testNumQuadsAndTheirVerticesWithoutSpacing( void )
    {
        double spacing = 0.0;
        osg::ref_ptr< WGESubdividedPlane > g = wge::genUnitSubdividedPlane( 2, 2, spacing );
        osg::Geometry *geo = dynamic_cast< osg::Geometry* >( g->getDrawable( 0 ) );
        if( !geo )
        {
            TS_FAIL( "The drawable inside the WGESubdividedPlane geode is not a geometry" );
        }
        TS_ASSERT( geo );
        osg::Vec3Array* verts =  dynamic_cast< osg::Vec3Array* >( geo->getVertexArray() );
        if( !verts )
        {
            TS_FAIL( "The vertex array inside is not a osg::Vec3Array" );
        }
        osg::ref_ptr< osg::Vec3Array > expected = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        expected->push_back( osg::Vec3( 0.0, 0.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 0.0, 0.0 ) );
        expected->push_back( osg::Vec3( 2.0, 0.0, 0.0 ) );
        expected->push_back( osg::Vec3( 2.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 0.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 2.0, 0.0 ) );
        expected->push_back( osg::Vec3( 0.0, 2.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 2.0, 1.0, 0.0 ) );
        expected->push_back( osg::Vec3( 2.0, 2.0, 0.0 ) );
        expected->push_back( osg::Vec3( 1.0, 2.0, 0.0 ) );
        TS_ASSERT_EQUALS( verts->asVector(), expected->asVector() );
    }

    /**
     * For each quad there is a center point defined in the center point array.
     */
    void testCenterPoints( void )
    {
        osg::ref_ptr< WGESubdividedPlane > g = wge::genUnitSubdividedPlane( 2, 2 );
        osg::ref_ptr< osg::Vec3Array > expected = osg::ref_ptr< osg::Vec3Array >( new osg::Vec3Array );
        expected->push_back( osg::Vec3( 0.5, 0.5, 0.0 ) );
        expected->push_back( osg::Vec3( 1.5, 0.5, 0.0 ) );
        expected->push_back( osg::Vec3( 0.5, 1.5, 0.0 ) );
        expected->push_back( osg::Vec3( 1.5, 1.5, 0.0 ) );
        TS_ASSERT_EQUALS( g->getCenterArray()->asVector(), expected->asVector() );
    }
};

#endif  // WGEGEODEUTILS_TEST_H
