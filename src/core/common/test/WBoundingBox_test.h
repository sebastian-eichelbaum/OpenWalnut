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

#ifndef WBOUNDINGBOX_TEST_H
#define WBOUNDINGBOX_TEST_H

#include <cxxtest/TestSuite.h>

#include <osg/Vec3d>
#include "../math/linearAlgebra/WPosition.h"

#include "../WBoundingBox.h"
#include "../WLimits.h"

/**
 * Unit tests for the WBoundingBox wrapper to osg::BoundingBox.
 */
class WBoundingBoxTest : public CxxTest::TestSuite
{
public:
    /**
     * The private subclassing from osg::BoundingBoxImpl makes many member functions private.  The
     * using directive again will publish those members selectively. This is tested in this test.
     */
    void testForwardingFunctions( void )
    {
        WBoundingBox bb( 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 );
        WBoundingBox bb1( bb );
        WBoundingBox bb2;
        WBoundingBox bb3( WVector3d( 0.0, 0.0, 0.0 ), WVector3d( 1.0, 1.0, 1.0 ) );
        WBoundingBox bb4( osg::Vec3d( 0.0, 0.0, 0.0 ), osg::Vec3d( 1.0, 1.0, 1.0 ) );
        bb4.expandBy( bb3 );
        TS_ASSERT( bb4.intersects( bb3 ) );
        TS_ASSERT_EQUALS( bb2.valid(), false );
        bb2.reset();
    }

    /**
     * The minimal distance between two bounding boxes is the minimal distance overall vertices of
     * the first bb to every vertex in the second bb.
     */
    void testMinimalDistanceBetweenTwoBB( void )
    {
        WBoundingBox bb1( 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 );
        WBoundingBox bb2( 1.5, 0.5, 0.0, 2.5, 1.5, 1.0 );
        TS_ASSERT_DELTA( bb1.minDistance( bb2 ), 0.5, wlimits::DBL_EPS );
    }

    /**
     * The distance should not depend on the order in which the boxes are given.
     */
    void testCommutativeIntervalDistance( void )
    {
        WBoundingBox bb1( 0.0, 0.0, 0.0, 1.0, 1.0, 1.0 );
        WBoundingBox bb2( 1.5, 0.5, 0.0, 2.5, 1.5, 1.0 );
        TS_ASSERT_DELTA( bb1.minDistance( bb2 ), 0.5, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( bb2.minDistance( bb1 ), 0.5, wlimits::DBL_EPS );
    }

    /**
     * Expanding a bounding box by points should update the both corner positions.
     */
    void testBoundingBoxComputation( void )
    {
        WBoundingBox box;
        TS_ASSERT( !box.valid() );
        box.expandBy( WPosition( 0.0, 0.0, 0.0 ) );
        TS_ASSERT( box.valid() );
        TS_ASSERT_DELTA( box.xMin(), 0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.yMin(), 0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.zMin(), 0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.xMax(), 0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.yMax(), 0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.zMax(), 0.0, wlimits::DBL_EPS );
        box.expandBy( WPosition(  1.0, 0.0, 0.0 ) );
        box.expandBy( WPosition( -1.0, 0.0, 0.0 ) );
        box.expandBy( WPosition( -1.0, 3.0, 0.0 ) );
        TS_ASSERT_DELTA( box.xMin(), -1.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.yMin(),  0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.zMin(),  0.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.xMax(),  1.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.yMax(),  3.0, wlimits::DBL_EPS );
        TS_ASSERT_DELTA( box.zMax(),  0.0, wlimits::DBL_EPS );
    }
};

#endif  // WBOUNDINGBOX_TEST_H
