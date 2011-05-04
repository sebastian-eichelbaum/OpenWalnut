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

#ifndef WPLANE_TEST_H
#define WPLANE_TEST_H

#include <set>

#include <cxxtest/TestSuite.h>

#include "../../../common/math/test/WPositionTraits.h"
#include "../WPlane.h"

/**
 * Unit tests the WPlane class
 */
class WPlaneTest : public CxxTest::TestSuite
{
public:
//    /**
//     * Simple sampling test
//     */
//    void testSamplePointsFromCenter( void )
//    {
//        WPlane p( WVector3d( 0, 0, 1 ), WPosition( 1.5, 1.5, 1. ) );
//        WGridRegular3D grid( 2, 2, 2, 1., 1., 1. );
//        boost::shared_ptr< std::set< WPosition > > expected( new std::set< WPosition > );
//        expected->insert( WPosition(   1,   1, 1 ) );
//        expected->insert( WPosition( 1.5,   1, 1 ) );
//        expected->insert( WPosition(   2,   1, 1 ) );
//        expected->insert( WPosition(   1, 1.5, 1 ) );
//        expected->insert( WPosition( 1.5, 1.5, 1 ) );
//        expected->insert( WPosition(   2, 1.5, 1 ) );
//        expected->insert( WPosition(   1,   2, 1 ) );
//        expected->insert( WPosition( 1.5,   2, 1 ) );
//        expected->insert( WPosition(   2,   2, 1 ) );
//        using string_utils::operator<<;
//        TS_ASSERT_EQUALS( *expected, *p.samplePoints( grid, 0.5 ) );
//    }

    /**
     * A predefined number of sampling points with step width from the center position.
     */
    void testFixedSampling( void )
    {
        WPlane p( WVector3d( 0, 0, 1 ), WPosition( 1.5, 1.5, 1. ) );
        boost::shared_ptr< std::set< WPosition > > expected( new std::set< WPosition > );
        expected->insert( WPosition(   1,   1, 1 ) );
        expected->insert( WPosition(   1, 1.5, 1 ) );
        expected->insert( WPosition(   1,   2, 1 ) );
        expected->insert( WPosition( 1.5,   1, 1 ) );
        expected->insert( WPosition(   2,   1, 1 ) );
        expected->insert( WPosition( 1.5, 1.5, 1 ) );
        expected->insert( WPosition(   2, 1.5, 1 ) );
        expected->insert( WPosition(   1, 1.5, 1 ) );
        expected->insert( WPosition( 1.5,   2, 1 ) );
        expected->insert( WPosition(   2,   2, 1 ) );
        TS_ASSERT_EQUALS( *expected, *p.samplePoints( 0.5, 2, 2 ) );
     }
};

#endif  // WPLANE_TEST_H
