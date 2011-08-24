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

#ifndef WTALAIRACHCONVERTER_TEST_H
#define WTALAIRACHCONVERTER_TEST_H

#include <iostream>

#include <cxxtest/TestSuite.h>

#include <boost/shared_ptr.hpp>

#include "../WTalairachConverter.h"

/**
 * Tests the talailrach converter class
 */
class WTalairachConverterTest : public CxxTest::TestSuite
{
friend class WTalairachConverter;

public:
    /**
     * Set up the special points of the Talairach coordinate system
     * for testing the converter.
     */
    void setUp( void )
    {
        WVector3d ac( 150, 122, 90 );
        WVector3d pc( 110, 124, 97 );
        WVector3d ihp( 140, 119, 188 );
        m_tc = boost::shared_ptr<WTalairachConverter> ( new WTalairachConverter( ac, pc, ihp ) );

        m_tc->setAp( WVector3d( 80, 0, 0 ) );
        m_tc->setPp( WVector3d( -97, 0, 0 ) );
        m_tc->setSp( WVector3d( 0, 0, 81 ) );
        m_tc->setIp( WVector3d( 0, 0, -41 ) );
        m_tc->setRp( WVector3d( 0, -60, 0 ) );
        m_tc->setLp( WVector3d( 0, 63, 0 ) );
    }

    /**
     * Release all data need for testing the converter
     */
    void tearDown( void )
    {
        m_tc.reset();
    }


    /**
     * Test the conversion "to" ACPC coords.
     */
    void testCanonical2ACPC( void )
    {
        WVector3d testPoint( 200, 60, 130 );
        WVector3d result = m_tc->Canonical2ACPC( testPoint );

        TS_ASSERT_DELTA( result[0], 45.35, 0.05 );
        TS_ASSERT_DELTA( result[1], -57.6, 0.05 );
        TS_ASSERT_DELTA( result[2], 50.69, 0.05 );
    }

    /**
     * Test the conversion "from" ACPC coords.
     */
    void testACPC2Canonical( void )
    {
        WVector3d testPoint( 45.35, -57.6, 50.69 );
        WVector3d result = m_tc->ACPC2Canonical( testPoint );

        TS_ASSERT_DELTA( result[0], 200, 0.05 );
        TS_ASSERT_DELTA( result[1], 60, 0.05 );
        TS_ASSERT_DELTA( result[2], 130, 0.05 );
    }

    /**
     * Test conversion from ACPC to Talairach coords.
     */
    void testACPC2Talairach( void )
    {
        WVector3d testPoint( 12, 43, -12 );
        WVector3d result = m_tc->ACPC2Talairach( testPoint );

        TS_ASSERT_DELTA( result[0], 12.0, 0.05 );
        TS_ASSERT_DELTA( result[1], 49.6, 0.05 );
        TS_ASSERT_DELTA( result[2], -3.8, 0.05 );
    }

    /**
     * Test conversion from Talairach to ACPC coords.
     */
    void testTalairach2ACPC( void )
    {
        WVector3d testPoint( 10.5, 46.41, -12.29 );
        WVector3d result = m_tc->Talairach2ACPC( testPoint );

        TS_ASSERT_DELTA( result[0], 10.5, 0.05 );
        TS_ASSERT_DELTA( result[1], 40.25, 0.05 );
        TS_ASSERT_DELTA( result[2], -38.35, 0.05 );
    }


private:
    /**
     * The Talairach converter object that is tested in the testing functions.
     */
    boost::shared_ptr<WTalairachConverter> m_tc;
};

#endif  // WTALAIRACHCONVERTER_TEST_H
