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
#include "../../../dataHandler/WDataSetFibers.h"

/**
 * Unit test the WMFiberDisplay
 */
class WMFiberDisplayTest : public CxxTest::TestSuite
{
public:
    /**
     * In almost every test we need a sample WDataSetFibers.
     */
    void setUp( void )
    {
        using wmath::WPosition;
        wmath::WFiber fiber;
        fiber.push_back( WPosition( 0., 0., 0. ) );
        fiber.push_back( WPosition( 1., 0., 0. ) );
        fiber.push_back( WPosition( 1., 1., 0. ) );
        fiber.push_back( WPosition( 1., 1., 1. ) );
        boost::shared_ptr< std::vector< wmath::WFiber > > fiberVector( new std::vector< wmath::WFiber >() );
        fiberVector->push_back( fiber );
        m_fiberDS = boost::shared_ptr< const WDataSetFibers >( new WDataSetFibers( fiberVector ) );
    }

    /**
     * Discard any precreated data sets.
     */
    void tearDown( void )
    {
        m_fiberDS.reset();
    }

    /**
     * The OSG geometry of a WFiber instance should have either arrays for
     * colors and vertices of the same size if localColoring is selected.
     */
    void testGeometryOfFiberOnLocalColoring( void )
    {
        WMFiberDisplay mod;
        osg::ref_ptr< osg::Geode > result = mod.genFiberGeode( m_fiberDS, false );
        osg::Geometry *geo = result->getDrawable( 0 )->asGeometry();
        TS_ASSERT_EQUALS( geo->getVertexArray()->getNumElements(), 4 );
        TS_ASSERT_EQUALS( geo->getVertexArray()->getNumElements(), geo->getColorArray()->getNumElements() );
    }

    /**
     * When global coloring is used, only one color should be there.
     */
    void testGeometryOfFiberOnGlobalColoring( void )
    {
        WMFiberDisplay mod;
        osg::ref_ptr< osg::Geode > result = mod.genFiberGeode( m_fiberDS );
        osg::Geometry *geo = result->getDrawable( 0 )->asGeometry();
        TS_ASSERT_EQUALS( geo->getVertexArray()->getNumElements(), 4 );
        // EVEN IF THERE ARE JUST ONE COLOR PER FIBER EVERY VERTEX HAS A COLOR
        // Since I don't know how to do it without, I assume BIND_OVERALL won't work
        TS_ASSERT_EQUALS( 4, geo->getColorArray()->getNumElements() );
    }

private:
    boost::shared_ptr< const WDataSetFibers > m_fiberDS; //!< default fiber dataset
};

#endif  // WMFIBERDISPLAY_TEST_H
