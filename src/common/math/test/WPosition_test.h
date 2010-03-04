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

#ifndef WPOSITION_TEST_H
#define WPOSITION_TEST_H

#include <cxxtest/TestSuite.h>
#include "../WPosition.h"

/**
 * Testsuite for WPosition.
 */
class WPositionTest : public CxxTest::TestSuite
{
public:
    /**
     * Tests nothing as WPosition is only an alias for WVector3D.
     * See the tests for WVector3D for methods testing the behavior.
     */
    void testNothing( void )
    {
        TS_TRACE( "Nothing to test for WPosition. See the tests of WVector3D." );
    }
};

#endif  // WPOSITION_TEST_H
