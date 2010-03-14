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

#include <cxxtest/TestSuite.h>

#include "../WPlane.h"

/**
 * Unit tests the WPlane class
 */
class WPlaneTest : public CxxTest::TestSuite
{
public:
    /**
     * TODO(math): Document this!
     */
    void testSamplePointsFromCenter( void )
    {
        WPlane p( wmath::WVector3D( 0, 0, 1 ), wmath::WPosition( 1, 1, 1 ) );
        // boost::shared_ptr< WGridRegular3D > grid( new WGridRegular3D( ) );
    }
};

#endif  // WPLANE_TEST_H
