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

#ifndef WGRID_TEST_H
#define WGRID_TEST_H

#include <utility>

#include <cxxtest/TestSuite.h>

#include "../../common/WProperties.h"
#include "../WGrid.h"

/**
 * Dummy class for testing the abstract class WGrid
 */
class Dummy : public WGrid
{
friend class WGridTest;

public:
    /**
     * Standard constructor of Dummy class.
     * \param size number of positions
     */
    explicit Dummy( size_t size )
        : WGrid( size )
    {
    }

    /**
     * Returns dummy bounding box.
     *
     * \return bbox
     */
    virtual WBoundingBox getBoundingBox() const
    {
        return WBoundingBox( WPosition( 0, 0, 0 ), WPosition( 1, 1, 1 ) );
    }
};

/**
 * Tests the WGrid class.
 */
class WGridTest : public CxxTest::TestSuite
{
public:
    /**
     *  Checks if the Dummy is instanceable.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( Dummy d( 1 ) );
    }

    /**
     * After instantiation there should be only 1 positions.
     */
    void testSize( void )
    {
        Dummy grid( 1 );
        TS_ASSERT_EQUALS( grid.size(), 1 );
    }
};

#endif  // WGRID_TEST_H
