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

#ifndef WMFIBERDISPLAY_TEST_H
#define WMFIBERDISPLAY_TEST_H

#include <vector>

#include <cxxtest/TestSuite.h>

#include <osg/Geode>
#include <osg/Geometry>

#include "../WMFiberDisplay.h"
#include "../../../math/WPosition.h"
#include "../../../math/WFiber.h"

/**
 * Unit test the WMFiberDisplay
 */
class WMFiberDisplayTest : public CxxTest::TestSuite
{
public:
    /**
     * The OSG geometry of a WFiber instance should have either arrays for
     * colors and vertices of the same size.
     */
    void testGeometryOfFiber( void )
    {
        using wmath::WPosition;
        std::vector< WPosition > fibData;
        fibData.push_back( WPosition( 0., 0., 0. ) );
        fibData.push_back( WPosition( 1., 0., 0. ) );
        fibData.push_back( WPosition( 1., 1., 0. ) );
        fibData.push_back( WPosition( 1., 1., 1. ) );
        using wmath::WFiber;
        WFiber fib( fibData );
        WMFiberDisplay mod;
        osg::ref_ptr< osg::Geode > result = mod.genFiberGeode( fib );
        osg::Geometry *geo = result->getDrawable( 0 )->asGeometry();
        TS_ASSERT_EQUALS( geo->getVertexArray()->getNumElements(), 4 );
        TS_ASSERT_EQUALS( geo->getVertexArray()->getNumElements(), geo->getColorArray()->getNumElements() );
    }
};

#endif  // WMFIBERDISPLAY_TEST_H
